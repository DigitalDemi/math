# socket 
# connect 
# read write -> server, client
# close -> finish

import socket 

HOST = socket.gethostbyname(socket.gethostname())
PORT = 50000

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect((HOST,PORT))
    while True:
        user_input = input("Check if number is prime")
        if user_input.lower() == 'q': break
        client.sendall(user_input.encode()) 
        data = client.recv(1024)
        print('Received', repr(data))