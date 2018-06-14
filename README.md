# WindowsSocketsFileServer
Simple file transfer server/client based on Windows sockets .

This project contains a server side implementation and a client side implementation of a file transfer server.

The server is built utilizing windows sockets, and was developed in VS2012 (Update 5). Please utilize this IDE for best results with this code.
The server listens for incoming connections and saves the data that comes in until the transmission is complete, then scrape the 1st line of incoming
data to get the filename, and save the rest of the input to file.

The client will begin the handshake protocol with the server, and upon successful connection display the user a menu of options with which to send a specific file.
The client will then break the file up into 512 byte parts and send them until the file is completely sent, while putting the filename first on it's own line of input.
