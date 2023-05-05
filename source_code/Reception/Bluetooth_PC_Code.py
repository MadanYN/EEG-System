# This code is for receiving data from microcontroller over bluetooth.

import serial
import time
import serial.tools.list_ports
import csv
import matplotlib.pyplot as plt

#nitial Connection
cp=serial.tools.list_ports.comports()
for p in cp: # this loop finds the port number for HC05
    if "BTHENUM" in p.hwid:
        start_of_address=p.hwid.rfind("&")
        end_of_address=p.hwid.rfind("_")
        address=p.hwid[start_of_address+1:end_of_address]
        if int(address,16)!=0:
            port_name=str(p.name)
            break

ser = serial.Serial(port_name, 9600, timeout = 3) #making serial connection with HC05 port
ser.reset_input_buffer()
time.sleep(3) #connection process resets the microcontroller so we wait for 3sec to avoid any random behaviour


# setting up csv file to store the data received
header=[]
header.append("Time(s)") 
for i in range(channels):
    header.append("Channel "+ str(i+1)) #writing column names

new_file = open('EEG_Data.csv','w',newline="")
write=csv.writer(new_file)
write.writerow(map(lambda x: x, header))
new_file = open('EEG_Data.csv','a',newline="")


#receiving data
gain = (100.0)/(2**15) #gain related related to ADC resolution
chunks=[] # to temporarily store 24 channel data
ser.flushInput()
t0=time.time() #initialise time
while(True):
    received = str(ser.readline().decode('ascii')) # received string
    if received == "b\r\n": # indicative of begin
        for i in range(channels): # reading data for channels
            chunks.append(ser.readline().decode('ascii')) #appending data to chunks
        chunks = [int(i) for i in chunks] #converting to int
        chunks = [float('%.3f'%(gain*i)) for i in chunks] # including gain factor
        chunks.insert(0, float('%.3f'%(time.time()-t0))) # noting the time of arrival
        # print("data received at t = "+ str(chunks[0])) # optional print 
        write.writerow(map(lambda x: x, chunks))#write to csv channel wise
        chunks=[]
    time.sleep(0.01)

