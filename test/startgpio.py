import RPi.GPIO as GPIO
import time
import sys 

inPin = 21
GPIO.setmode(GPIO.BCM) # Broadcom pin-numbering scheme

GPIO.setup(inPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
