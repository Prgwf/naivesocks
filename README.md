## Thanks BOSS Qian!

### Client
1. listen requests from browser
2. encrypt the data
3. forward data to the server
4. receive the data from server
5. decrypt the data and then send to browser

### Server
1. listen requests from client
2. decrypt the data with socks5 protocol
3. get the content from web server
4. encrypt the data
5. forward data to the client