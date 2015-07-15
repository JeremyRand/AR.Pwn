#!/bin/bash

echo "Building AR.Pwn Hook..."
echo 

droneFlag=""

if [ "$1" == "DRONEV2" ]; then
    droneFlag="-D"$1
fi

"arm-none-linux-gnueabi-gcc.exe" -shared -fPIC -ldl -o libhook.so hook.c $droneFlag

echo 
echo "Build finished, Press Enter to exit."
read DUMMY
