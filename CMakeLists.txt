cmake_minimum_required(VERSION 3.9)
project(naivesocks)

set(CMAKE_CXX_STANDARD 11)

add_executable(naivesocks
        src/cipher.c src/cipher.h src/base64.c src/base64.h src/client.c src/client.h src/server.c src/server.h src/udprelay.c src/udprelay.h)


