link_directories(${CMAKE_BINARY_DIR}/lib)

add_executable(ns-key getkey.c)
target_link_libraries(ns-key cipher)

add_library(base64 base64.c)

add_library(cipher cipher.c)
target_link_libraries(cipher ${LIBEVENT_LIB} base64)

set(LIBEVENT_LIB "-levent")



