add_library(cipher cipher.c)
add_library(securesocket securesocket.c cipher.c)

add_executable(ns-key cipher.c base64.c)