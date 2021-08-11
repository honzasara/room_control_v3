#!/bin/bash

./bootloader-programuj.sh

tftp 192.168.2.169 -v -m binary -c put room_control_v3.bin 
