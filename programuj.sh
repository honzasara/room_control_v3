#!/bin/bash
date
ls -la room_control_v3.ino_atmega2561_16000000L.hex && sudo avrdude -c usbasp -p m2561 -V  -U flash:w:room_control_v3.ino_atmega2561_16000000L.hex:i && rm room_control_v3.ino_atmega2561_16000000L.hex
