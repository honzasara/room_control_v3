#!/bin/bash
date
ls -la room_control_v3.ino_atmega2561_16000000L.hex && avr-objcopy -I ihex room_control_v3.ino_atmega2561_16000000L.hex -O binary room_control_v3.bin
