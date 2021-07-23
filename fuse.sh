sudo avrdude -c usbasp -p m2561 -U lfuse:w:0xff:m -U hfuse:w:0xd0:m -U efuse:w:0xff:m
