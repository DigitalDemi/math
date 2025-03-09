# socket 
# bind
# listen 
# accept
# read
# write
# close

#from my dirstubted systems class, I want to create rpc and use the function here

import socket
import signal
import sys
from Prime.prime import Prime



HOST = ''
PORT  = 50000


def sigint_handler(signum, frame):
    print('\nCtrl+C pressed, shutting down server...')
    sys.exit(0)

signal.signal(signal.SIGINT, sigint_handler)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
    server.bind((HOST,PORT))
    server.listen(1)
    while True:
        connection , address = server.accept()
        with connection:
            print(f"The the client is {address}, {PORT}")
            while True:
                data = connection.recv(1024)
                number = data.decode().strip()
                prime = Prime(int(number))
                connection.sendall(str(prime).encode('utf-8'))

            
