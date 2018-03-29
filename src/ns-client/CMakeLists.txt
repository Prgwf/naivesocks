link_directories(${CMAKE_BINARY_DIR}/lib)

add_executable(client client.c)
target_link_libraries(client ${LIBEVENT_LIB} cipher)

set(LIBEVENT_LIB "-levent")
