package main

import (
	"fmt"
	"log"
	"os"
	"os/signal"
	"runtime"
	"syscall"

	"github.com/tarm/serial"
	"golang.org/x/term"
)

func main() {
	portName := detectDefaultPort()
	baudRate := 9600

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

	fmt.Printf("Conntected to the Arduino! %s@%d\r\n", portName, baudRate)

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
