import serial
import time

PORT = 'COM13'
BAUD = '9600'
PARITY = serial.PARITY_NONE

s = []
i = 0

ser = serial.Serial(PORT, BAUD, timeout=1)
while(1):
    if ser.is_open:
        s = ser.readline()
        print s
        i += 1

    else:
        print 'Serial Port ' + PORT + ' not open'
