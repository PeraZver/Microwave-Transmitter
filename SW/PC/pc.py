# -*- coding: utf-8 -*-
"""
PC Interface with my Teensy/MAX2871 based microwave transmitter 

via serial port. 


Created on Wed Sep 06 22:33:43 2017

@author: Pero_2912
"""
import serial
import sys
import argparse
import ast
import time

BAUDRATE = 9600
PARITY = False
#Configures serial port
def configure_serial(serial_port):
    return serial.Serial(
        port=serial_port,
        baudrate=BAUDRATE,
        parity=serial.PARITY_EVEN if PARITY else serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.EIGHTBITS,
        timeout=0.05
    )
    
if __name__ == "__main__":   
    print "Microwave Transmitter Interface"
    print "v1.0"
    print "Pero, September 2017"
    
    try:
        ser = configure_serial('COM8')
        if not ser.isOpen():
            raise Exception
        else:
            print "Serial port opened"
            print "Initializing Microwave transmitter...\n"
            ser.write(' ')
    except:
        print 'Opening serial port failed'
        exit(1)    
        
    while True:
        try:
            line = ser.readline()
            if not line.strip():  # evaluates to true when an "empty" line is received
                var = raw_input(">> ")
                if var:
                    ser.write(var)
            else:
                print line,
            
        except KeyboardInterrupt:
            ser.close()
            break
