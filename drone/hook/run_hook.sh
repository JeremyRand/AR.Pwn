#!/bin/ash

cd /update/snarc/

killall program.elf
killall program_backup.elf
killall TelnetInterface
killall TelnetInterfaceBlueMarker

sleep 5

LD_PRELOAD=./libhook.so ./program_backup.elf >/dev/null &
