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
#import time


f_REF = 19.6  # Reference frequency
params = {'R':1, 'N':120, 'F':0.0, 'M':40, 'D':5}  # frequency settings parameters



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

def frequency (freq_dict):
# First test if we're somewhere where it's not allowed to be in    
    if (freq_dict['R'] < 1) | (freq_dict['R'] > 1023):
        print 'R must be between 1 and 1023'
        return 0    
    if (freq_dict['N'] < 16) | (freq_dict['N'] > 65535):
        print 'N must be between 16 and 65535'
        return 0
    if (freq_dict['M'] < 2) | (freq_dict['M'] > 4095):
        print 'M must be between 2 and 4095'
        return 0
    if (freq_dict['F'] > freq_dict['M']):
        print 'F must be lower than M'
        return 0
# Calculate frequency from the given params        
    f_PDF = f_REF/freq_dict['R']
# Now check if N is somewhere where we won't be able to control VCOs. It must 
# be high enough that (f_VCO/N - f_PFD) passes through loop filter. It also must be
# high enough that oscillator frequency is within 3-6 GHz.
    N_min = int(round(max(3000/abs(f_PDF - 0.776), 3000/f_PDF)))
    N_max = int(round(max(6000/abs(f_PDF - 0.776), 6000/f_PDF)))
    if ((freq_dict['N'] < N_min) | (freq_dict['N'] > N_max)):
        print 'N must be between '+ str(N_min) + ' and '+ str(N_max)
        return 0
    
    f_VCO = (freq_dict['N'] + freq_dict['F']*1.0/freq_dict['M'])*f_PDF
    f_RFOUT = f_VCO / (2**freq_dict['D'])
    print 'f_PDF is ' + str(round(f_PDF,4)) + ' MHz'
    print 'f_VCO is ' + str(f_VCO) + ' MHz'
    print 'f_RFOUT is ' + str(f_RFOUT) + ' MHz'
    return 1
    
        
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
                
    frequency(params)

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
            params['N'] = reg[1:17].uint
            params['F'] = reg[17:29].uint
            
        elif (i == 1):
            print 'Fraction Modulus M: ' + str(reg[17:29].uint)
            params['M'] = reg[17:29].uint
            
        elif (i == 2):
            print 'Reference divider R: ' + str(reg[8:18].uint)
            print 'Reference DBR: ' + str(reg[6])
            print 'Reference DIV2: ' + str(reg[7])
            params['R'] = reg[8:18].uint
            
        elif (i == 4):
            print 'Output divider DIVA: ' + str(reg[9:12].uint)
            print 'Output power RFOUTA: ' + str(3*reg[27:29].uint - 4) + ' dBm'
            print 'Output power RFOUTA: ' + str(3*reg[24:26].uint - 4) + ' dBm'
            print 'RFOUTA enabled: ' + str(reg[26])
            print 'RFOUTB enabled: ' + str(reg[23])
            params['D'] = reg[9:12].uint

    
def command_parser(data):
# function that converts user input to Teensy commands
    user_in = data.split(' ');

    if user_in[0] in ['N', 'F', 'M', 'R']:
        params[user_in[0]] = int(user_in[1])
        frequency(params)  # update frequecy settings
        return (user_in[0]+user_in[1]+'a')
            
    elif (user_in[0] == 'D'):
        params['D'] = int(user_in[1])
        frequency(params)
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
