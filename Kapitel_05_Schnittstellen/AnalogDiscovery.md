[⬅ Zurück zur Aufgabenstellung](README.md)

# Messen mit dem Analog Discovery

## Inhalt
- [Die beiden Instrumente](#die-beiden-instrumente)
- [Anschluss](#anschluss)
- [Pinbelegung](#pinbelegung)
- [Logic einrichten](#logic-einrichten)
- [Dekoder-Einstellungen](#dekoder-einstellungen)
- [Protocol als Gegenstelle](#protocol-als-gegenstelle)
- [Aufzeichnen und dokumentieren](#aufzeichnen-und-dokumentieren)
- [Wenn nichts zu sehen ist](#wenn-nichts-zu-sehen-ist)

**Dokumentation**
- WaveForms Reference Manual: [Logic Analyzer](https://files.digilent.com/manuals/WaveForms/3.22.1/logic.html) · [Protocol](https://files.digilent.com/manuals/WaveForms/3.22.1/protocol.html)
- Analog Discovery 3 [Referenzhandbuch](https://digilent.com/reference/test-and-measurement/analog-discovery-3/reference-manual)

Die Handbuchseiten sind versioniert. Die zu Ihrer Installation passende Fassung erreichen Sie in WaveForms über *Help → Reference Manual*.

---

## Die beiden Instrumente

Für die Schnittstellenmessung werden zwei Instrumente gebraucht, und es ist wichtig, sie auseinanderzuhalten:

| Instrument | Rolle | Was es zeigt |
|---|---|---|
| **Logic** | hört nur mit | Signalverlauf über der Zeit, dekodiert als Bytes |
| **Protocol** | wirkt als Teilnehmer | antwortet, sendet, bestätigt – und protokolliert mit |

**Logic** ist ein Logikanalysator: Er zeichnet Pegel auf und legt darüber einen *Interpreter*, der aus den Flanken Bytes macht. Er greift nicht in den Bus ein.

**Protocol** ist das Gegenstück: Hier wird das Analog Discovery selbst zum SPI-Slave, zum I2C-Slave oder zur UART-Gegenstelle. Ohne dieses Instrument lässt sich kein Empfangspfad prüfen – und beim I2C überhaupt keine vollständige Übertragung erzeugen, weil ohne antwortenden Teilnehmer jeder Zugriff mit einem NACK endet.

> **Nicht gleichzeitig.** Protocol benutzt intern denselben Logikanalysator und Pattern-Generator. Läuft das Logic-Fenster parallel, streiten sich die beiden um die Pins. Schließen Sie es, und nutzen Sie zum Mitlesen den **Spy**-Modus im Protocol-Instrument – er dekodiert dasselbe.

---

## Anschluss

**1. Masse zuerst.** Verbinden Sie ⏚ (schwarz, beim Flachbandkabel die Leitungen mit weißem Rand) mit der Fahrzeugmasse. Ohne gemeinsamen Bezugspunkt sind alle dekodierten Daten Zufall – das ist der häufigste Grund für scheinbar defekte Schnittstellen.

**2. Logiksignale.** Die digitalen Eingänge heißen **DIO 0 bis 15**. Sie sind für 3,3-V-Logik ausgelegt und passen damit direkt zum MSP430. Legen Sie pro Signal eine Leitung auf einen DIO-Pin und notieren Sie die Zuordnung – ohne Notiz ist nach dem dritten Umstecken nicht mehr klar, welcher Kanal was zeigt.

**3. Pull-Ups beim I2C.** Das Analog Discovery hat **keine** zuschaltbaren Pull-Ups; SDA und SCL brauchen externe Widerstände (4,7 kΩ für 100 kHz). Prüfen Sie zuerst im [Schaltplan](https://fhjoanneum-my.sharepoint.com/:b:/g/personal/florian_mayer_fh-joanneum_at/EfXYu-rqsLRErJbybsbN4AEB_RUMizJhwpb5D_ysimZehA?e=Ti7PtO), ob die Platine welche bestückt hat. Fehlen sie, bleiben beide Leitungen auf undefiniertem Pegel und der Bus meldet sich als dauerhaft belegt.

> **VORSICHT:** Die Aufschrift **MOSI und CLK am Displayboard sind vertauscht.** Gehen Sie beim Anschließen nach dem Schaltplan, nicht nach dem Aufdruck.

---

## Pinbelegung

Tragen Sie Ihre Zuordnung ein. Die Controllerpins der vorgegebenen Module stehen fest, die SPI-Pins ergeben sich aus Ihrer eigenen GPIO-Konfiguration:

| Signal | Pin am MSP430 | DIO |
|---|---|---|
| SPI CLK | aus Ihrer GPIO-Konfiguration | |
| SPI SIMO (MOSI) | " | |
| SPI SOMI (MISO) | " | |
| SPI CS | GPIO, Makro aus `halGPIO.h` | |
| Debug-UART TXD | P2.1 | |
| Debug-UART RXD | P2.0 | |
| AUX-UART | P8.2 / P8.3 | |
| I2C SCL | P2.7 | |
| I2C SDA | P2.6 | |

Beim AUX-UART legen Sie **beide** Pins auf je einen DIO-Kanal. Welcher der beiden TXD ist, zeigt die Messung selbst: Dort entsteht beim Senden Aktivität, die andere Leitung bleibt im Ruhepegel. Das ist schneller als die Suche in der Port-Funktionstabelle des Datenblatts.

---

## Logic einrichten

1. **Instrument öffnen** – im WaveForms-Hauptfenster *Logic*.

2. **Signale hinzufügen** – im Signalraster über **+ (Add)**. Einzelne Pegel kommen als *Signal* dazu, die Protokoll-Dekoder unter **Add → Protocol** als eigene Zeile: *SPI*, *I2C* oder *UART*.

   Legen Sie zusätzlich die Rohsignale als einzelne Kanäle an. Der Dekoder zeigt Bytes, aber nicht, warum er sie so gelesen hat; dafür brauchen Sie die Flanken.

3. **Dekoder konfigurieren** – siehe Tabelle unten. Die Einstellungen müssen zu Ihrer Konfiguration im Controller passen; ein Dekoder mit falscher Taktphase oder falscher Baudrate liefert Bytes, die plausibel aussehen und falsch sind.

4. **Abtastrate und Puffer** – oben links *Rate* und *Samples*. Richtwert: mindestens **zehnfache Bitrate**. Für SPI mit 100 kHz und für 57600 Baud sind 10 MS/s bequem ausreichend. Zu niedrig abgetastet, verschiebt sich die erkannte Flanke und der Dekoder verliert Bits.

5. **Trigger setzen** – ohne Trigger läuft das Fenster über und zeigt irgendeinen Ausschnitt. Brauchbare Bedingungen:

   | Schnittstelle | Trigger |
   |---|---|
   | SPI | fallende Flanke auf CS – damit beginnt das Bild am Anfang der Übertragung |
   | UART | fallende Flanke auf TXD – das Startbit |
   | I2C | Startbedingung, über den Trigger-Assistenten des I2C-Dekoders |

6. **Aufzeichnen** – **Single** für eine einzelne Aufzeichnung, **Run** für fortlaufende. Für die Dokumentation ist *Single* das Richtige: ein stehendes Bild, das sich vermessen lässt.

---

## Dekoder-Einstellungen

| | Einstellung | Wert für diese Übung |
|---|---|---|
| **SPI** | Select | CS-Kanal, aktiv **Low** |
| | Clock | Abtastflanke passend zu CPOL/CPHA |
| | Data | SIMO bzw. SOMI, **MSB first** |
| | Bits per word | 8 |
| **UART** | Data | TXD-Kanal |
| | Bits | 8, Parity **None**, Stop **1** |
| | Rate | 57600 (Debug) bzw. 9600 (AUX) |
| **I2C** | Clock / Data | SCL- und SDA-Kanal |
| | Address | **7 Bit** |

Die Adressdarstellung beim I2C ist die klassische Falle: Stellt der Dekoder 8 Bit *mit* R/W-Bit dar, erscheint Ihre Adresse um den Faktor zwei verschoben, obwohl die Übertragung korrekt ist. Vergleichen Sie im Zweifel mit dem Wert, den Sie in `UCB0I2CSA` geschrieben haben.

Der SPI-Dekoder zeigt in der Grundform eine Datenleitung. Für SIMO und SOMI gleichzeitig gibt es eine eigene Variante – sinnvoll, sobald Sie den Empfangspfad prüfen.

---

## Protocol als Gegenstelle

Instrument *Protocol* öffnen, oben den Reiter der Schnittstelle wählen. Jeder Reiter hat Unterbetriebsarten; gebraucht werden:

| Schnittstelle | Betriebsart | Was damit geht |
|---|---|---|
| **SPI** | **Slave** | antwortet mit den Bytes, die Sie als MISO-Daten eintragen |
| | *Spy* | dekodiert mit, wie Logic |
| **I2C** | **Slave** | horcht auf eine frei gewählte Adresse, quittiert und liefert Read-Daten |
| | *Master* | eigene Lese-/Schreibzugriffe auf Ihren Bus |
| **UART** | **Send & Receive** | sendet auf TX, empfängt auf RX |
| | *Spy* | dekodiert TX und RX mit |

Beim UART gibt es keine Master-/Slave-Rolle – deshalb spricht die Aufgabenstellung dort von *Empfänger* und bei den anderen beiden von *Slave*.

**Vor dem ersten Senden** die Grundeinstellungen des Reiters prüfen: Pinzuordnung, Frequenz bzw. Baudrate, Polarität, Bitreihenfolge. Diese Felder sind dieselben wie beim Dekoder – nur wirken sie hier nicht auf die Auswertung, sondern auf das, was das Gerät tatsächlich treibt.

**Beim I2C-Slave** zusätzlich: Adresse (7 Bit), Anzahl der zu quittierenden Schreibbytes, und die Bytes, die bei einem Lesezugriff zurückgehen. Erst damit entsteht eine vollständige Übertragung mit ACK.

---

## Aufzeichnen und dokumentieren

**Bitzeit messen.** Cursor im Logic-Fenster auf zwei aufeinanderfolgende Flanken setzen; die Anzeige liefert den Abstand. Bei 57600 Baud sind rund 17,4 µs zu erwarten, bei 9600 Baud rund 104 µs. Weicht der gemessene Wert ab, liegt es am Baudratenteiler – nicht am Messgerät.

**Taktfrequenz messen.** Beim SPI genügt der Abstand zweier Taktflanken; der Kehrwert der Periode ist die Frequenz. Vergleichen Sie mit dem `#define` im Header.

**Exportieren.** *File → Export* speichert das Bild als PNG oder die Rohwerte als CSV. Für das Protokoll genügen die PNGs – mit sichtbarer Dekoderzeile, sonst ist nicht nachvollziehbar, was gemessen wurde.

Benennen Sie die Dateien so, dass die Einstellung mitgeht, z. B. `uart_aux_9600.png`. Vier Screenshots mit gleichem Namen sind im Protokoll nicht mehr auseinanderzuhalten.

---

## Wenn nichts zu sehen ist

| Symptom | Ursache |
|---|---|
| alle Kanäle flach | Masse nicht verbunden, oder Controller im Reset |
| Flanken sichtbar, Dekoder leer | falsche Baudrate, falsche Taktphase, oder Dekoder auf dem falschen Kanal |
| Bytes plausibel, aber falsch | CPOL/CPHA vertauscht, oder Bitreihenfolge auf LSB first |
| I2C-Adresse um Faktor 2 daneben | Dekoder auf 8-Bit-Adressdarstellung |
| I2C dauerhaft belegt | Pull-Ups fehlen |
| Bild zeigt irgendeinen Ausschnitt | kein Trigger gesetzt |
| Bits fehlen, Timing springt | Abtastrate zu niedrig |
| Protocol reagiert nicht | Logic-Fenster läuft parallel |

---

[⬆ Zurück zur Aufgabenstellung](README.md)
