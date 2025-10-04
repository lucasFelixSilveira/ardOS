#!/bin/bash

# === CONFIGURAÇÕES ===
MCU=atmega328p
F_CPU=16000000UL
BAUD=115200
PORT=/dev/ttyUSB0
PROGRAMMER=arduino

SRC_DIR=src
INC_DIR=include
OUT_DIR=build
ELF_NAME=main.elf
HEX_NAME=main.hex

# === PREPARAÇÃO ===
mkdir -p $OUT_DIR

# === COMPILAÇÃO ===
echo "[1/3] Compilando C..."
avr-gcc -Wall -Os -DF_CPU=$F_CPU -mmcu=$MCU -I$INC_DIR -o $OUT_DIR/$ELF_NAME $SRC_DIR/main.c

# === CONVERSÃO PARA HEX ===
echo "[2/3] Gerando HEX..."
avr-objcopy -O ihex -R .eeprom $OUT_DIR/$ELF_NAME $OUT_DIR/$HEX_NAME

# === UPLOAD VIA AVRDUDE ===
echo "[3/3] Enviando para Arduino Uno..."
avrdude -v -p $MCU -c $PROGRAMMER -P $PORT -b $BAUD -D -U flash:w:$OUT_DIR/$HEX_NAME:i

echo "✅ Upload completo!"
