all:server client ss-server server remote connect main event_buffer
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
main:main.cpp
	g++ -std=c++17 -g main.cpp -o main -l event -l pthread
event_buffer:event_buffer.cpp
	g++ -std=c++17 -g event_buffer.cpp -o event_buffer -l event -l pthread

