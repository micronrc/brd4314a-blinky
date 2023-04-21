/*
 * Simple blinky example using the sdk_support library on a
 * BRD4314A dev board (EFR32BG22C224F512GN32)
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_rtcc.h"
#include "em_gpio.h"

#include "sl_iostream.h"
#include "sl_iostream_uart.h"
#include "sl_iostream_usart.h"
#include "em_usart.h"

#include "sl_sleeptimer.h"

#include <stdio.h>
#include <string.h>

// BRD4314A LED on PA04
#define LED   4
#define TOOGLE_DELAY_MS         500

static sl_iostream_uart_t uart_iostream;
sl_iostream_t *uart_iostream_handle = &uart_iostream.stream;
static sl_iostream_usart_context_t context_usart;

#define RX_BUFFER_SIZE  32
static uint8_t rx_buffer[32];

void led_off(void);

void iostream_init(void)
{
  USART_InitAsync_TypeDef init_usart = USART_INITASYNC_DEFAULT;
  init_usart.baudrate = 115200;
  init_usart.parity = usartNoParity;
  init_usart.stopbits = usartStopbits1;

  sl_iostream_usart_config_t usart_config = {
    .usart = USART1,
    .clock = cmuClock_USART1,
    .tx_port = gpioPortA, .tx_pin = 5,
    .rx_port = gpioPortA, .rx_pin = 6,
    .usart_index = 1,
  };

  sl_iostream_uart_config_t uart_config = {
    .tx_irq_number = USART1_TX_IRQn,
    .rx_irq_number = USART1_RX_IRQn,
    .rx_buffer = rx_buffer,
    .rx_buffer_length = RX_BUFFER_SIZE,
    .lf_to_crlf = 1,        // convert LF to CRLF
    .rx_when_sleeping = 1,
  };

  sl_status_t status = sl_iostream_usart_init(
                                  &uart_iostream,
                                  &uart_config,
                                  &init_usart,
                                  &usart_config,
                                  &context_usart);
  if (status != SL_STATUS_OK) {
    led_off();
    while(1) {}
  }

  /* Set unbuffered mode for stdout */
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stdin, NULL, _IONBF, 0);
}

void USART1_RX_IRQHandler(void)
{
  sl_iostream_usart_irq_handler(uart_iostream.stream.context);
}

void USART1_TX_IRQHandler(void)
{
  sl_iostream_usart_irq_handler(uart_iostream.stream.context);
}

static sl_sleeptimer_timer_handle_t timer;
static bool toggle_timeout = false;
static void on_timeout(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)&handle;
  (void)&data;
  toggle_timeout = true;
}

int main() 
{
  CHIP_Init();

  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize LED driver
  GPIO_PinModeSet(gpioPortA, LED, gpioModePushPull, 0);
  GPIO_PinOutSet(gpioPortA, LED);

  iostream_init();

  const char msg1[] = "message 1\n";
  sl_iostream_write(uart_iostream_handle, msg1, strlen(msg1));

  sl_iostream_set_default(uart_iostream_handle);
  const char msg2[] = "message 2\n";
  sl_iostream_write(SL_IOSTREAM_STDOUT, msg2, strlen(msg2));

  printf("blink started\n");

  sl_sleeptimer_init();
  sl_sleeptimer_start_periodic_timer_ms(&timer,
      TOOGLE_DELAY_MS, on_timeout, NULL, 0,
      SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);

  printf("type something> ");
  fflush(stdout);

  while (1) {
    if (toggle_timeout == true) {
      GPIO_PinOutToggle(gpioPortA, LED);
      toggle_timeout = false;
    }

#define BUFSIZE   120
    static char buffer[BUFSIZE];
    static uint8_t index = 0;

    int8_t c = getchar();
    if (c > 0) {
      if (c == '\r' || c == '\n') {
        buffer[index] = '\0';
        printf("\nYou wrote: %s\n> ", buffer);
        index = 0;
      } else {
        if (index < BUFSIZE - 1) {
          buffer[index] = c;
          index++;
        }
        /* Local echo */
        putchar(c);
      }
    }
  }
}

void led_off()
{
  GPIO_PinOutClear(gpioPortA, LED);
}
