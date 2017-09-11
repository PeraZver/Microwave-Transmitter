# -*- coding: utf-8 -*-
#! python
"""
PC Interface with my Teensy/MAX2871 based microwave transmitter 

via serial port. 


Created on Wed Sep 06 22:33:43 2017

@author: Pero_2912
"""
import serial
import sys
#import argparse
#import ast
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
        timeout=0.5
    )


def set_sweep(data):
# sets sweep
    for i in range(int(data[2]), int(data[3])):    
        ser.write(data[1]+str(i)+'a')
        line = ser.readline()
        while (line.strip()):
            print line.strip()
            line = ser.readline()            
        time.sleep(1)
        
    
def command_parser(data):
# function that converts user input to Teensy commands
    user_in = data.split(' ');
    if user_in[0] in ['N', 'F', 'M', 'R']:
        return (user_in[0]+user_in[1]+'a')
    elif (user_in[0] == "sweep"):
        set_sweep(user_in)
        return (' ')
    else:
        return data
        
        
        
    
if __name__ == "__main__":   
    print "Microwave Transmitter Frequency Sweep"
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
                    ser.write(command_parser(var))
            else:
                print line,
            
        except KeyboardInterrupt:
            ser.close()
            sys.exit(0)
            break
