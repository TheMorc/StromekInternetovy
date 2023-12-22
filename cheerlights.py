#!/usr/bin/env python

#
# zapinani cheerlights
# v sudo nano /etc/rc.local 
#

from __future__ import division
import time
from urllib.request import urlopen
import serial
import colorsys
from datetime import datetime

arduino = serial.Serial("/dev/ttyACM0", 115200)
print("Arduino reset delay")
time.sleep(3)

def SetLedBorg(hue):
    #print("sending hue: " + str(int(hue*255)))
    if hue == -1:
        arduino.write("W_".encode())
    elif hue == -2:
        arduino.write("R_".encode())
    else:
        arduino.write(str(int(hue)).encode())

cheerlightsUrl = 'http://api.thingspeak.com/channels/1417/field/1/last.txt'

colourMap = {'red':         0,
             'green':       95,
             'blue':        170,
             'cyan':        120,
             'white':       -1,
             'warmwhite':   -2,
             'purple':      185,
             'magenta':     200,
             'yellow':      65,
             'orange':      35,
             'pink':        235,
             'oldlace':     -2}

lastColor = ""
lastHour = 2137
currentHour = -2137

while True:
    #print("lastColor:" + str(lastColor))
    now = datetime.now()
    currentHour = int(now.strftime("%H"))
    if currentHour != lastHour:
        print("Hour change")
        if currentHour == 6:
            print("full brightness")
            arduino.write("BR1".encode())
        elif currentHour >= 19 and currentHour <= 21:
            print("half brightness")
            arduino.write("BR2".encode())
        elif currentHour == 22 or currentHour == 23 or currentHour == 0:
            print("low brightness")
            arduino.write("BR1".encode())
        elif currentHour == 1:
            print("no brightness")
            arduino.write("BR0".encode())
    if (arduino.inWaiting() > 0):
        data_str = arduino.read(arduino.inWaiting()).decode('utf-8')
        print(data_str)
    try:
        if currentHour <= 23 and currentHour >= 6 or currentHour == 0:
            with urlopen(cheerlightsUrl) as colourResponse:
                colourName = colourResponse.read().decode("utf-8")
                if colourName in colourMap:                   # If we recognise this colour name then ...
                    hue = colourMap[colourName]
                if lastColor != colourName:
                    SetLedBorg(hue)
                    print("new web color: " + colourName)
                lastColor = colourName
    except Exception as e:                                             # If we have an error
        print("ZESER:")
        print(e)
        pass                                                # Ignore it (do nothing)
    finally:                                            # Regardless of errors:
        lastHour = currentHour
        time.sleep(10)

