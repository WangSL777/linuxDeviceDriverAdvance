#!/bin/sh

rmmod /dev/device4MB
make clean
make
if [ -f "device4MB.ko" ]
then
	dmesg -c
	insmod device4MB.ko
	cat /dev/device4MB
	dmesg | tail
fi
