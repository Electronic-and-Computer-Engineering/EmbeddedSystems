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

## Unterlagen
- [Crazy Car Schematic](https://fhjoanneum-my.sharepoint.com/:b:/g/personal/florian_mayer_fh-joanneum_at/EfXYu-rqsLRErJbybsbN4AEB_RUMizJhwpb5D_ysimZehA?e=Ti7PtO)
- [MSP430f5335-Datasheet](https://www.ti.com/lit/ds/symlink/msp430f5335.pdf)
- [MSP430x5xx and MSP430x6xx Family Guide](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf)


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
<summary><strong>4–6: PWM, SPI, Display</strong></summary>

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

</details>

<details>
<summary><strong>7-9: ADC, DMA, Sensorik</strong></summary>

### 7. [ADC-Konfiguration](Kapitel_07_ADC/README.md)
- Einrichtung des ADC12_A
- Timer-gesteuerte Abtastung (120 Hz)
- Zwischenspeicherung in Datenstruktur

### 8. [ADC mit DMA](Kapitel_08_ADC_DMA/README.md)
- DMA0 für automatischen Speichertransfer
- Status-Flag Handling

### 9. [Sharp Abstandssensoren](Kapitel_9_Abstandssensoren/README.md)
- Messung und Darstellung der Sensor-Kennlinie
- Linearisierung: Lookup-Table vs. Approximation
- Filterung (Moving Average)

</details>

<details>
<summary><strong>10: Fahralgorithmen</strong></summary>

### 10. [Fahralgorithmen](Kapitel_11_Fahralgorithmen/README.md)
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
