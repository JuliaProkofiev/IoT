# Exercício 3: Conectar Inclinômetro e CAN 1 para enviar dados, CAN0 receber e filtrar os dados da CAN1 para comandar LEDS 1 e 2, e valor alto no Inclinômetro comandar LED 3. 
""" Ler mensagem da CAN, e definir modos de operação de LED:
Caso byte 1 == 0x01 a 0x7F: Desligado
Caso byte 1 == 0x80 a 0xFF: LED 1 aceso
Caso byte 2 == 0x01 a 0x7F: Desligado
Caso byte 2 == 0x80 a 0xFF: LED 2 aceso
Caso byte 1+2 (inclinômetro) > 0x7F: Manter LED 3 aceso, else: desligado 
Caso byte 3+4 (inclinômetro) > 0x7F: Manter LED 3 aceso, else: desligado

Inclinômetro 
PGN: 61.459 

Dados

Pitch: Byte 1-2
Função:
sum = hex(int(message.data[0], 16) + int(message.data[1], 16)) 
sum = (int(sum[2:],2)-64)/500
Roll: Byte 3-4
Função:
sum = hex(int(message.data[2], 16) + int(message.data[3], 16)) 
sum = (int(sum[2:],2)-64)/500

Offset: -64
Mult.: 500
Explicação:
1/0,002 = 500 (resolução de 0,002 deg por 1 bit)
64,51/0,002 = 32.255 bits
32.255/500 = 64,51 (Teste da Verdade)

min.: -64 deg || máx.: 64,51 deg
64/0,002 = 32.000
64,51/0,002 = 32.255
min.: -32.000 bits || máx.: 32.255 bits """

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
            print('Nothing!!')
        else:
            c = '{0:f} {1:x} {2:x} '.format(message.timestamp, message.arbitration_id, message.dlc)
            s=''
            for i in range(message.dlc):
                s +=  '{0:x} '.format(message.data[i])
            print(' {}'.format(c+s))
except KeyboardInterrupt:
    #Catch keyboard interrupt
    print('\n\rKeyboard interrtupt')
