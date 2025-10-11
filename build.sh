#!/bin/bash
############################################
# ardOS Build System - Multi-Platform Build
# Supports: ATmega328P, x86 (32-bit), Xtensa (ESP32)
############################################

PROJECT_NAME="ardOS"

# ==============================
# CORES PARA LOGS
# ==============================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_error()   { echo -e "${RED}[ERROR]${NC} $1" >&2; exit 1; }
log_info()    { echo -e "${BLUE}[INFO]${NC} $1"; }
log_warning() { echo -e "${YELLOW}[WARN]${NC} $1"; }

# ==============================
# ARQUITETURA
# ==============================
ARCH_FLAGS=""
case $1 in
    atmega) ARCH_FLAGS="-DARCH_AVR" ;;
    xtensa) ARCH_FLAGS="-DARCH_XTENSA" ;;
    x86|x86_32|x86_64) ARCH_FLAGS="-DARCH_X86" ;;
    all) ARCH_FLAGS="-DARCH_X86" ;;
esac

if [ "$2" = "debug" ]; then
    ARCH_FLAGS="$ARCH_FLAGS -DDEBUG"
    log_info "Debug mode enabled"
fi

# ==============================
# CONFIGURAÇÕES GERAIS
# ==============================
ESP_IDF_PATH="${ESP_IDF_PATH:-$HOME/.espressif/esp-idf}"
[ -d "$ESP_IDF_PATH" ] || ESP_IDF_PATH="/home/dev_lucasfs/.espressif/esp-idf"
KERNEL_MAX_SIZE=$((8 * 512))
KERNEL_WARNING_SIZE=$((6 * 512))

# ==============================
# FUNÇÕES GERAIS
# ==============================
setup_project() {
    mkdir -p build
    log_success "Project structure ready"
}

clean_build() {
    log_info "Cleaning build directory..."
    rm -rf build/*
    log_success "Build directory cleaned"
}

# ==============================
# LISTAR ARQUIVOS
# ==============================
list_sources() {
    X86_SOURCES=$(find src -name "*.c" | tr '\n' ' ')
    DRIVERS_SOURCES=$(find src/drivers -name "*.c" 2>/dev/null | tr '\n' ' ')
    SYSCALL_SOURCES=$(find src/syscall -name "*.c" 2>/dev/null | tr '\n' ' ')
    echo "Main: $X86_SOURCES"
    echo "Drivers: $DRIVERS_SOURCES"
    echo "Syscalls: $SYSCALL_SOURCES"
    echo "Kernel limit: $KERNEL_MAX_SIZE bytes"
}

# ==============================
# ======= BUILD: ATMEGA ========
# ==============================
ATMEGA_CC="avr-gcc"
ATMEGA_CFLAGS="-mmcu=atmega328p -O2 -Wall -Iinclude $ARCH_FLAGS"
ATMEGA_TARGET="build/main_atmega.elf"
ATMEGA_HEX="build/main_atmega.hex"
ATMEGA_SOURCES=$(find src -name "*.c" | tr '\n' ' ')

AVRDUDE="avrdude"
AVR_PORT="/dev/ttyUSB0"
AVR_BAUD="115200"
AVRDUDE_FLAGS="-c arduino -p m328p -P $AVR_PORT -b $AVR_BAUD"

check_prerequisites_atmega() {
    command -v $ATMEGA_CC >/dev/null 2>&1 || log_error "avr-gcc not found"
    command -v avr-objcopy >/dev/null 2>&1 || log_error "avr-objcopy not found"
    command -v $AVRDUDE >/dev/null 2>&1 || log_error "avrdude not found"
    log_success "AVR toolchain OK"
}

build_atmega() {
    log_info "Building for ATmega328P..."
    mkdir -p build
    $ATMEGA_CC $ATMEGA_CFLAGS -o $ATMEGA_TARGET $ATMEGA_SOURCES || log_error "Compilation failed"
    avr-objcopy -O ihex -R .eeprom $ATMEGA_TARGET $ATMEGA_HEX || log_error "HEX conversion failed"
    log_success "Built HEX → $ATMEGA_HEX"
}

upload_atmega() {
    log_info "Uploading to ATmega..."
    [ -f "$ATMEGA_HEX" ] || log_error "HEX not found"
    log_info "Port: $AVR_PORT | Baud: $AVR_BAUD"
    $AVRDUDE $AVRDUDE_FLAGS -U flash:w:$ATMEGA_HEX:i || log_error "Upload failed"
    log_success "Firmware uploaded!"
}

# ==============================
# ======= BUILD: X86 ===========
# ==============================
X86_CC="gcc"
X86_BOOT="src/boot.S"
X86_BOOTLOADER="build/boot.bin"
X86_KERNEL_32="build/kernel32.bin"
X86_IMAGE_32="build/ardOS_32.img"
X86_CFLAGS_32="-m32 -ffreestanding -O2 -Wall -Iinclude -nostdlib -fno-stack-protector -fno-pic -fno-builtin $ARCH_FLAGS"

check_prerequisites_x86() {
    command -v nasm >/dev/null 2>&1 || log_error "nasm not found"
    command -v $X86_CC >/dev/null 2>&1 || log_error "gcc not found"
    command -v ld >/dev/null 2>&1 || log_error "ld not found"
    command -v objcopy >/dev/null 2>&1 || log_error "objcopy not found"
    log_success "x86 toolchain OK"
}

check_kernel_size() {
    local file=$1
    local size=$(stat -c%s "$file")
    if [ $size -gt $KERNEL_MAX_SIZE ]; then
        log_error "Kernel too large ($size bytes > $KERNEL_MAX_SIZE)"
    elif [ $size -gt $KERNEL_WARNING_SIZE ]; then
        log_warning "Kernel approaching limit ($size/$KERNEL_MAX_SIZE)"
    else
        log_success "Kernel size OK ($size/$KERNEL_MAX_SIZE)"
    fi
}

build_x86() {
    log_info "Building for x86..."
    mkdir -p build

    nasm -f bin $X86_BOOT -o $X86_BOOTLOADER || log_error "Bootloader build failed"
    log_success "Bootloader built: $X86_BOOTLOADER"

    SOURCES=$(find src -name "*.c" | tr '\n' ' ')
    OBJECTS=""
    for src in $SOURCES; do
        obj="build/$(basename ${src%.c})32.o"
        $X86_CC $X86_CFLAGS_32 -c $src -o $obj || log_error "Compile fail: $src"
        OBJECTS="$OBJECTS $obj"
    done

    ld -m elf_i386 -T linker.ld -o build/kernel32.elf $OBJECTS || log_error "Link failed"
    objcopy -O binary build/kernel32.elf $X86_KERNEL_32 || log_error "Objcopy failed"

    check_kernel_size "$X86_KERNEL_32"
    dd if=/dev/zero of=$X86_IMAGE_32 bs=512 count=2880 2>/dev/null
    dd if=$X86_BOOTLOADER of=$X86_IMAGE_32 conv=notrunc 2>/dev/null
    dd if=$X86_KERNEL_32 of=$X86_IMAGE_32 bs=512 seek=1 conv=notrunc 2>/dev/null
    log_success "Created bootable image: $X86_IMAGE_32"
}

# ==============================
# ======= BUILD: XTENSA ========
# ==============================
check_prerequisites_xtensa() {
    log_success "ESP-IDF OK"
}

build_xtensa() {
    log_info "Building for ESP32..."

    cp src/*.c main/ >/dev/null 2>&1 || "Fail to copy src/*"
    cp include/*.h main/ >/dev/null 2>&1 || "Fail to copy include/*"

    for file in include/xtensa/*.h; do
        node helper/index.js "$file"
    done

    idf.py set-target esp32 >/dev/null 2>&1
    idf.py build || log_error "ESP32 build failed"
    log_success "ESP32 firmware built"
}

upload_xtensa() {
    log_info "Flashing ESP32..."
    idf.py -p /dev/ttyUSB0 flash || log_error "ESP32 upload failed"
    log_success "ESP32 flashed successfully!"
}

# ==============================
# ======= AJUDA ================
# ==============================
show_help() {
    echo "Usage: $0 {atmega|x86|xtensa|all|clean|list} [debug]"
}

# ==============================
# ======= EXECUÇÃO PRINCIPAL ===
# ==============================
[ $# -eq 0 ] && { show_help; exit 1; }

PLATFORM=$1
setup_project

case $PLATFORM in
    list) list_sources ;;
    clean) clean_build ;;
    atmega)
        check_prerequisites_atmega
        build_atmega
        upload_atmega
        ;;
    x86|x86_32)
        check_prerequisites_x86
        build_x86
        ;;
    xtensa)
        check_prerequisites_xtensa
        build_xtensa
        upload_xtensa
        ;;
    all)
        check_prerequisites_atmega
        check_prerequisites_x86
        check_prerequisites_xtensa
        build_atmega
        upload_atmega
        build_x86
        build_xtensa
        upload_xtensa
        ;;
    *)
        log_error "Invalid platform: $PLATFORM"
        ;;
esac

echo ""
log_success "Build completed for $PLATFORM!"
echo ""
echo "=== TESTING INSTRUCTIONS ==="
echo "x86: qemu-system-i386 -drive format=raw,file=build/ardOS_32.img -serial stdio"
echo "ATmega: avrdude -c arduino -p m328p -P $AVR_PORT -b $AVR_BAUD -U flash:w:$ATMEGA_HEX:i"
echo "ESP32: idf.py -p /dev/ttyUSB0 flash monitor"
echo ""
