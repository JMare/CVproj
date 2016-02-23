import RPi.GPIO as GPIO
import time
import os

inPin = 21

GPIO.setup(inPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

if GPIO.input(inPin):
    #wire not there
    pinsJumped = false
else :
    pinsJumped = true


if pinsJumped:
    sys.stdout.write('1\n')
    sys.stdout.flush()
    sys.exit(0)
else :
    sys.stdout.write('0\n')
    sys.stdout.flush()
    sys.exit(0)
