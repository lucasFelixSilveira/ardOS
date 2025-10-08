#!/bin/bash

# === CONFIGURAÇÕES ===
MCU=atmega328p
F_CPU=16000000UL
BAUD=115200
PORT=/dev/ttyUSB0
PROGRAMMER=arduino

SRC_DIR=atmega328p/src
INC_DIR=atmega328p/include
OUT_DIR=atmega328p/build
ELF_NAME=main.elf
HEX_NAME=main.hex

# === PREPARAÇÃO ===
mkdir -p $OUT_DIR

# === COMPILAÇÃO ===
echo "[1/3] Compilando C..."

# Descobrir automaticamente todos os arquivos .c
C_FILES=$(find $SRC_DIR -name "*.c" | sort)
C_COUNT=$(echo "$C_FILES" | wc -l)

echo "📁 Encontrados $C_COUNT arquivos .c:"
echo "$C_FILES" | sed 's/^/  - /'
echo ""

avr-gcc -Wall -Os -DF_CPU=$F_CPU -mmcu=$MCU -I$INC_DIR -o $OUT_DIR/$ELF_NAME $C_FILES

if [ $? -eq 0 ]; then
    echo "✅ Compilação bem-sucedida!"
else
    echo "❌ Erro na compilação!"
    exit 1
fi

# === CONVERSÃO PARA HEX ===
echo "[2/3] Gerando HEX..."
avr-objcopy -O ihex -R .eeprom $OUT_DIR/$ELF_NAME $OUT_DIR/$HEX_NAME

if [ $? -eq 0 ]; then
    echo "✅ Arquivo HEX gerado com sucesso!"
else
    echo "❌ Erro ao gerar arquivo HEX!"
    exit 1
fi

# === UPLOAD VIA AVRDUDE ===
echo "[3/3] Enviando para Arduino Uno..."
echo "🔌 Porta: $PORT | Programador: $PROGRAMMER | MCU: $MCU"

if avrdude -v -p $MCU -c $PROGRAMMER -P $PORT -b $BAUD -D -U flash:w:$OUT_DIR/$HEX_NAME:i; then
    echo "✅ Upload completo!"
else
    echo "❌ Erro no upload! Verifique se:"
    echo "  - O Arduino está conectado na porta $PORT"
    echo "  - Você tem permissão para acessar a porta (sudo chmod 666 $PORT)"
    echo "  - O Arduino não está sendo usado por outro programa"
    exit 1
fi
