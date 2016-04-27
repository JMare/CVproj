#!/bin/bash

#this will create an ad hoc network to connect to in the field
#connection not great

ifconfig wlan0 down
iwconfig wlan0 mode ad-hoc
iwconfig wlan0 key aaaaa11111
iwconfig wlan0 essid RPi
ifconfig wlan0 10.0.0.200 netmask 255.255.255.0 up
dhcpd wlan0
echo "Ad hoc network created"

