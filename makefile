all:server client ss-server remote connect
server:server.cpp
	g++ -std=c++17 server.cpp -o server
client:client.cpp
	g++ -std=c++17 client.cpp -o client
ss-server:ss-server.cpp
	g++ -std=c++17 -g ss-server.cpp -o ss-server -l event -l pthread
remote:remote.cpp
	g++ -std=c++17 -g remote.cpp -o remote -l event -l pthread
connect:connect.cpp
	g++ -std=c++17 -g connect.cpp -o connect -l event -l pthread

