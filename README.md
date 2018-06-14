# WindowsSocketsFileServer
Simple client/server c++ program to trasnfer files from one to the other using Windows sockets (like UNIX sockets)

This program was built in VS2012, Update 5. Please test and play with this code in that IDE for optimal results.

The client will pick a file to send to the server, and send the file in ~512 byte sizes until the whole file has been sent.

The filename will be sent on the 1st line so that the filename is easily retreieved from the incoming data on the server side.

The server will parrot back in the console what was sent to it, and save it as the proper file name.
