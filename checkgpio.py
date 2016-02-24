import RPi.GPIO as GPIO
import time
import sys 

inPin = 21
GPIO.setmode(GPIO.BCM) # Broadcom pin-numbering scheme

GPIO.setup(inPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

if GPIO.input(inPin):
    #wire not there
    sys.stdout.write('0\n')
    sys.stdout.flush()
    sys.exit(0)
else :
    #wirethere
    sys.stdout.write('1\n')
    sys.stdout.flush()
    sys.exit(0)
