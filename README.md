# TcpChatroom
Hello Doc
first server side : chmod +x server.c
gcc -pthread -o server server.c
./server 8888 (example port 8888)
to turn off the server type (_shdn)
if the server is closed the chatRoom leave all grp clients member



second client side :chmod +x client.c 
second :gcc -pthread -o client client.c
first client : ./client jeff
_connect jeff 127.0.0.1 8888
second client : ./client ray
_connect ray 127.0.0.1 8888
to leave clientside type (_quit)
to see member in the same grp type (_who)
and than when you live simply type exit
