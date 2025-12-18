#!/bin/bash
avr-gcc -mmcu=atmega48 -Os -o main.elf  src/main.c
avr-objcopy -O ihex main.elf  main.hex
avrdude -c usbasp-clone -p m48 -U flash:w:main.hex