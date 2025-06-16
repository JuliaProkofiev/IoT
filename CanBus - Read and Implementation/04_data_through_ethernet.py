# Exercício 4: Enviar dados Ethernet – Raspberry to PC:
# Terminal Usage 
# Python Server
from socket import *

bufferSize = 1024
msgFromServer = "Howdy Client, happy to be your server"
ServerPort = 2222
ServerIP = '192.168.49.89'
bytesToSend = msgFromServer.encode('utf-8')
RPIsocket = socket(AF_INET, SOCK_DGRAM)
RPIsocket.bind((ServerIP,ServerPort))
print('Server is Up and Listening...')
cnt = 0

try:
    while True:
        message,address = RPIsocket.recvfrom(bufferSize)
        message = message.decode('utf-8')
        print(message)
        print('Client Adress: ',address[0])
        if message=='INC':
            cnt=cnt+1
        if message=='DEC':
            cnt=cnt-1
        msg=str(cnt)
        print(msg)
        msg=msg.encode('utf-8')
        RPIsocket.sendto(msg,address)
except KeyboardInterrupt: 
    socket.close()
    print('\n\rKeyboard interrtupt')

# Python Client
from socket import *

msgFromClient='Howdy Server, from your Client'
bytesToSend = msgFromClient.encode('utf-8')
serverAddress = ('192.168.49.89', 2222)
bufferSize=1024
UDPClient = socket(AF_INET, SOCK_DGRAM)

try:
    while True:
        cmd = input('What do you want to do with counter, INC or DEC?')
        cmd = cmd.encode('utf-8')
        UDPClient.sendto(cmd, serverAddress)
        data, address = UDPClient.recvfrom(bufferSize)
        data = data.decode('utf-8')
        print('Data from Server: ', data)
        print('Server IP Address: ', address[0])
        print('Server Port: ', address[1])
except KeyboardInterrupt: 
    socket.close()
    print('\n\rKeyboard interrtupt')
