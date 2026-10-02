[⬅ Zurück zur Kapitelübersicht](../README.md#kapitelübersicht--aufgabenstellungen)

# Crazy Car Platine – Einführung, I/O-Port Konfiguration

## Inhalt

- [Crazy Car Platine](#crazy-car-platine)
- [Softwarestruktur](#softwarestruktur)
- [Code Composer Studio](#code-composer-studio)
- [IO Port Konfiguration](#io-port-konfiguration)
### Durchzuführende Aufgaben
- [[AUFGABE] Anlegen eines Basis Projektes](#aufgabe-anlegen-eines-basis-projektes)
- [[AUFGABE] Grundkonfiguration der GPIOs](#aufgabe-grundkonfiguration-der-gpio)

### Unterlagen
- [Family Guide - Digital I/O Ports Chapter 12](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=408)
- [Crazy Car Schematic](https://fhjoanneum-my.sharepoint.com/:b:/g/personal/florian_mayer_fh-joanneum_at/EfXYu-rqsLRErJbybsbN4AEB_RUMizJhwpb5D_ysimZehA?e=Ti7PtO)

### Grundlegendes - Videos
- [Code Composer Basics - Projects](https://youtu.be/DIkQaNBwspo?si=LmN9qGZYQwnxywni)
- [Code Composer Basics - Debugger](https://www.youtube.com/watch?v=BvRv3tO5Kd4&t=1s)
- [Digilent Scope / Waveforms](https://youtu.be/eUOFmPMJk18?si=pilhvG0IRg6HJkWf)

## Crazy Car Platine

- MSP430F5335
- 3 analoge Eingänge für Abstandssensoren
- 1 Hall-Effekt-Sensor für Drehzahl/Drehrichtungsmessung der Hinterachse
- MPU-9250: 9-Achsen-Sensor (Beschleunigung, Gyroskop, Kompass)
- Start/Stop-Taste
- Ausgänge für Servo und Fahrtenregler
- LC-Display
- JTAG Spy-bi-wire Interface

<p align="center">
  <img src="./media/CCPlatine2.png" alt="Include Options">
</p>
---

## Softwarestruktur

Die Software wird in Form eines klaren Layermodells entwickelt, das eine saubere Trennung zwischen Hardwarezugriff, Funktionseinheiten und Anwendungslogik sicherstellt. Dieses Schichtenmodell erhöht die Lesbarkeit, Wartbarkeit und Wiederverwendbarkeit der Software – insbesondere bei komplexen eingebetteten Systemen wie dem Crazy Car. 

Jede Schicht kommuniziert ausschließlich mit der direkt darunterliegenden Schicht. Direkte Zugriffe über mehrere Ebenen hinweg sind nicht zulässig. Dadurch entsteht eine stabile und erweiterbare Architektur, die unabhängig von konkreten Hardwaredetails bleibt und gleichzeitig die systematische Entwicklung unterstützt.

**Vorteile dieser Layerstruktur:** 

- Modularität: Jede Schicht kann unabhängig entwickelt oder ausgetauscht werden
- Wartbarkeit: Fehler lassen sich klar eingrenzen und beheben
- Wiederverwendbarkeit: HAL und DL-Komponenten lassen sich in anderen Projekten nutzen
- Abstraktion: Die Anwendung (AL) ist unabhängig von Hardwaredetails
- Testbarkeit: Funktionen können schichtweise getestet werden
<p align="center">
  <img src="./media/LayerStruct.png" alt="Include Options">
</p>

### HAL – Hardware Abstraction Layer (Registerebene)

Die HAL-Schicht kommuniziert direkt mit der Hardware, indem sie Mikrocontroller-Register setzt oder liest. Sie bildet die unterste Schicht und kapselt alle Zugriffe auf Peripherie-Register.

Typische Aufgaben im HAL:

- Konfiguration der Portrichtung über `PxDIR`
- Lesen von Eingangspegeln über `PxIN`
- Setzen/Rücksetzen von Ausgängen über `PxOUT`
- Aktivierung interner Pull-Ups/-Downs über `PxREN` in Verbindung mit `PxOUT`
- Einstellen der Treiberstärke über `PxDS`
- Auswahl alternativer Funktionen über `PxSEL`
- Zuordnung mappbarer Peripherie zu einem Pin über den Port-Mapping-Controller (`PxMAP`)
- Flankenauswahl und Freigabe der Port-Interrupts über `PxIES`, `PxIE`, `PxIFG`
- Initialisierung von Timer, SPI, I2C, UART, ADC und DMA
- Bereitstellen der Interrupt-Service-Routinen und Weitergabe der Ereignisse an die höheren Schichten

#### Beispiel: GPIO auf High setzen 
Funktionen im HAL sind für die mögliche Verwendung innerhalb des selben Layers, sowie zur multiplen Verwendung innerhalb der anderen übergeordneten Layer gedacht. 

```c
// Setze Bit 3 (z. B. als Ausgang):
P1DIR |= RPM_SENSOR;
// Lösche Bit 3 (z. B. als Eingang):
P1DIR &= ~RPM_SENSOR;
```

#### Beispiel: Makros für LCD-Backlight
Diese Art von Makros sind für die Verwendung innerhalb des selben Layers gedacht. Wenn es nun vorkommt, dass ein übergeordneter Layer (in unserem Fall der Driver Layer) darauf zugreifen soll, würde sich ein Funktionsaufruf dafür besser eignen. 

```c
#define LCD_BL             BIT2
#define LCD_BACKLIGHT_ON   (P8OUT |= LCD_BL)
#define LCD_BACKLIGHT_OFF  (P8OUT &= ~LCD_BL)
```

### DL – Driver Layer

Der DL-Layer abstrahiert die HAL-Funktionen weiter, um komplexe Komponenten wie Sensoren oder Displays auf höherer Ebene nutzbar zu machen. Registerzugriffe sind hier nicht mehr erlaubt – es wird ausschließlich über HAL-Funktionen gearbeitet.

```c
void dlDisplayInit(void);
void dlDisplayWriteText(const char* str, uint8_t len);
```

### AL – Application Layer

Im AL-Layer liegt die Applikationslogik: Fahrverhalten, Steuerstrategien, Reaktionen auf Sensoren usw. Hier werden ausschließlich die abstrahierten dl_-Funktionen genutzt.

```c
if (dist_cm < 20) {
    dlDisplayWriteText("STOP", 4);
    dlSetSteering(0);
}
```

---
## Code Composer Studio

Um das besprochene Layermodell umzusetzen (siehe Abbildung unten), müssen die entsprechenden Ordner angelegt werden.

> Rechtsklick auf das Projekt → New → Folder

- Ordner: `HAL`, `DL`, `AL` oder `Hardware`, `Driver`, `Application`

<p align="center">
  <img src="./media/LayerStructure.png" alt="Include Options">
</p>

In jedem Ordner liegen die jeweiligen Dateien des Layers. Eindeutige Namenskonventionen z. B.:

```c
halUsciB1.c / halUsciB1.h
```

Jede C-Datei besitzt eine passende Header-Datei mit Definitionen und Funktionsprototypen. Bestehende C-Dateien können direkt in den Projektordner kopiert werden. Anschließend erscheinen sie im Project Explorer.

Wird z. B. im `main.c` die Datei `halUsciB1.h` inkludiert, lautet der Pfad:

```c
#include "HAL/halUsciB1.h"
```

Um dies zu vereinfachen, empfiehlt es sich, die Include-Pfade in den Projekteinstellungen hinzuzufügen:

> Rechtsklick auf Projekt → Properties → Build → MSP430 Compiler → Include Options

<p align="center">
  <img src="./media/wholeBoard.png" alt="">
</p>

Danach kann der Aufruf so erfolgen:
```c
#include "halUsciB1.h"
```

### Debugger (In-Circuit Emulator)

Nach Klick auf das Symbol „Download and Debug“ wird das Projekt kompiliert, gelinkt und auf den Mikrocontroller geladen. Anschließend lässt sich das Programm auf der Hardware ausführen und debuggen.

Der **Register-Viewer** erlaubt das Live-Ändern und Einsehen von Registerinhalten. Auch Variablen können in den Tabs „Variables“ oder „Expressions“ überwacht werden.

<p align="center">
  <img src="./media/image7.png" alt="Include Options">
</p>

⚠️ *Hinweis*: Einige Hardwaremodule (z. B. Timer) laufen weiter, auch wenn das Programm angehalten ist. Diese Option lässt sich unter `Tools → Debugger Options → MSP430 Debugger Options → Clock Control` ändern.

---

## Code Composer Studio

- CCS kompiliert, linkt und programmiert das System über JTAG (Spy-bi-wire)
- Projekte liegen in einem Workspace (lokal empfohlen)
- Jedes Laborprojekt sollte als separates CCS-Projekt angelegt werden

### Projekt erstellen

1. File → New → CCS Project
2. Project Name + Target Device auswählen
3. Optimizer deaktivieren:
   - Properties → Build → MSP430 Compiler → Optimization: "off"
4. Include-Pfade setzen:
   - Properties → Build → MSP430 Compiler → Include Options → "+" → z. B. `/HAL` oder `/DL`

<p align="center">
  <img src="./media/FolderStructure.png" alt="Include Options">
</p>

### Debugger

- Download & Debug: Programmieren und Ausführen direkt auf der Hardware
- Register-Viewer: Registerinhalte live betrachten und ändern
- Breakpoints, Expressions, Variables: Live-Debugging

<p align="center">
  <img src="./media/image7.png" alt="Include Options">
</p>

---
## [AUFGABE] Anlegen eines Basis Projektes

### Schritte

1. Legen Sie ein neues Projekt für den verwendeten Mikrokontroller an, z. B. `Laboruebung_1` oder `LAB_1`.
2. Erstellen Sie die Layer-Ordner `HAL`, `DL`, `AL` und fügen Sie die Include-Pfade - wie oben beschrieben - hinzu.
3. Schalten Sie den Optimizer aus.
4. Fügen Sie eine Endlosschleife in die `main`-Funktion ein und tauschen Sie `int` gegen `void` aus.
5. Builden Sie das Programm.
6. Kopieren Sie die Dateien `halPmm.c` und `halPmm.h` in die Projektordner-Struktur im Windows Explorer.  
   Die Dateien erscheinen anschließend automatisch im *Project Explorer*.
7. Erstellen Sie eine neue C- und H-Datei: `halGeneral.c`, `halGeneral.h`.  
   - Vergessen Sie nicht, in der Headerdatei die `#ifndef`-Abfrage anzugeben.  
   - Inkludieren Sie die Headerdatei in der C-Datei.
8. Erstellen Sie in der `halGeneral.c` eine Funktion `halInit` ohne Übergabeparameter.  
   Schreiben Sie den Funktionsprototypen in die `halGeneral.h`.
9. Inkludieren Sie die `halPmm.h` in der `halGeneral.c`.
10. Rufen Sie in der Funktion `halInit` die Funktion `HAL_PMM_Init()` auf. (Nomenklatur im File bereits gegeben)
11. Inkludieren Sie die `halGeneral.h` in der `main.c` und rufen Sie in der `main`-Funktion die Funktion `halInit()` auf.
12. Erstellen Sie für die Konfiguration des Watchdog-Timers ein eigenes Modul (`halWdt.c`, `halWdt.h`) und implementieren Sie eine Funktion `halWdtInit()`.  

    - Diese wird ebenfalls in der `halInit`-Funktion aufgerufen, **bevor** das PMM-Modul initialisiert wird.


## IO Port Konfiguration

Die Konfiguration der I/O-Ports erfolgt durch gezielte Setzung der entsprechenden Steuerregister für Richtung, Ausgang, Peripheriemodul-Zuweisung und Pull-Up/-Down-Konfiguration. Der MSP430 stellt hierfür unter anderem folgende Register pro Port zur Verfügung:

- `PxDIR`: Richtung (0 = Eingang, 1 = Ausgang)
- `PxOUT`: Ausgangswert
- `PxIN`: Eingelesener Wert
- `PxSEL`, `PxSEL2`: Peripheriemodul-Zuweisung
- `PxREN`: Pull-Up/-Down-Enable

### Aufbau und Schaltbild

Die folgende schematische Darstellung (aus dem MSP430 User Guide) zeigt den internen Aufbau der Pad-Logik für Port P1.0–P1.7 [[P1]](https://www.ti.com/lit/ds/symlink/msp430f5335.pdf#page=80):

<p align="center">
  <img src="./media/PortOne.png" alt="Include Options">
</p>

Jede dieser Leitungen (z. B. P1.3 = TA0.2) kann als GPIO oder als Peripherieausgang (z. B. Timer Output) verwendet werden. Dies wird durch die Registereinstellungen gesteuert. Ein Beispiel aus der Crazy Car Plattform zeigt die Belegung von Port 1:

<p align="center">
  <img src="./media/PortOneCC.png" alt="Include Options">
</p>

### Beispielhafte Definition und Konfiguration

```c
// ##### Port 1 #####
#define RPM_SENSOR       BIT3
#define RPM_SENSOR_DIR   BIT4  
#define I2C_INT_MOTION   BIT5 
#define START_BUTTON     BIT6  
#define STOP_BUTTON      BIT7  

// Setze RPM_SENSOR als Eingang
P1DIR &= ~RPM_SENSOR;
```

#### Bitweise Operationen

Um gezielt einzelne Bits in einem Register zu setzen oder zu löschen, ohne die anderen zu beeinflussen:

```c
// Setze Bit 3 (z. B. als Ausgang):
P1DIR |= RPM_SENSOR;

// Lösche Bit 3 (z. B. als Eingang):
P1DIR &= ~RPM_SENSOR;
```

```c
// RPM_SENSOR entspricht BIN: 00001000
// ~RPM_SENSOR ergibt BIN:    11110111
```

Dadurch werden gezielt einzelne Pins geändert, ohne andere Bits im Register zu beeinflussen.

### Makros und Lesbarkeit

Zur besseren Lesbarkeit und Wartbarkeit empfiehlt es sich,entsprechende Makros zu verwenden:

```c
#define LCD_BL             BIT2
#define LCD_BACKLIGHT_ON   (P8OUT |= LCD_BL)
#define LCD_BACKLIGHT_OFF  (P8OUT &= ~LCD_BL)
```

### Umgang mit unbenutzten Pins

Nicht genutzte Pins sollten definiert konfiguriert werden (z. B. als Ausgang mit definierter Pegelvorgabe), um Fehlverhalten durch Floating-Eingänge zu vermeiden. Andernfalls können unerwartete Stromverbräuche oder EMV-Probleme entstehen.

---

## [AUFGABE] Grundkonfiguration der GPIO

In dieser Aufgabe geht es darum, die Beschaltung der Crazy-Car-Platine – wie sie im Schaltplan ersichtlich ist – systematisch im Code abzubilden. Alle relevanten Pins sollen im Rahmen eines hal_-GPIO-Moduls konfiguriert werden. Dabei werden die Pins je nach Funktion als Ein- oder Ausgang initialisiert. Die Namen und Registerkonfigurationen sollen so gewählt werden, dass eine klare Zuordnung zwischen physischer Schaltung und Software möglich ist. Ziel ist eine robuste, nachvollziehbare Pininitialisierung im HAL.

### Aufgaben:

1. Modul `halGpio.c/.h` erstellen und in der Projektstruktur korrekt ablegen
2. In `halGpio.h` alle verwendeten Pins als `#define` Makros mit sprechenden Namen deklarieren (z. B. `RPM_SENSOR`, `START_BUTTON`)
3. Funktion `halGpioInit()` in `halGpio.c` implementieren
4. Innerhalb von `halGpioInit()`:
   - Richtung aller beschalteten Pins gemäß ihrer Funktion setzen (`PxDIR`)
   - Optional: Ausgänge initial mit definiertem Pegel belegen (`PxOUT`)
   - Optional: Pull-Ups/-Downs für Eingänge aktivieren (`PxREN`, `PxOUT`)
5. Unbenutzte Pins als digitale Ausgänge mit Low-Pegel konfigurieren (Floating vermeiden)
6. `halGpioInit()` in `halInit()` aufrufen
7. **Unbedingt** Funktionalität im Debugger überprüfen (z. B. Lesbarkeit der Inputs, korrekte OUT-Zustände)
8. [Meilensteinüberprüfung] Was passiert mit unbeschaltenen PINs ?

## Referenzen

- **MSP430x5xx and MSP430x6xx Family User Guide**, Texas Instruments, Literature Number: SLAU208O, Rev. O, April 2019.  
  Verfügbar unter: [https://www.ti.com/lit/pdf/slau208](https://www.ti.com/lit/pdf/slau208)

- **MSP430F5335 Datasheet**, Texas Instruments, Document Number: SLAS590N, Rev. N, October 2018.  
  Verfügbar unter: [https://www.ti.com/lit/gpn/msp430f5335](https://www.ti.com/lit/gpn/msp430f5335)

- John H. Davies, **MSP430 Microcontroller Basics**, Newnes/Elsevier, ISBN 978‑0‑7506‑8276‑3.  

[⬆ Zurück zum Hauptverzeichnis](../README.md#kapitelübersicht--aufgabenstellungen)
