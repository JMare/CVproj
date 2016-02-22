#!/bin/bash

cd /home/pi/git/PiBits/ServoBlaster/user
./servod

echo 5=50% > /dev/servoblaster
echo 6=50% > /dev/servoblaster
