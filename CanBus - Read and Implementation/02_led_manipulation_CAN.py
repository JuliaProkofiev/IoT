# Exercício 2: Comunicar o Servidor CAN XXX.XXX.XX.XX, receber por CAN o status do LED.
# Terminal Usage 

#!/usr/bin/python3

import RPi.GPIO as GPIO
import can
import time
import os
from threading import Thread

lc0   = 18
lc1   = 3
lss   = 23

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(lc0,GPIO.OUT)
GPIO.setup(lc1,GPIO.OUT)
GPIO.setup(lss,GPIO.OUT)

try:
    bus0 = can.interface.Bus(channel='can0', bustype='socketcan')
    bus1 = can.interface.Bus(channel='can1', bustype='socketcan')
    GPIO.output(lc0, False)
    GPIO.output(lc1, False)
    GPIO.output(lss, True)
except OSError:
    print('Cannot find PiCAN board.')
    GPIO.output(lc0, False)
    GPIO.output(lc1, False)
    GPIO.output(lss, False)
    exit()

def can_rx_task():
    tp = 0
    while True: 
        message = bus1.recv(0.001)  # Wait until a message is received.
        if message is None:
            tp = tp +1
        else:
            print('received message  (', tp, ')')
            tp = 0
            c = '{0:f} {1:x} {2:x} '.format(message.timestamp, message.arbitration_id, message.dlc)
            s=''
            for i in range(message.dlc):
                if(message.data[i]=='ff'):
                    GPIO.output(lc1,True)
                else:
                    GPIO.output(lc1,False)
                    s +=  '{0:x} '.format(message.data[i])
            print(' {}'.format(c+s))
        #time.sleep(0.001)
                    
t = Thread(target = can_rx_task)
t.start()

# Main loop
try:
    while True:
        GPIO.output(lc0,True)
        bus0.flush_tx_buffer() #will send the message
        msg = can.Message(arbitration_id=0xC0FFE,data=[0xff,0x01,0xff,0xff,0xff,0x00,0x00,0xff]) #makes the fake message
        bus0.send(msg,1) #sended
        time.sleep(0.1)
        GPIO.output(lc0,False)
        time.sleep(0.1)
        
except KeyboardInterrupt: 
    #Catch keyboard interrupt
    GPIO.output(lc0,False)
    GPIO.output(lc1,False)
    GPIO.output(lss,False)
    #os.system("sudo /sbin/ip link set can0 down")
    #os.system("sudo /sbin/ip link set can1 down")
    print('\n\rKeyboard interrtupt')    
