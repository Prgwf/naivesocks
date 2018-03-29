add_executable(client client.c
        ../ns-utility/base64.c
        ../ns-utility/cipher.c
        ../ns-utility/securesocket.c)


target_link_libraries(client ${LIBEVENT_LIB})

set(LIBEVENT_LIB "-levent")
