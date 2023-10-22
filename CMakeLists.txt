cmake_minimum_required(VERSION 3.5)
project(STYLE-FORMATTER)

set(C_SOURCES 
  src/main.c
  src/stack.h
)
add_executable(${PROJECT_NAME} ${C_SOURCES})
