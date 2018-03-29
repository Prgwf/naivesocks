add_executable(ns-key cipher.c base64.c)

add_library(cipher cipher.c)
add_library(securesocket securesocket.c cipher.c)

set(LIBEVENT_LIB "-levent")
target_link_libraries(securesocket ${LIBEVENT_LIB})



