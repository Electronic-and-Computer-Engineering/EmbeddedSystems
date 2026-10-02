# EmbeddedSystems

Layerbasierte Embedded-Systems-Laborübung mit MSP430F5335 und Crazy Car Plattform. Ziel ist die Entwicklung eines autonomen Mini-Fahrzeugs inkl. GPIO, Timer, PWM, ADC (DMA), SPI-Display, I2C-Sensorik, FreeRTOS und Regelalgorithmen in C. Projektstruktur mit HAL, DL, AL. Entwicklung mit Code Composer Studio, Verifikation mit Unity.

## Laborübersicht

Dieses Repository begleitet die Embedded-Systems-Laborreihe im Studiengang Elektronik und Computer Engineering (FH JOANNEUM). Im Zentrum steht die systematische Entwicklung eines autonomen Fahrzeugs (Crazy Car) auf Basis des MSP430F5335-Mikrocontrollers.

Die Laborreihe gliedert sich in neun inhaltlich geführte Einheiten. Die verbleibenden Termine dienen dem freien Fahren und der Vorbereitung auf das Crazy Car Event.

Die Übung vermittelt praxisnah:
- Hardwarenahe C-Programmierung
- Strukturierte Layer-Architektur (HAL / DL / AL)
- Debugging, Registerzugriffe, ISR
- Messtechnische Verifikation mit Oszilloskop und Logic-Analyzer
- Testen mit Unity: von bereitgestellten Vorlagen bis zur eigenständigen Suite
- Modularisierung und Wiederverwendbarkeit von Komponenten
---

## Unterlagen
- [Crazy Car Schematic](https://fhjoanneum-my.sharepoint.com/:b:/g/personal/florian_mayer_fh-joanneum_at/EfXYu-rqsLRErJbybsbN4AEB_RUMizJhwpb5D_ysimZehA?e=Ti7PtO)
- [MSP430f5335-Datasheet](https://www.ti.com/lit/ds/symlink/msp430f5335.pdf)
- [MSP430x5xx and MSP430x6xx Family Guide](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf)
- [ISR-Vector List](ISR_VectorList.md)


## Einheitsübersicht & Aufgabenstellungen

<details>
<summary><strong>1–3: Einführung, Interrupts, Clock und Timer</strong></summary>

### 1. [Grundlagen, Werkzeuge, Registereinstieg](Kapitel_01_Einfuehrung/README.md)
- Überblick zur Crazy Car Platine
- Softwarearchitektur: HAL, DL, AL
- Projektstruktur in CCS, Git-Versionierung
- Memory-Mapped I/O, Pin-Header, GPIO-Konfiguration

### 2. [Rechendauer, Interrupts, ISRs](Einheit_02_Interrupts/README.md)
- Rechendauer: Integer vs. Float
- Interruptgesteuerte Tasterauswertung, Entprellen
- Polling vs. Delay: zeitliche Zuverlässigkeit
- Debugging (Breakpoints, Register, Expressions)

### 3. [SystemClock und TimerB](Einheit_03_Clock_TimerB/README.md)
- Unified Clock System (UCS)
- TimerB0-Konfiguration
- Soll- vs. Ist-Taktfrequenz am Oszilloskop
- Einstieg Unity: Gerüst vorgegeben, Grenzfälle ergänzen

</details>

<details>
<summary><strong>4–5: PWM, Aktorik und Schnittstellen</strong></summary>

### 4. [TimerA und PWM](Einheit_04_TimerA_PWM/README.md)
- PWM mit TimerA1, Ansteuerung von Servo & ESC
- Driver Layer für Lenkung und Beschleunigung
- Drehzahl, Geschwindigkeit und Weg über TimerA
- MockHAL für DL-Tests

### 5. [Schnittstellen: SPI, UART, I2C](Einheit_05_Schnittstellen/README.md)
- SPI über USCI_B1 selbst konfigurieren, interruptgesteuerte Übertragung
- UART und I2C als vorgegebene Module einbinden und erweitern
- Alle drei Schnittstellen am Logic-Analyzer messen und dekodieren

</details>

<details>
<summary><strong>6–7: Sensorik, Datenerfassung, Display</strong></summary>

### 6. [Abstandssensoren, ADC und DMA](Einheit_06_Sensorik_ADC_DMA/README.md)
- ToF-Sensoren und API angeleitet einbinden, Adressvergabe über XSHUT
- ADC12_A aufsetzen, timergesteuerte Abtastung, Batteriespannung
- Erfassung auf DMA umstellen, DL-Tests müssen unverändert bestehen
- Verifikation ausschließlich im Debugger

### 7. [Display und Kommunikation](Einheit_07_Display_Kommunikation/README.md)
- Displayinitialisierung (ST7565), Zeichenausgabe, Zeichentabelle
- Rahmenformat und Serviceroutinen der Kommunikation vervollständigen
- Sensorwerte am Display und am PC sichtbar machen
- DL-Tests mit Mock-HAL

</details>

<details>
<summary><strong>8–9: FreeRTOS und Fahralgorithmus</strong></summary>

### 8. [FreeRTOS und Scheduling](Einheit_08_FreeRTOS/README.md)
- Gesamte bisherige Testsuite fertigstellen
- FreeRTOS einbinden, Task-Pipeline Fetch → Process → Decide
- Task-Timing und Latenz über die Telemetrie verifizieren
- Neue Testkategorie: Ablauf- und Scheduling-Tests

### 9. [Fahralgorithmus](Einheit_09_Fahralgorithmus/README.md)
- Zustandsautomat: Links / Mitte / Rechts
- Regler für Lenkung und Geschwindigkeit
- Host-Tests mit bereitgestelltem Mocking, Verifikation im CrazyCar-Simulator
- Nachweis am realen Fahrzeug

</details>

<details>
<summary><strong>Freies Fahren</strong></summary>

- Feinabstimmung der Regelung am realen Fahrzeug
- Systemintegration und individuelle Erweiterung
- Vorbereitung auf das Crazy Car Event
- Vollständige Testsuite bleibt verpflichtend

</details>

---

## Ziele

- Modularisierung der Embedded Software (Layerstruktur)
- Verständnis für low-level Hardwareansteuerung
- Messtechnischer Nachweis der eigenen Konfiguration
- Schichtweises Testen mit Unity und Mocking
- Entwicklung von Steuerungs- und Regelalgorithmen
- Einsatz eines Echtzeitbetriebssystems
- Umsetzung eines lauffähigen autonomen Systems auf Mikrocontroller-Basis

---

## Projektstruktur

Die Projektstruktur folgt dem klassischen Layer-Prinzip:

- HAL          – Hardware Abstraction Layer (Registerzugriff, ISR)
- DL           – Driver Layer (Komponentensteuerung)
- AL           – Application Layer (Tasks, Zustandsautomat, Regelung)
- extLib       – eingebundene Bibliotheken (FreeRTOS, VL53L1X)
- test         – Unity-Tests und MockHAL
- main.c       – Einstiegspunkt, Systeminitialisierung

Das Projekt wächst über die gesamte Laborreihe hinweg. Es wird nicht pro Einheit neu angelegt.

---

## Verwendete Tools

- Mikrocontroller: MSP430F5335 (Texas Instruments)
- Entwicklungsumgebung: Code Composer Studio (TI)
- Debugger: Spy-Bi-Wire / JTAG
- Messtechnik: Oszilloskop, Digilent Analog Discovery mit WaveForms
- Testframework: Unity (Host-Tests mit MockHAL)
- Echtzeitbetriebssystem: FreeRTOS (ab Einheit 8)
- Simulation: CrazyCar-Simulator (ab Einheit 9)
- Dokumentation: TI User Guide, Schaltpläne, Datenblätter
- Versionsverwaltung (optional empfohlen): GitLab, GitHub, Git

---

## Voraussetzungen

- Grundkenntnisse in C (Bitmasken, Pointer, Headerstrukturen)
- Verständnis für Mikrocontroller-Peripherie
- Umgang mit Code Composer Studio und Debugging-Werkzeugen

---

## Git / Versionierung (Sollte ihnen bereits geläufig sein)

Es wird empfohlen, das Projekt versionsverwaltet in einem GitLab- oder GitHub-Repository zu entwickeln. Ein typischer Initialisierungsvorgang:

```bash
git init
git remote add origin https://gitlab.com/<benutzer>/<projekt>.git
git add .
git commit -m "Initial commit"
git push -u origin master
```

Das vorhandene `.gitignore` im Hauptverzeichnis kann ihrem Projektordner hinzugefügt werden und ist bereits auf die durchgehend generierten Metadaten des Code Composers abgestimmt.