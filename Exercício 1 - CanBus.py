# Exercício 1: Comunicar o Servidor CAN, receber a CAN do Inclinômetro LOHR e printar mensagem.
# Terminal Usage 

#!/usr/bin/python3.7

import can
import time
import os

try:
    bus = can.interface.Bus(channel='can1', bustype='socketcan')
except OSError:
    print('Cannot find PiCAN board.')
    exit()
    
print('Ready')

try:
    while True:
        message = bus.recv(0.5) # Wait until a message is received.
        if message is None:
            print('Não Recebeu Mensagem!!')
        else:
            c = '{0:f} {1:x} {2:x} '.format(message.timestamp, message.arbitration_id, message.dlc)
            s=''
            for i in range(message.dlc):
                s +=  '{0:x} '.format(message.data[i])
            print(' {}'.format(c+s))
except KeyboardInterrupt:
    #Catch keyboard interrupt
    print('\n\rKeyboard interrtupt')
