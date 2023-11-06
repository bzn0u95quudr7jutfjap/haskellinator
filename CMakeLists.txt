cmake_minimum_required(VERSION 3.5)
project(STYLE-FORMATTER)

set(C_SOURCES 
  src/main.c
  src/string_class.c
  src/stack_string.c
  src/parenthesis.c
)

add_compile_options("-g")

add_executable(${PROJECT_NAME} ${C_SOURCES})
