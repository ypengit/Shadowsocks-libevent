import socket

s = socket.socket()
s.connect(("127.0.0.1", 7000))
s.send("\x05\x03\x00\x08\x19", 0);
a = s.recv(2)
for i in a:
    print("%x"%ord(i))

#s.send("\x00", 0)
s.send("\x05\x00\x00\x01" + "\x7f\x00\x00\x01" + "\x1f\x40", 0);
print "send 2 succed"
a = s.recv(4)
for i in a:
    print("%x"%ord(i))
s.close()



