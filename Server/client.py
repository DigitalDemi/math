# socket 
# connect 
# read write -> server, client
# close -> finish

import socket 

HOST = socket.gethostbyname(socket.gethostname())
PORT = 50000

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect((HOST,PORT))
    client.sendall(b"Check Prime")
    data = client.recv(1024)
    print('Received', repr(data))