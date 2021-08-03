cmake_minimum_required(VERSION 3.17)
project(Openu_Project C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -ansi -pedantic")

add_executable(Openu_Project assembler.c labelsDB.c labelsDB.h data.h tests.h operationsDB.c operationsDB.h pandas.c pandas.h tests.c dataImageDB.c dataImageDB.h codeImageDB.c codeImageDB.h labelCallsDB.c labelCallsDB.h entryCallsDB.c entryCallsDB.h)