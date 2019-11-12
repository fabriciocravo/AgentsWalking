import serial
import time
import datetime
import sys
# A20B30C50D234E28F10G
handshake = 0;
COMS = ["COM0", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM6", "COM7", "COM8", "COM9",
        "COM10", "COM11", "COM12", "COM13", "COM14"]

#Here we text all the possible USB ports looking for the Arduino
#The arduino constantly spams Arduino when it's been turned on
#So we look for the port spaming the string Arduino
#If found we keep going, if not exit the program
#This part mlight be problematic on Mac and Linux, since i have no idea how they assign logical ports on those OS
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

#Now let us get the time using datetime
TheTime = datetime.datetime.now()
hour = str(TheTime.hour)
minute = str(TheTime.minute)
second = str(TheTime.second)
millis = str(round(TheTime.microsecond/1000))
day = str(TheTime.day)
month = str(TheTime.month)

#Here we prepare to send the data to the Arduino
#Each section of information is envoloped in a set of letters
#The Arduino is programmed to read the hour between A and B, the minutes between B and C and so on!
#Reads the day and the month to name the experiment file
dataSent = "A" + hour + "B" + minute + "C" + second + "D" + millis + "E" + day + "F" + month + "G"
print(dataSent)
dataSent = dataSent.encode()
print(dataSent)
link.write(dataSent)
time.sleep(10)

#Here we ask the arduino for it's millis value for the first time
#This will be done to set the linear constant that helps the arduino keep up with the real time
#We also register the time the answer has been sent
dataSent = "T"
dataSent = dataSent.encode()
link.write(dataSent)
FirstTimeArduino = link.readline()
FirstTime = time.time()*1000
FirstTimeArduino = FirstTimeArduino.decode("ascii")
print("FA: " + FirstTimeArduino)
FirstTimeArduino = float(FirstTimeArduino)

#W8 until the arduino counts 
time.sleep(270)

#After the arduino has counted for five minutes we ask for it's time again 
#We register when the answer was send 
#Them we calculate the difference, in an ideal world the arduino would count exactly 3 minutes 
#And it would have no difference between the time.time and the arduinos time resulting in a ratio of one
#But since the clocks count differently this ratio is how faster the real clock is counting in respect to the arduino clock
#We calculate this ratio and send it back to the arduino so it can use it to correct it's clock ratio
dataSent = "T"
dataSent= dataSent.encode()
link.write(dataSent)
SecondTimeArduino = link.readline()
SecondTime = time.time()*1000
SecondTimeArduino = SecondTimeArduino.decode("ascii")
print("SA: " + SecondTimeArduino)
SecondTimeArduino = float(SecondTimeArduino)

#Here we calculate and send the different ratios
A = (SecondTime - FirstTime)/(SecondTimeArduino - FirstTimeArduino)
A = round(A, 7)
A = str(A)
print("The A value: " + str(A))

A = A.encode()
link.write(A)

#Now it's all done! 
time.sleep(5)
print("Initial Procedure Done - You can remove the arduino now")

