link_directories(${CMAKE_BINARY_DIR}/lib)

add_executable(ns-server server.c)
target_link_libraries(ns-server ${LIBEVENT_LIB} cipher)

set(LIBEVENT_LIB "-levent")
