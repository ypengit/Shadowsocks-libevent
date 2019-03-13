import socket

s = socket.socket()
s.bind(("127.0.0.1", 7000))
s.listen(5)
s.accept()
s.setblocking(False)
while True:
    a = "                                                      "
    a = s.recv(1)
    if(len(a) == 0):
        continue
    else:
        print a
