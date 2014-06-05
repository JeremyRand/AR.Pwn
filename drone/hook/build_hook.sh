#!/bin/bash

echo "Building AR.Pwn Hook..."
echo 

"arm-none-linux-gnueabi-gcc.exe" -shared -fPIC -ldl -o libhook.so hook.c

echo 
echo "Build finished, Press Enter to exit."
read DUMMY