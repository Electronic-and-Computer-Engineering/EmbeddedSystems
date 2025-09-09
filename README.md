# EmbeddedSystems
Layerbasierte Embedded-Systems-Laborübung mit MSP430F5335 und Crazy Car Plattform. Ziel ist die Entwicklung eines autonomen Mini-Fahrzeugs inkl. GPIO, Timer, PWM, ADC (DMA), SPI-Display, Sensorik und Regelalgorithmen in C. Projektstruktur mit HAL, DL, AL. Entwicklung mit Code Composer Studio.

## Projektüberblick

Dieses Repository begleitet die Embedded-Systems-Laborreihe im Studiengang Elektronik und Computer Engineering (FH JOANNEUM). Im Zentrum steht die systematische Entwicklung eines autonomen Fahrzeugs (Crazy Car) auf Basis des MSP430F5335-Mikrocontrollers.
Die Übung vermittelt praxisnah die hardwarenahe Programmierung, modulare Softwarearchitektur und den systematischen Umgang mit Mikrocontroller-Peripherie.

## Kapitelübersicht & Aufgabenstellungen

<details>
<summary>Expand</summary>

### 1. [Einführung und Projektstruktur](Kapitel_01_Einfuehrung/README.md)
- Crazy Car Platine – Hardwareübersicht
- Softwarearchitektur: HAL / DL / AL
- Projektaufbau in Code Composer Studio
- Git-Versionierung

### 2. [Digitale Ein-/Ausgabe](Kapitel_02_GPIO/README.md)
- GPIO-Konfiguration
- Interruptsteuerung für Start-/Stoptaste
- Performance-Messung (Integer vs. Float)
- Debugging mit Breakpoints

### 3. [Clock System und Timer B0](Kapitel_03_TimerB0/README.md)
- Unified Clock System (UCS)
- TimerB0-Konfiguration
- Periodische Aufgaben & Oszilloskopmessung

### 4. [PWM und Aktorik](Kapitel_04_PWM_Aktorik/README.md)
- PWM mit Timer A1
- Servo- und ESC-Steuerung
- Abstraktion über Driver Layer

### 5. [SPI-Kommunikation](Kapitel_05_SPI/README.md)
- USCI_B1 im SPI-Master-Modus
- Interruptgesteuerte Übertragung
- Chip Select Handling

### 6. [LC-Display Ansteuerung](Kapitel_06_LCD/README.md)
- Initialisierung
- Text-/Zahlenausgabe
- Cursor-Positionierung

### 7. [SPI / LCD-Integration](Kapitel_07_SPI_LCD/README.md)
- Zusammenspiel von Displaydriver und SPI
- Vollständiger Textausgabestack

### 8. [ADC-Konfiguration](Kapitel_08_ADC/README.md)
- Konfiguration des ADC12_A
- Sensorabfrage über Timer-Trigger
- Pufferung und Flag-Handling

### 9. [ADC mit DMA](Kapitel_09_ADC_DMA/README.md)
- Automatischer Datenübertrag per DMA0
- Entkopplung von CPU und ADC

### 10. [Sharp Abstandssensoren](Kapitel_10_SharpSensoren/README.md)
- Kennlinienmessung
- Linearisierung: Formel oder Lookup-Table

### 11. [Fahralgorithmen](Kapitel_11_Fahralgorithmen/README.md)
- Regelung (z. B. PID, statische Schwellen)
- Zustandsautomat
- Spurhaltung, Bandeverfolgung, Kurvenerkennung

</details>

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
