#!/usr/bin/env python

#
# zapinani cheerlights
# v sudo nano /etc/rc.local 
#



# Import the library functions we need
from __future__ import division
import time
from urllib.request import urlopen
import serial
import colorsys
from datetime import datetime

arduino = serial.Serial("/dev/ttyACM0", 115200)
arduino.read()

# A function to set the LedBorg colours
def SetLedBorg(red, green, blue):
    hue, sat, val = colorsys.rgb_to_hsv(red,green,blue)
    #print("sending hue: " + str(int(hue*255)))
    if red and green and blue == 1:
        arduino.write("W_".encode())
    else:
        arduino.write(str(int(hue*255)).encode())

# Setup parameters
cheerlightsUrl = 'http://api.thingspeak.com/channels/1417/field/1/last.txt'
#            Name           Red     Green   Blue
colourMap = {'red':         (1.0,   0.0,    0.0),
             'green':       (0.0,   1.0,    0.0),
             'blue':        (0.0,   0.0,    1.0),
             'cyan':        (0.0,   1.0,    1.0),
             'white':       (1.0,   1.0,    1.0),
             'warmwhite':   (1.0,   1.0,    0.9),
             'purple':      (0.4,   0.0,    0.5),
             'magenta':     (1.0,   0.0,    1.0),
             'yellow':      (1.0,   1.0,    0.0),
             'orange':      (1.0,   0.65,   0.0),
             'pink':        (1.0,   0.75,   0.8),
             'oldlace':     (1.0,   1.0,    0.9)}

lastHour = -2147
currentHour = -2137

while True:
    print("current hour:" + str(currentHour) + " | lastHour: " + str(lastHour))
    now = datetime.now()
    currentHour = int(now.strftime("%H"))
    if currentHour != lastHour:
        print("Hour change")
        if currentHour == 6:
            print("full brightness")
            arduino.write("BR1".encode())
        elif currentHour == 21:
            print("half brightness")
            arduino.write("BR2".encode())
        elif currentHour == 0:
            print("low brightness")
            arduino.write("BR1".encode())
        elif currentHour == 1:
            print("no brightness")
            arduino.write("BR0".encode())
    if (arduino.inWaiting() > 0):
        data_str = arduino.read(arduino.inWaiting()).decode('utf-8') 
        print(data_str)
    try:                                                # Attempt the following:
        with urlopen(cheerlightsUrl) as colourResponse:
            colourName = colourResponse.read().decode("utf-8")
            if colourName in colourMap:                   # If we recognise this colour name then ...
                red, green, blue = colourMap[colourName]            # Get the LedBorg colour to use from the name
            else:
                red, green, blue = (0.0, 0.5, 0.5)
            print("current web color: " + colourName)
            SetLedBorg(red, green, blue)
    except Exception as e:                                             # If we have an error
        print("ZESER:")
        print(e)
        pass                                                # Ignore it (do nothing)
    finally:                                            # Regardless of errors:
        lastHour = currentHour
        time.sleep(10)

