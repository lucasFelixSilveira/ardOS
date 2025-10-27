package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"os/signal"
	"runtime"
	"strings"
	"syscall"

	"github.com/tarm/serial"
	"golang.org/x/term"
)

func main() {
	// Solicita a escolha da plataforma e define o baud rate
	platform, baudRate := selectPlatform()

	portName := detectDefaultPort()
	c := &serial.Config{Name: portName, Baud: baudRate}
	port, err := serial.OpenPort(c)
	if err != nil {
		log.Fatalf("Error to open serial port %s: %v", portName, err)
	}
	defer port.Close()

	oldState, err := term.MakeRaw(int(os.Stdin.Fd()))
	if err != nil {
		log.Fatalf("Error to enable raw mode: %v", err)
	}
	defer term.Restore(int(os.Stdin.Fd()), oldState)

	fmt.Printf("Connected to the %s! %s@%d\r\n", platform, portName, baudRate)

	sigs := make(chan os.Signal, 1)
	signal.Notify(sigs, syscall.SIGINT, syscall.SIGTERM)

	go func() {
		buf := make([]byte, 1)
		for {
			n, err := port.Read(buf)
			if err != nil {
				log.Printf("Erro na leitura da serial: %v", err)
				continue
			}
			if n > 0 {
				fmt.Print(string(buf[:n]))
			}
		}
	}()

	input := make([]byte, 1)
	for {
		select {
		case <-sigs:
			fmt.Println("\nEncerrando conexão.")
			return
		default:
			n, err := os.Stdin.Read(input)
			if err != nil {
				log.Printf("Erro na leitura do teclado: %v", err)
				continue
			}
			if n > 0 {
				_, err := port.Write(input[:n])
				if err != nil {
					log.Printf("Erro ao escrever na serial: %v", err)
				}
			}
		}
	}
}

func detectDefaultPort() string {
	switch runtime.GOOS {
	case "windows":
		return "COM3"
	case "linux":
		return "/dev/ttyUSB0"
	case "darwin":
		return "/dev/tty.usbserial"
	default:
		log.Fatal("Sistema não suportado")
		return ""
	}
}

func selectPlatform() (string, int) {
	reader := bufio.NewReader(os.Stdin)
	fmt.Println("Selecione a plataforma (xtensa ou atmega):")
	for {
		platform, err := reader.ReadString('\n')
		if err != nil {
			log.Printf("Erro ao ler a entrada: %v", err)
			continue
		}
		platform = strings.TrimSpace(strings.ToLower(platform))
		switch platform {
		case "xtensa":
			return "Xtensa", 115200
		case "atmega":
			return "ATmega", 9600
		default:
			fmt.Println("Plataforma inválida. Por favor, escolha 'xtensa' ou 'atmega':")
		}
	}
}
