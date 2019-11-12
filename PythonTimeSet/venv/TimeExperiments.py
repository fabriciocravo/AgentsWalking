import serial
import time
import datetime
import sys
# A20B30C50D234E28F10G
handshake = 0;
COMS = ["COM0", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM6", "COM7", "COM8", "COM9",
        "COM10", "COM11", "COM12", "COM13", "COM14"]

for com in COMS:

    try:
       print("Trying " + com)
       link = serial.Serial(port = com, baudrate=115200, timeout = 10)
       time.sleep(1)

       data = str(link.readline())

       if("Arduino" in data):
            link.write(77)
            print("Found it")

            time.sleep(1)
            data = str(link.readline())
            print(data)
            if( "Ok" in data ):
                print(link.name)
                flag = 1
                handshake = 1
                break

    except:
        pass

if(handshake):
    print("Nice! Link established, starting the experiment ")
else:
    print("Error connecting to arduino")
    sys.exit();


TheTime = datetime.datetime.now()
hour = str(TheTime.hour)
minute = str(TheTime.minute)
second = str(TheTime.second)
millis = str(round(TheTime.microsecond/1000))
day = str(TheTime.day)
month = str(TheTime.month)

dataSent = "A" + hour + "B" + minute + "C" + second + "D" + millis + "E" + day + "F" + month + "G"
print(dataSent)
dataSent = dataSent.encode()
print(dataSent)
link.write(dataSent)
time.sleep(10)

dataSent = "T"
dataSent = dataSent.encode()
link.write(dataSent)
FirstTimeArduino = link.readline()
FirstTime = time.time()*1000
FirstTimeArduino = FirstTimeArduino.decode("ascii")
print("FA: " + FirstTimeArduino)
FirstTimeArduino = float(FirstTimeArduino)

time.sleep(270)

dataSent = "T"
dataSent= dataSent.encode()
link.write(dataSent)
SecondTimeArduino = link.readline()
SecondTime = time.time()*1000
SecondTimeArduino = SecondTimeArduino.decode("ascii")
print("SA: " + SecondTimeArduino)
SecondTimeArduino = float(SecondTimeArduino)

A = (SecondTime - FirstTime)/(SecondTimeArduino - FirstTimeArduino)
A = round(A, 7)
A = str(A)
print("The A value: " + str(A))

A = A.encode()
link.write(A)
time.sleep(5)

while(True):
    data = link.readline()
    print("Arduino time is: " + str(data))
    TheTime = datetime.datetime.now()
    TheTime = TheTime.hour*60*60*1000 + TheTime.minute*60*1000 +TheTime.second*1000 + round(TheTime.microsecond/1000)
    print("Computer time is: " + str(TheTime))
    print("######################################")

