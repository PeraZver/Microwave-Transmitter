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


f_REF = 19.2  # Reference frequency
params = {'R':1, 'N':200, 'F':0.0, 'M':100, 'D':5}  # frequency settings parameters
f_PDF = f_REF/params['R'] 


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
        timeout=0.85)
      
def check_frequency (freq_dict):
# Function that calculates frequency based on PLL parameters 
# First test if we're somewhere where it's not allowed to be in   
    global f_PDF 
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
# Now check if N is somewhere where we won't be able to control VCOs.
    N_min = int(round(3000/f_PDF))
    N_max = int(round(6000/f_PDF))
    if ((freq_dict['N'] < N_min) | (freq_dict['N'] > N_max)):
        print 'N must be between '+ str(N_min) + ' and '+ str(N_max)
        return 0
    
    f_VCO = (freq_dict['N'] + freq_dict['F']*1.0/freq_dict['M'])*f_PDF
    f_RFOUT = f_VCO / (2**freq_dict['D'])
    print 'f_PDF is ' + str(round(f_PDF,4)) + ' MHz'
    print 'f_VCO is ' + str(f_VCO) + ' MHz'
    print 'f_RFOUT is ' + str(f_RFOUT) + ' MHz'
    return 1
    

    
def select_D(freq):
#    Selects D parameter based on desired frequency output
    if   3000 < freq <= 6000:
        return 0
    elif 1500 < freq <= 3000:
        return 1
    elif 750  < freq <= 1500:
        return 2    
    elif 375  < freq <= 750:
        return 3    
    elif 187.5 < freq <= 375:
        return 4
    elif 93.75 < freq <= 187.5:
        return 5
    elif 46.875 < freq <= 93.75:
        return 6        
    elif 23.5 < freq <=  46.875:
        return 7
        
def set_frequency_INT(freq):
    # sets  integer mode paramteres for given frequency
    D = select_D(freq) # we need parameter D first
    f_VCO = freq * 2**D  # this is what VCO will need to generate    
    N = int(f_VCO/f_PDF)
    return N, D

def set_frequency_FRAC(freq):
    # sets  integer mode paramteres for given frequency
    D = select_D(freq) # we need parameter D first
    f_VCO = freq * 2**D  # this is what VCO will need to generate    
    N = int(f_VCO/f_PDF)
    F = int((f_VCO/f_PDF - N)*params['M'])
    return N, D, F


def set_param_sweep(args):
#    returns command that starts given parameter sweep
    param = args[1]
    start = args[2]
    stop  = args[3]
    if (len(args) == 5):
        td = args[4]
    else:
        td = '10'    
    return ('s'+ param + start + 'a'+ stop +'a' + td +'a')
    
def freq_sweep(f_start, f_stop):
#    set frequency sweep from f_start to f_stop
    N_start, D_start = set_frequency_INT(f_start)
    N_stop, D_stop = set_frequency_INT(f_stop)
    return (set_param_sweep(('sweep','N', str(N_start), str(N_stop))))

        
def read_registers(ser):
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
#    Update paramters that set frequency            
    check_frequency(params)

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

    
def command_parser(data, ser):
# function that converts user input to Teensy commands
    user_in = data.split(' ');

    if user_in[0] in ['N', 'F', 'M', 'R']:
        params[user_in[0]] = int(user_in[1])
        check_frequency(params)  # update frequecy settings
        return (user_in[0]+user_in[1]+'a')
            
    elif (user_in[0] == 'D'):
        params['D'] = int(user_in[1])
        check_frequency(params)
        return ('D'+user_in[1])
        
    elif (user_in[0] in ("sweep", "SWEEP")):
        return set_param_sweep(user_in)
        
    elif (user_in[0] == 'g'):
        read_registers(ser)
        return ('')
    
    elif (user_in[0] == 'FINT'):  # Set frequency in integer mode
        N, D = set_frequency_INT(int(user_in[1]))
        ser.write('D'+str(D))
        ser.write('N'+str(N)+'a')
        ser.write('F0a')  # Kill fractional mode
        for i in range(1,6):
            line = ser.readline()
            if  line.strip():  # evaluates to true when an "empty" line is received
                print line.strip()
        return ('')
        
    elif (user_in[0] == 'FFRAC'):  # Set frequency in fractional mode
        N, D, F = set_frequency_FRAC(float(user_in[1]))
        ser.write('D'+str(D))
        ser.write('N'+str(N)+'a')
        ser.write('F'+str(F)+'a')        
        for i in range(1,6):
            line = ser.readline()
            if  (line.strip() != ''):
                print line.strip()
        return ('') 
        
    elif (user_in[0] == 'help'):
        read_help(user_in)
        return('')
        
    else:
        return data
        
        
        
    
    
def main():
    print "Microwavino Interface"
    print "v1.1"
    print "type 'help [command]' if not sure how to proceed\n"
    print "Pero, September 2017"
    
    try:
        ser = configure_serial('COM8')
        if not ser.isOpen():
            raise Exception
        else:
            print "\nSerial port opened"
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
                    ser.write(command_parser(var, ser))

            else:
                print line.strip()
            
        except KeyboardInterrupt:
            ser.close()
            sys.exit(1)
            break

    
def read_help(*args):
# displays help for each command
    if (len(args[0]) > 1):
        command = args[0][1]
    else:
        command = args[0][0]

    if (command == 'D'):
        print """ 
        Output frequency divider (DIVA):
        0 = Divide by 1, if 3000MHz ≤ fRFOUTA ≤ 6000MHz
        1 = Divide by 2, if 1500MHz ≤ fRFOUTA< 3000MHz
        2 = Divide by 4, if 750MHz ≤ fRFOUTA < 1500MHz
        3 = Divide by 8, if 375MHz ≤ fRFOUTA < 750MHz
        4 = Divide by 16, if 187.5MHz ≤ fRFOUTA < 375MHz
        5 = Divide by 32, if 93.75MHz ≤ fRFOUTA < 187.5MHz
        6 = Divide by 64, if 46.875MHz ≤ fRFOUTA < 93.75MHz
        7 = Divide by 128, if 23.5MHz ≤ fRFOUTA< 46.875MHz"""
        
    elif (command == 'sweep'):
        print """ 
        function SWEEP X a b [dt]
        sweeps parameter X from a to b with resolution of dt milliseconds. 
        Default value for dt is 10 ms. """
    
    elif (command == 'g'):
        print """ 
        command g displays current content of registers 0 - 5 of MAX2871."""
        
    elif (command == 'r'):
        print """
        Readout of MAX2871 register 6. Internal ADC of MAX2871 can be configured
        to measure temperature or voltage at the VCO:
        rt - display temperature value.
        rv - display VCO voltage. """
    
    else:
        print """ 
        COMMAND LIST:
        -----------------------------------------------
        g - read MAX2871 register content
        rt - read MAX2871 ADC set to temperature readout
        rv - read MAX2871 ADC set to VCO voltage readout
        e - activate RF output
        d - deactivate RF output
        px - sets the power of RF output, where {x} can be 1,2,3 or 4.
        fx - selects the filter, where {x} can be 1,2,3 or 4.
        
        To change frequency settings, use following commands:
        
        N x - set integer divider where x is between 16 and 65535.
        M x - set fractional modulus where x is between 2 and 4095.
        F x - set fractional divider where x is between 1 and M.
        R x - set reference divider where x is between 1 and 1023.
        D x - set output divider where x is between 1 and 7.
        
        To change frequency directly, use two functions:
        
        FINT x - set frequency x in integer mode
        FFRAC x - set frequency x in fractional mode
        
        To sweep parameters, try this:
        
        sweep x a b dt - sweep parameters from a to b."""        
        
if __name__ == "__main__":   
    main()