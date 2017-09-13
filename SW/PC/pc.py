# -*- coding: utf-8 -*-
#! python
"""
PC Interface with my Teensy/MAX2871 based 
microwave transmitter via serial port. 


Created on Wed Sep 06 22:33:43 2017

@author: Pero_2912
"""
import serial
import sys
import bitstring as bit
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
        timeout=0.1
    )


def read_registers():
# Reads and parses registers from MAX2871
    ser.write('g')
    while(True):
        line = ser.readline().strip()
        if line == 'MAX2871 Registers printout:':
            break
    print line
    for i in range(0,6):
        line = ser.readline()
        print 'Register ' + str(i)
        parse_register(line.strip(), i)
        print ' '

def parse_register(line, i):
#parses each register and displays values in human form  
        # if register has first bits equal to 0, Teensy will 
        # send data without them. That's why I fill like this
        while (len(line) < 8): 
            line = '0'+line
        print line
        reg = bit.BitArray(hex=line)
        #examine registers
        if (i == 0):
            print 'Integer mode: ' + str(reg[0])
            print 'Integer divider N: ' + str(reg[1:17].uint)
            print 'Fraction divider F: ' + str(reg[17:29].uint)
            
        elif (i == 1):
            print 'Fraction Modulus M: ' + str(reg[17:29].uint)
            
        elif (i == 2):
            print 'Reference divider R: ' + str(reg[8:18].uint)
            print 'Reference DBR: ' + str(reg[6])
            print 'Reference DIV2: ' + str(reg[7])
            
        elif (i == 4):
            print 'Output divider DIVA: ' + str(reg[9:12].uint)
            print 'Output power RFOUTA: ' + str(3*reg[27:29].uint - 4) + ' dBm'
            print 'Output power RFOUTA: ' + str(3*reg[24:26].uint - 4) + ' dBm'
            print 'RFOUTA enabled: ' + str(reg[26])
            print 'RFOUTB enabled: ' + str(reg[23])
            
       
    
def command_parser(data):
# function that converts user input to Teensy commands
    user_in = data.split(' ');
    if user_in[0] in ['N', 'F', 'M', 'R']:
        return (user_in[0]+user_in[1]+'a')
    elif (user_in[0] == 'D'):
        return ('D'+user_in[1])
    elif (user_in[0] == "sweep"):
        return ('s'+user_in[1] + user_in[2] + 'a'+ user_in[3]+'a' + user_in[4]+'a')
    elif (user_in[0] == 'g'):
        read_registers()
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
            sys.exit(1)
            break
