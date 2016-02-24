#!/bin/bash
/usr/bin/python /home/james/git/cvproj/startgpio.py

sleep 30 
#if stdout from python contains a 1 it means the pin has been jumped

/usr/bin/python /home/james/git/cvproj/checkgpio.py | grep -q "1" && /bin/bash /home/james/git/cvproj/create_adhoc.sh
