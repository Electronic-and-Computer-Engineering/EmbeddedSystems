# EmbeddedSystems
Layerbasierte Embedded-Systems-Laborübung mit MSP430F5335 und Crazy Car Plattform. Ziel ist die Entwicklung eines autonomen Mini-Fahrzeugs inkl. GPIO, Timer, PWM, ADC (DMA), SPI-Display, Sensorik und Regelalgorithmen in C. Projektstruktur mit HAL, DL, AL. Entwicklung mit Code Composer Studio.

## Projektüberblick

Dieses Repository begleitet die Embedded-Systems-Laborreihe im Studiengang Elektronik und Computer Engineering (FH JOANNEUM). Im Zentrum steht die systematische Entwicklung eines autonomen Fahrzeugs (Crazy Car) auf Basis des MSP430F5335-Mikrocontrollers.

Die Übung vermittelt praxisnah die hardwarenahe Programmierung, modulare Softwarearchitektur und den systematischen Umgang mit Mikrocontroller-Peripherie.

## Ziele

- Strukturierte Embedded-Programmierung in C
- Layer-basierte Softwareentwicklung (HAL, DL, AL)
- Initialisierung und Nutzung folgender Module:
  - Digitale Ein-/Ausgänge (inkl. Interrupts)
  - Timer (TimerA1 für PWM, TimerB0 für ISR)
  - ADC12_A mit DMA
  - SPI-Kommunikation (Display)
  - LC-Display-Initialisierung und -Ausgabe
  - Linearisierung analoger Sensorsignale (Sharp IR)
- Integration eines einfachen Fahr- und Regelalgorithmus (z. B. PID)

## Projektstruktur

Die Projektstruktur orientiert sich an einem klassischen Layer-Modell:

- HAL          – Hardware Abstraction Layer  
- DL           – Driver Layer  
- AL           – Application Layer  
- main.c       – Hauptfunktion  
- include      – Globale Header und Definitionen

Jede Ebene erfüllt eine klar abgegrenzte Funktion:

- **HAL**: Direkte Initialisierung der Peripherie (z. B. GPIO, Timer, ADC)  
- **DL**: Logik für konkrete Komponenten (z. B. Display, Motorsteuerung)  
- **AL**: Anwendungslogik, Zustandsautomat, Hauptschleife

## Verwendete Tools

- Mikrocontroller: MSP430F5335 (Texas Instruments)
- Entwicklungsumgebung: Code Composer Studio (TI)
- Debugger: Spy-by-Wire Interface / JTAG
- Dokumentation: MSP430x5xx/6xx Family User Guide, Datenblätter
- Versionskontrolle (empfohlen): GitLab oder GitHub

## Voraussetzungen

- Grundlagen in C (Bitmasken, Pointer, Headerstruktur)
- Verständnis von Mikrocontroller-Peripherie
- Umgang mit Registerkonfiguration und Interrupts
- Erfahrung mit Code Composer Studio ist von Vorteil

## Git / Versionierung

Die Nutzung eines Git-Repositories wird empfohlen. 
