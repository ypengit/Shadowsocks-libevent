all:server client ss-server
server:server.cpp
	g++ -std=c++17 server.cpp -o server
client:client.cpp
	g++ -std=c++17 client.cpp -o client
ss-server:ss-server.cpp
	g++ -std=c++17 -g ss-server.cpp -o ss-server -l event -l pthread
