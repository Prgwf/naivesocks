add_executable(server server.c)
target_link_libraries(server ${LIBEVENT_LIB} base64 cipher securesocket)

set(LIBEVENT_LIB "-levent")
