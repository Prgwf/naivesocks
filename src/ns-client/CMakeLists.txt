link_directories(${CMAKE_BINARY_DIR}/lib)

add_executable(ns-client client.c)
target_link_libraries(ns-client ${LIBEVENT_LIB} cipher)

set(LIBEVENT_LIB "-levent")
