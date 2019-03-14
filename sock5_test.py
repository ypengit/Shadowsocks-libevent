import socket

s = socket.socket()

s.bind(("127.0.0.1", 1080))

s.listen(5)

accept_fd, info = s.accept()

res = accept_fd.recv(2048)

print "res:%x" % res
