Distributed Tennis
Jonathan Lepage
November 2010


To compile:
make

Launch the server:
./server <nb of clients>

Launch the clients:
./client <IP of server>


Default number of clients is 3, default IP of server is 127.0.0.1.
Depending on the library of the machine it's compiled on, you might need to uncomment some of the #include lines in file tennis.h.
As the client use some ANSI escape characters to clear the screen and draw the colours, the printf calls might be a bit hard to understand.
