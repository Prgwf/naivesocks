add_executable(ns-server server.c)
target_link_libraries(ns-server ${LIBEVENT_LIB} base64 cipher securesocket)

set(LIBEVENT_LIB "-levent")
