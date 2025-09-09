# EmbeddedSystems

Layerbasierte Embedded-Systems-Laborübung mit MSP430F5335 und Crazy Car Plattform. Ziel ist die Entwicklung eines autonomen Mini-Fahrzeugs inkl. GPIO, Timer, PWM, ADC (DMA), SPI-Display, Sensorik und Regelalgorithmen in C. Projektstruktur mit HAL, DL, AL. Entwicklung mit Code Composer Studio.

## Laborübersicht

Dieses Repository begleitet die Embedded-Systems-Laborreihe im Studiengang Elektronik und Computer Engineering (FH JOANNEUM). Im Zentrum steht die systematische Entwicklung eines autonomen Fahrzeugs (Crazy Car) auf Basis des MSP430F5335-Mikrocontrollers.

Die Übung vermittelt praxisnah:
- Hardwarenahe C-Programmierung
- Strukturierte Layer-Architektur (HAL / DL / AL)
- Debugging, Registerzugriffe, ISR
- Modularisierung und Wiederverwendbarkeit von Komponenten
---

## Kapitelübersicht & Aufgabenstellungen

<details>
<summary><strong>1–3: Einführung, GPIO, Timer</strong></summary>

### 1. [Einführung und Projektstruktur](Kapitel_01_Einfuehrung/README.md)
- Überblick zur Crazy Car Platine
- Softwarearchitektur: HAL, DL, AL
- Projektstruktur in CCS
- Git-Versionierung & Setup

### 2. [Digitale Ein-/Ausgabe](Kapitel_02_GPIO/README.md)
- GPIO-Initialisierung
- Interruptgesteuerte Tasterauswertung
- Performancevergleich: Integer vs. Float
- Debugging (Breakpoints, Register, Expressions)

### 3. [Clock System und Timer B0](Kapitel_03_TimerB0/README.md)
- Unified Clock System (UCS)
- TimerB0: ISR-basierte LED/PWM-Steuerung
- Frequenzmessung per Oszilloskop

</details>

<details>
<summary><strong>4–7: PWM, SPI, Display</strong></summary>

### 4. [PWM und Aktorik](Kapitel_04_PWM_Aktorik/README.md)
- PWM mit TimerA1
- Ansteuerung von Servo & ESC
- Driver Layer für Lenkung und Gas

### 5. [SPI-Kommunikation](Kapitel_05_SPI/README.md)
- USCI_B1 SPI-Konfiguration
- Interruptgesteuerte Übertragung
- CS-Signal Handling

### 6. [LC-Display Ansteuerung](Kapitel_06_LCD/README.md)
- Displayinitialisierung (ST7565)
- Zeichenausgabe, Cursorpositionierung
- Zeichentabelle und Clear-Routinen

### 7. [SPI / LCD-Integration](Kapitel_07_SPI_LCD/README.md)
- Kopplung von Displayfunktionen und SPI
- Aufbau einer robusten Textausgabe
- Test aller Pixel (Vollbildtest)

</details>

<details>
<summary><strong>8–10: ADC, DMA, Sensorik</strong></summary>

### 8. [ADC-Konfiguration](Kapitel_08_ADC/README.md)
- Einrichtung des ADC12_A
- Timer-gesteuerte Abtastung (120 Hz)
- Zwischenspeicherung in Datenstruktur

### 9. [ADC mit DMA](Kapitel_09_ADC_DMA/README.md)
- DMA0 für automatischen Speichertransfer
- Status-Flag Handling

### 10. [Sharp Abstandssensoren](Kapitel_10_SharpSensoren/README.md)
- Messung und Darstellung der Sensor-Kennlinie
- Linearisierung: Lookup-Table vs. Approximation
- Filterung (Moving Average)

</details>

<details>
<summary><strong>11: Fahralgorithmen</strong></summary>

### 11. [Fahralgorithmen](Kapitel_11_Fahralgorithmen/README.md)
- Zustandsautomat: Links / Mitte / Rechts
- Regler (z. B. PID) für Lenkung und Geschwindigkeit
- Umsetzung einfacher Fahrstrategien:
  - Bandeverfolgung
  - Spurmitte halten
  - Kurvenkompensation
- Nutzung aller verfügbaren Sensoren

</details>

---

## Ziele

- Modularisierung der Embedded Software (Layerstruktur)
- Verständnis für low-level Hardwareansteuerung
- Entwicklung von Steuerungs- und Regelalgorithmen
- Erweiterung um zusätzliche Peripherie und Sensordatenverarbeitung
- Umsetzung eines lauffähigen autonomen Systems auf Mikrocontroller-Basis

---

## Projektstruktur

Die Projektstruktur folgt dem klassischen Layer-Prinzip:

- HAL          – Hardware Abstraction Layer (Registerzugriff)
- DL           – Driver Layer (Komponentensteuerung)
- AL           – Application Layer (Applikationslogik, Statemachine)
- main.c       – Einstiegspunkt, Systeminitialisierung
- include      – Globale Header und Definitionen

---

## Verwendete Tools

- Mikrocontroller: MSP430F5335 (Texas Instruments)
- Entwicklungsumgebung: Code Composer Studio (TI)
- Debugger: Spy-by-Wire / JTAG
- Dokumentation: TI User Guide, Schaltpläne, Datenblätter
- Versionsverwaltung (optional empfohlen): GitLab, GitHub, Git

---

## Voraussetzungen

- Grundkenntnisse in C (Bitmasken, Pointer, Headerstrukturen)
- Verständnis für Mikrocontroller-Peripherie
- Umgang mit Code Composer Studio und Debugging-Werkzeugen

---

## Git / Versionierung

Es wird empfohlen, das Projekt versionsverwaltet in einem GitLab- oder GitHub-Repository zu entwickeln. Ein typischer Initialisierungsvorgang:

```bash
git init
git remote add origin https://gitlab.com/<benutzer>/<projekt>.git
git add .
git commit -m "Initial commit"
git push -u origin master
