# Create the executable target
add_executable(${CMAKE_PROJECT_NAME}
    main.c
)

# Show the section sizes of the elf file
print_sizes(${CMAKE_PROJECT_NAME})
