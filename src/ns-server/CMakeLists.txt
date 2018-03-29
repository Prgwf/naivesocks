add_executable(server server.c)

target_link_libraries(server ${LIBEVENT_LIB})

set(LIBEVENT_LIB "-levent")
