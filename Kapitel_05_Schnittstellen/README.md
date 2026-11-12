[⬅ Zurück zur Kapitelübersicht](../README.md#kapitelübersicht--aufgabenstellungen)
# Schnittstellen: SPI, UART, I2C

## Inhalt
- [Die Unterkapitel](#die-unterkapitel)
- [Registerzugriffe gehören in den HAL](#registerzugriffe-gehören-in-den-hal)
- [Pins und Port-Mapping](#pins-und-port-mapping)
- [SPI selbst implementieren](#spi-selbst-implementieren)
- [UART und I2C einbinden](#uart-und-i2c-einbinden)
- [Verifikation mit dem Analog Discovery](#verifikation-mit-dem-analog-discovery)
- [Testing](#testing)

**Laborübung**

- *MSP430x5xx and MSP430x6xx Family User Guide Rev. O* – Texas Instruments
  - Kapitel 36: [USCI – UART Mode](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=938)
  - Kapitel 37: [USCI – SPI Mode](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=968)
  - Kapitel 38: [USCI – I2C Mode](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=994)

- Crazy Car Controller FHJ Schaltplan [Crazy Car Schematic](https://fhjoanneum-my.sharepoint.com/:b:/g/personal/florian_mayer_fh-joanneum_at/EfXYu-rqsLRErJbybsbN4AEB_RUMizJhwpb5D_ysimZehA?e=Ti7PtO)
- Digilent **Analog Discovery** mit **WaveForms** – [Referenzhandbuch](https://digilent.com/reference/test-and-measurement/analog-discovery-3/reference-manual)

**Wissensüberprüfung**

- Recherche:
  - Worin unterscheiden sich synchrone und asynchrone Übertragung?
  - Woran erkennt ein UART-Empfänger den Anfang eines Zeichens, obwohl keine Taktleitung mitläuft?
  - Warum braucht ein I2C-Bus Pull-Up-Widerstände, ein SPI-Bus aber nicht?
  - Wozu dient der Port-Mapping-Controller, und warum ist der Zugriff auf seine Register geschützt?
- Arbeiten Sie die vier Unterkapitel durch – siehe unten.

> **VORSICHT:** Die Aufschrift MOSI und CLK am Displayboard sind vertauscht.

**Video**
 - [Einführungsvideo - Einheit 5](https://youtu.be/q_kA9L3taqE?si=FuuGbJwFVF0v3IEW)

### Durchzuführende Aufgaben
- [[AUFGABE] Pins finden und in `halGPIOInit()` setzen](#aufgabe-pins-finden-und-in-halgpioinit-setzen)
- [[AUFGABE] USCI B1 – SPI implementieren](#aufgabe-usci-b1--spi-implementieren)
- [[AUFGABE] UART und I2C in Betrieb nehmen](#aufgabe-uart-und-i2c-in-betrieb-nehmen)
- [[AUFGABE] Funktionsdiagramm der vorgegebenen Module](#aufgabe-funktionsdiagramm-der-vorgegebenen-module)
- [[AUFGABE] Messen am Analog Discovery](#aufgabe-messen-am-analog-discovery)

---

In dieser Einheit werden alle drei seriellen Schnittstellen des Fahrzeugs in Betrieb genommen. **SPI** implementieren Sie selbst; es ist die überschaubarste der drei und wird in der nächsten Einheit für das Display gebraucht. **UART** und **I2C** werden als fertige Module bereitgestellt; dort liegt die Arbeit darin, den fremden Code zu verstehen und sein Verhalten zu dokumentieren.

| Schnittstelle | Modul | Verwendung | Ihre Aufgabe |
|---|---|---|---|
| SPI | `halUSCIB1` | LC-Display | selbst implementieren |
| UART | `halUSCIA0` | Debug-Schnittstelle zum PC über den Debugger | einbinden, Funktionsdiagramm |
| I2C | `halUSCIB0` | ToF-Abstandssensoren | einbinden, Funktionsdiagramm |

Alle drei Module werden am Ende mit dem Analog Discovery gemessen und dekodiert; das Gerät dient dabei zugleich als Gegenstelle.

---

## Die Unterkapitel

Die Grundlagen stehen in vier eigenen Unterkapiteln. Die ersten drei behandeln jeweils zuerst das Protokoll und danach dessen Umsetzung im MSP430: welches USCI-Modul, welche Register, welche Besonderheiten. Das vierte beschreibt die Messung.

| | Inhalt |
|---|---|
| ➡ **[SPI am MSP430](SPI.md)** | Leitungen, CPOL/CPHA, Schieberegister, USCI_B1, Takt und Teiler |
| ➡ **[UART am MSP430](UART.md)** | Rahmenaufbau, Baudratenberechnung mit Modulator, USCI_A0, Rahmenerkennung |
| ➡ **[I2C am MSP430](I2C.md)** | Open Drain, Adressierung, ACK/NACK, Repeated Start, USCI_B0, Zustandsautomat |
| ➡ **[Messen mit dem Analog Discovery](AnalogDiscovery.md)** | Anschluss, Pinbelegung, Logic und Protocol, Dekoder-Einstellungen, Fehlersuche |

Arbeiten Sie die Unterkapitel vor der jeweiligen Aufgabe durch. Die Aufgabenstellungen unten setzen voraus, dass Sie die Register, den Ablauf und die Bedienung des Messgeräts kennen – sie wiederholen die Grundlagen nicht.

---

## Registerzugriffe gehören in den HAL

Bevor es an die Schnittstellen geht, eine Regel, die ab dieser Einheit scharf wird.

In Einheit 4 haben Sie den Driver Layer eingeführt. Die Begründung dort war die Lesbarkeit – `driverSetSteering(-100)` sagt mehr als `TA1CCR2 = 2750`. Bei den Schnittstellen kommt ein handfesterer Grund dazu: **Eine Übertragung ist ein Ablauf, kein Registerwert.**

Wer aus dem Driver Layer heraus direkt in `UCB1TXBUF` schreibt, umgeht den Zustand, den die ISR verwaltet. Das Ergebnis sind Fehler, die nur manchmal auftreten und sich kaum eingrenzen lassen: ein Byte, das mitten in eine laufende Übertragung geschrieben wird, ein CS-Signal, das zu früh wieder steigt, ein Flag, das niemand zurücksetzt.

**Die Regel lautet deshalb:**

| Schicht | darf | darf nicht |
|---|---|---|
| **HAL** | Register lesen und schreiben, ISRs bereitstellen, Status nach außen melden | – |
| **DL** | HAL-Funktionen aufrufen, Status abfragen | Register anfassen, `#include <msp430.h>` |
| **AL** | DL-Funktionen aufrufen | beides nicht |

Praktisch heißt das: Jede Schnittstelle bietet nach außen genau drei Dinge an – eine Funktion zum Initialisieren, eine zum Anstoßen einer Übertragung, und eine Abfrage, ob sie fertig ist. Was dazwischen passiert, bleibt im Modul.

```c
halUSCIB1Init();                        // einmal beim Start
halUSCIB1Send(data, len);               // Übertragung anstoßen
while (halUSCIB1Busy());                // auf das Ende warten
```

Dieselbe Dreiteilung finden Sie in den vorgegebenen Modulen wieder: `halUSCIA0Init()` / `halUSCIA0Send()` / `halUSCIA0TxBusy()` und `halUSCIB0Init()` / `halI2CWrite()` / `halI2CBusy()`. Das ist kein Zufall, sondern dasselbe Muster.

Ihr SPI-Modul verwaltet seinen Zustand in einer globalen Struktur. Die gehört dem Modul – nach außen sichtbar sind nur die drei Funktionen. Eine Zeile wie `LCD_Data.TxData.len = …` oder `LCD_Data.Status.B.TxSuc` im Driver Layer wäre ein Registerzugriff mit Zwischenschritt: Sie greift an dem Ablauf vorbei, den die ISR verwaltet.

> **Prüfen Sie Ihren Code am Ende der Einheit daraufhin:** Steht in einer Datei des Driver Layers irgendwo ein Registername oder ein `#include <msp430.h>`, gehört die betreffende Zeile in den HAL.

Ab Einheit 6 wird diese Trennung zur Voraussetzung: Die Gerätetreiber für Display und Sensoren bauen auf den drei Schnittstellen auf, und sie dürfen nicht wissen, an welchem USCI-Modul sie hängen.

---

## Pins und Port-Mapping

Keine Schnittstelle funktioniert, solange ihre Pins noch gewöhnliche I/O-Pins sind. Bevor es an die Module geht, klären Sie deshalb für alle drei: welcher Controllerpin trägt welches Signal, und wie kommt die Sonderfunktion darauf.

Zwei Quellen, die zusammenpassen müssen:

| Quelle | Antwortet auf |
|---|---|
| [Schaltplan](https://fhjoanneum-my.sharepoint.com/:b:/g/personal/florian_mayer_fh-joanneum_at/EfXYu-rqsLRErJbybsbN4AEB_RUMizJhwpb5D_ysimZehA?e=Ti7PtO) | Welcher Pin ist mit Display, Steckerleiste und Sensorbus verdrahtet? |
| Datenblatt MSP430F5335, Port-Funktionstabelle | Welche Sonderfunktion kann dieser Pin tragen, und ist sie über `PxSEL` direkt erreichbar oder nur über den Port-Mapping-Controller? |

Der Unterschied zwischen beiden Wegen ist der Grund, warum diese Aufgabe eigenständig ist: Bei einem Pin mit fester Funktion genügt ein gesetztes Bit in `PxSEL`. Bei einem gemappten Pin muss zusätzlich ein `PxMAPy`-Register beschrieben werden – und das gelingt nicht ohne Weiteres.

**`PMAPKEYID`.** Ein direkter Schreibzugriff auf die Mapping-Register bleibt wirkungslos. Dazwischen liegt ein Schlüsselregister, und wie es zu bedienen ist, finden Sie selbst heraus – im Family User Guide, Kapitel *Port Mapping Controller*. Drei Fragen sollten Sie danach beantworten können:

- Welcher Wert muss in `PMAPKEYID` geschrieben werden, und was passiert bei einem anderen?
- Wie lange bleibt der Zugriff offen, und wie schließt man ihn wieder?
- Was geschieht mit einem Schreibzugriff auf ein `PxMAPy`-Register, nachdem das Fenster wieder geschlossen ist?

Die letzte Frage ist die unangenehmste: Sie erklärt, warum ein zweiter Konfigurationsversuch im laufenden Programm stillschweigend ohne Wirkung bleiben kann.

### [AUFGABE] Pins finden und in `halGPIOInit()` setzen

1. **Pins ermitteln** und das Ergebnis in eine Tabelle eintragen:

   | Signal | Pin | Sonderfunktion über | Richtung |
   |---|---|---|---|
   | SPI CLK, SIMO, SOMI | | `PxSEL` oder Mapping | |
   | SPI CS | | gewöhnlicher I/O | Ausgang |
   | Debug-UART RXD, TXD | | | |
   | I2C SDA, SCL | | | |

   Diese Tabelle brauchen Sie später noch zweimal: beim Anschließen des Analog Discovery und bei der Fehlersuche.

2. **`halGPIOInit()` ergänzen.** Setzen Sie für jedes Signal das `PxSEL`-Bit und – wo nötig – die Richtung. Verwenden Sie die Bitmakros, keine Zahlenwerte.

3. **Port-Mapping dort, wo es nötig ist.** Das Gerüst sieht so aus – der Debug-UART ist als Beispiel eingetragen, der Rest ist Ihre Arbeit:

   ```c
   /* Auszug aus halGPIOInit() – Port-Mapping */

   // TODO: Schreibzugriff auf die PxMAP-Register freigeben.
   //       Welches Register, welcher Wert?  -> Family User Guide, Port Mapping Controller

   P2MAP0 = PM_UCA0RXD;        // Beispiel: P2.0 trägt RXD des Debug-UART
   P2MAP1 = PM_UCA0TXD;        // Beispiel: P2.1 trägt TXD des Debug-UART

   // TODO: dasselbe für den I2C – SDA und SCL auf UCB0 legen.
   //       Die passenden PM_...-Makros stehen in msp430f5335.h

   // TODO: Schreibzugriff wieder sperren

   // TODO: für jeden gemappten Pin zusätzlich das PxSEL-Bit setzen.
   //       Das Mapping allein schaltet die Sonderfunktion nicht ein.

   // Hinweis: Nicht jeder Pin braucht Mapping. Wo die Sonderfunktion fest
   //          auf dem Pin liegt, genuegt das PxSEL-Bit.
   ```

   Ersetzen Sie die TODO-Kommentare durch Code und schreiben Sie dazu, was Ihre Zugriffe auf `PMAPKEYID` bewirken – sonst weiß beim nächsten Lesen niemand mehr, warum die beiden Zeilen dort stehen.

   Das Mapping muss gesetzt sein, **bevor** das zugehörige USCI-Modul aus dem Reset kommt – also vor dem jeweiligen `halUSCIxInit()`.

4. **Chip Select** bleibt ein gewöhnlicher Ausgang und gehört nicht zum USCI-Modul. Legen Sie dafür ein Makro in `halGPIO.h` an, nach dem Muster der bestehenden Makros aus Einheit 2.

5. **Prüfen Sie das Ergebnis im Debugger**, bevor Sie weitermachen: Sehen Sie sich die `PxSEL`- und `PxDIR`-Register an. Ein falsch gesetzter Pin erzeugt später ein Fehlerbild, das wie ein Konfigurationsfehler im USCI-Modul aussieht – und dort suchen Sie dann vergeblich.

> **VORSICHT:** Die Aufschrift **MOSI und CLK am Displayboard sind vertauscht.** Gehen Sie nach dem Schaltplan, nicht nach dem Aufdruck.

---

## SPI selbst implementieren

Das LC-Display des Crazy Car Controllers wird über die SPI-Schnittstelle USCI_B1 angesteuert. Dazu müssen sowohl die I/O-Pins als auch das USCI-Modul korrekt initialisiert werden. Die Datenübertragung erfolgt interruptgesteuert, um die CPU zu entlasten.

> Grundlagen und Register: ➡ **[SPI am MSP430](SPI.md)**

### [AUFGABE] USCI B1 – SPI implementieren

1. **HAL-Modul anlegen:** `halUSCIB1.c` und `halUSCIB1.h`

2. **Funktion `halUSCIB1Init(void)` erstellen:**
   - SPI-Master-Mode
   - Taktfrequenz: 100 kHz (als `#define` im Header)
   - 8 Bit, MSB first
   - Taktpolarität und Taktphase: `UCCKPL` gesetzt, `UCCKPH` gelöscht – das Display erwartet Takt idle HIGH und Übernahme an der zweiten Flanke (SPI-Modus 3). Warum die Bits nicht heißen wie die Modusbezeichnung, steht im Unterkapitel.
   - RX-Interrupt aktivieren
   - Softwarereset vor/nach Konfiguration setzen (`UCB1CTL1`)
   - Aufruf in `halInit()`

3. **Datenstruktur definieren (in `halUSCIB1.h`):**

```c
typedef struct {
    union {
        unsigned char R;
        struct {
            unsigned char TxSuc:1;      // 1 = Übertragung abgeschlossen
            unsigned char dummy:7;
        } B;
    } Status;

    struct {
        unsigned char len;              // Anzahl zu übertragender Bytes
        unsigned char cnt;              // aktueller Index
        unsigned char Data[256];        // Tx-Array
    } TxData;

    struct {
        unsigned char len;              // Anzahl empfangener Bytes
        unsigned char Data;             // zuletzt empfangenes Byte
    } RxData;

} USCIB1_SPICom;
```

> **Hinweis:** Das Empfangsarray ist bewusst auf ein einzelnes Byte reduziert. Das Display sendet nichts zurück, und der Speicher wird in einer späteren Einheit für FreeRTOS gebraucht.

4. **Globale Variable vom Typ `USCIB1_SPICom` in `halGeneral.c` deklarieren** – sie heißt `LCD_Data`, `volatile` nicht vergessen, die ISR schreibt darauf.

5. **Als `extern volatile` in den Dateien deklarieren, die sie verwenden.**

6. **Übertragung anstoßen – zwei Funktionen, eine davon nach außen:**

   Modulintern (`static`), setzt die Übertragung in Gang:
   - Erstes Byte in `UCB1TXBUF` schreiben
   - `TxSuc` auf 0 setzen
   - Alle weiteren Bytes schreibt die ISR, bis `TxData.len` erreicht ist

   Nach außen:
   ```c
   unsigned char halUSCIB1Send(const unsigned char *data, unsigned char len);
   ```
   kopiert die Daten in `TxData`, setzt die Länge und ruft die interne Funktion auf. Rückgabe `0`, wenn bereits eine Übertragung läuft – dasselbe Muster wie `halUSCIA0Send()`.

   > **Warum kopieren und nicht nur den Zeiger merken?** Die anderen Module merken sich den Zeiger und verlangen deshalb, dass das Array gültig bleibt. Hier liegt der Puffer ohnehin schon im Modul – kopieren kostet nichts zusätzlich und nimmt dem Aufrufer eine Fehlerquelle ab.

7. **ISR-Funktion (RX-Interrupt) programmieren:**
   - Die ISR wird über das **RX**-Interrupt angestoßen, nicht über TX – die Begründung steht im Unterkapitel.
   - Abbruchbedingung: Vergleich von `TxData.cnt` mit `TxData.len`
   - Ist die Übertragung vollständig, `TxSuc` auf 1 setzen. Dieses Bit dient später dazu, auf das Ende einer Übertragung zu warten – etwa bei der Displayinitialisierung.

8. **Statusabfrage nach außen anbieten:** `unsigned char halUSCIB1Busy(void)`, die `TxSuc` kapselt und `1` liefert, solange eine Übertragung läuft.

   Mit `halUSCIB1Init()`, `halUSCIB1Send()` und `halUSCIB1Busy()` sind die drei Dinge beisammen, die eine Schnittstelle nach außen anbietet. Ab hier greift niemand außerhalb des Moduls mehr auf `LCD_Data` zu – weder lesend noch schreibend.

9. **Chip Select (CS) einbinden:**
   - Vor der Übertragung: `CS_LOW`
   - Nach dem letzten Byte: `CS_HIGH`
   - über das Makro aus `halGPIO.h`, das Sie in der vorigen Aufgabe angelegt haben

10. **[Meilensteinüberprüfung]** Woran erkennt der Driver Layer, dass eine Übertragung abgeschlossen ist?

---

## UART und I2C einbinden

Beide Module sind deutlich aufwendiger als SPI: Der UART muss in einem kontinuierlichen Zeichenstrom den Anfang eines Rahmens erkennen, und die I2C-Serviceroutine ist ein Zustandsautomat mit Adressierung, Repeated Start und Fehlerbehandlung.

Diese Module schreiben Sie **nicht** selbst. Ihre Aufgabe ist es, sie in Betrieb zu nehmen und ihr Verhalten so zu dokumentieren, dass Sie sie in den folgenden Einheiten sicher verwenden können.

> Grundlagen, Register und die vollständige Schnittstellenbeschreibung:
> ➡ **[UART am MSP430](UART.md)** · ➡ **[I2C am MSP430](I2C.md)**

Beim UART bekommen Sie `halUSCIA0` – den Debug-UART, der über den Debugger am PC als COM-Port erscheint. Sein Empfangszustand liegt dabei nicht im Modul selbst, sondern in einer eigenen Datei. Warum das so gebaut ist, steht im Unterkapitel; vollständig auflösen wird sich die Frage in Einheit 7.

> **Achtung, für alle Module:** Die übergebenen Arrays müssen gültig bleiben, bis die Übertragung abgeschlossen ist. Die Module merken sich den **Zeiger**, nicht den Inhalt. Eine lokale Variable in einer Funktion, die vorher zurückkehrt, führt zu Datenmüll auf der Leitung.

### [AUFGABE] UART und I2C in Betrieb nehmen

1. Kopieren Sie folgende Dateien in Ihren `HAL`-Ordner:

   | Datei | Inhalt |
   |---|---|
   | `halUSCIA0.c/.h` | Debug-UART über den eZ-FET-Backchannel |
   | `halSharedUART.h` | Empfangs- und Sendezustand des UART |
   | `halUSCIB0.c/.h` | I2C-Master |

2. `halSharedUART.h` **definiert** Variablen, es deklariert sie nicht nur. Die Datei darf deshalb nur an **einer** Stelle inkludiert werden – im Projekt ist das `halGeneral.c`. Ein zweites `#include` führt zu doppelten Symbolen beim Linken.

3. Rufen Sie `halUSCIA0Init()` und `halUSCIB0Init()` in `halInit()` auf. Die Parameter entnehmen Sie den Kommentaren in den Headern.

   > **Ausblick:** Im fertigen Fahrzeug wird der UART nicht hier initialisiert, sondern aus dem Driver Layer heraus – weil erst die Anwendung entscheidet, über welchen Weg die Daten gehen. In dieser Einheit bleiben wir auf der HAL-Ebene; die Umstellung folgt in Einheit 7.

4. Prüfen Sie die Pins aus Ihrer Tabelle noch einmal gegen das, was die drei Module erwarten. Debug-UART und I2C liegen auf gemappten Pins – vergleichen Sie mit dem SPI, wo die Sonderfunktion fest auf dem Pin liegt.

5. Schreiben Sie für jede Schnittstelle eine kurze Testausgabe, die zyklisch aus der Endlosschleife läuft:
   - **SPI:** zwei bekannte Bytes, z. B. `0xA5` und `0x3C`
   - **Debug-UART:** eine kurze Zeichenfolge über `halUSCIA0Send()`
   - **I2C:** ein Schreibzugriff und ein Lesezugriff auf eine frei gewählte Adresse. Es hängt noch keine Gegenstelle am Bus, der Zugriff endet also mit einem NACK – das ist hier das **erwartete** Ergebnis. Prüfen Sie `halI2CError()`; eine vollständige Übertragung erzeugen Sie erst bei der Messung.

   > **Achtung:** Stoßen Sie eine neue Sendung erst an, wenn `halUSCIA0TxBusy()` eine 0 liefert. Das Modul merkt sich den Zeiger auf Ihr Array, nicht dessen Inhalt.

6. **Debug-UART am PC prüfen.** Starten Sie ein Terminalprogramm (z. B. HTerm), wählen Sie den COM-Port des Debuggers – er erscheint als *MSP Application UART* – und stellen Sie 57600 Baud, 8N1 ein. Ihre Zeichenfolge muss dort im Klartext ankommen.

   Senden Sie anschließend in die Gegenrichtung: Geben Sie mit `halUSCIA0Start()` ein Sync-Muster und eine Rahmenlänge vor, tippen Sie den passenden Rahmen in HTerm ein und holen Sie ihn mit `halUSCIA0GetFrame()` ab. Prüfen Sie den Inhalt im Debugger.

7. **[Meilensteinüberprüfung]** Rechnen Sie den Baudratenteiler für Ihre Taktfrequenz und 57600 Baud nach. Welchen Wert nimmt der Modulator an, und warum ist er nötig?

### [AUFGABE] Funktionsdiagramm der vorgegebenen Module

Fremden Code einzubinden ist einfach – ihn zu verstehen ist die eigentliche Arbeit. Erstellen Sie je ein Funktionsdiagramm für **UART** und **I2C**, das den Ablauf von außen nachvollziehbar macht.

**Was das Diagramm zeigen muss:**

- Welche Funktion stößt eine Übertragung an, und was passiert dabei im Register?
- Was übernimmt die ISR, und wodurch wird sie ausgelöst?
- Welche Zustände durchläuft eine Übertragung, und welche Flags zeigen sie nach außen an?
- Wo wird ein Flag gesetzt, wo gelöscht?
- Wie endet eine Übertragung – im Normalfall und im Fehlerfall?

**Form:** Flussdiagramm, Zustandsdiagramm oder Sequenzdiagramm, je nachdem was den jeweiligen Ablauf besser trifft. Handskizze genügt, sofern lesbar. Entscheidend ist, dass ein Außenstehender danach weiß, wann er welche Funktion aufrufen darf.

**Leitfragen, die Ihr Diagramm beantworten soll:**

*UART:*
1. Der Empfänger sucht in einem fortlaufenden Zeichenstrom nach dem Anfang eines Rahmens. Wie erkennt er ihn, und was passiert bei einem Zeichen, das nicht passt?
2. Was geschieht, wenn ein Byte eintrifft, das selbst wieder der Rahmenanfang sein könnte?
3. Warum wird der TX-Interrupt am Ende einer Übertragung wieder abgeschaltet?

*I2C:*

4. `halI2CWriteRead()` schreibt zuerst und liest danach. An welcher Stelle wechselt die ISR die Richtung, und welcher Buszustand wird dabei erzeugt?
5. Was passiert, wenn kein Teilnehmer auf die Adresse antwortet?
6. Beim Lesen eines einzelnen Bytes gibt es einen Sonderfall. Welchen, und warum ist er nötig?

Beantworten Sie die Fragen schriftlich – das Diagramm allein reicht nicht.

---

## Verifikation mit dem Analog Discovery

Ob eine Schnittstelle korrekt konfiguriert ist, zeigt erst die Messung. Alle drei Schnittstellen werden mit demselben Gerät geprüft – dem Digilent Analog Discovery unter WaveForms. Es dient dabei zugleich als Gegenstelle: Mithören zeigt nur, was der Controller sendet. Erst mit einem antwortenden Teilnehmer lässt sich der Empfangspfad prüfen – und beim I2C überhaupt erst eine vollständige Übertragung erzeugen, weil ohne Antwort jeder Zugriff mit einem NACK endet.

> **Anschluss, Pinbelegung und alle Einstellungen in WaveForms stehen im Unterkapitel:**
> ➡ **[Messen mit dem Analog Discovery](AnalogDiscovery.md)**
>
> Arbeiten Sie es vor dieser Aufgabe durch. Die Schritte unten nennen nur noch, *was* zu messen ist, nicht mehr, wie das Gerät dafür einzurichten ist.

### [AUFGABE] Messen am Analog Discovery

**SPI**

1. Kanäle auf CLK, MOSI, MISO und CS legen. Unter *Logic* den SPI-Dekoder hinzufügen: 8 Bit, MSB first, CS aktiv LOW, Abtastflanke passend zu Ihrer Konfiguration. Der Dekoder benennt die Flanke direkt – übersetzen Sie Ihre beiden Registerbits darauf, statt eine Modusnummer zu raten.
2. Zeichnen Sie die zyklische Ausgabe Ihrer beiden Bytes auf. Dokumentieren Sie Taktfrequenz, Bitreihenfolge und das Verhalten der CS-Leitung zwischen den Bytes.
3. Betreiben Sie das Analog Discovery unter *Protocol → SPI* als **Slave** und lassen Sie es ein bekanntes Byte zurücksenden. Prüfen Sie im Debugger, ob dieses Byte in `RxData.Data` ankommt.

   Damit weisen Sie nach, dass Ihr Empfangspfad funktioniert – am Display allein ließe sich das nicht zeigen, weil es nichts zurückschickt.

**UART**

4. **Debug-UART:** Kanal auf die TX-Leitung von UCA0 legen, unter *Logic* den UART-Dekoder hinzufügen: 57600 Baud, 8N1. Messen Sie die Dauer eines einzelnen Bits und vergleichen Sie sie mit der eingestellten Baudrate.

5. Betreiben Sie das Analog Discovery unter *Protocol → UART* als **Empfänger** und lassen Sie sich die Zeichenfolge im Klartext anzeigen. Senden Sie anschließend in der Gegenrichtung: Das Gerät schickt einen Rahmen, Ihr Controller empfängt ihn über `halUSCIA0Start()` und `halUSCIA0GetFrame()`.

   Wählen Sie das Sync-Muster so, wie Sie es an `halUSCIA0Start()` übergeben haben – sonst erkennt der Empfänger den Rahmenanfang nicht.

**I2C**

6. Kanäle auf SDA und SCL legen, unter *Logic* den I2C-Dekoder hinzufügen.
7. Betreiben Sie das Analog Discovery unter *Protocol → I2C* als **Slave** mit einer frei gewählten Adresse. Führen Sie von Ihrem Controller aus einen Schreibzugriff und anschließend einen Lesezugriff auf diese Adresse durch.
8. Im aufgezeichneten Diagramm müssen erkennbar sein: Startbedingung, Adressbyte mit R/W-Bit, ACK des Slaves, Repeated Start, gelesene Bytes, Stoppbedingung.
9. **Fehlerfall provozieren:** Greifen Sie auf eine Adresse zu, auf die kein Teilnehmer eingestellt ist. Zeichnen Sie auf, was passiert, und prüfen Sie im Debugger, ob `halI2CError()` gesetzt wird.

**Zu dokumentieren**

10. Je ein Screenshot pro Schnittstelle mit sichtbarer Dekodierung, dazu einer vom I2C-Fehlerfall.
11. Stimmen die gemessenen Taktfrequenzen mit Ihren berechneten Werten überein? Woher kommt eine eventuelle Abweichung?
12. **[Meilensteinüberprüfung]** Vergleichen Sie die Diagramme: Woran erkennen Sie im Signalverlauf, ob eine Schnittstelle synchron oder asynchron arbeitet?
13. **[Meilensteinüberprüfung]** SPI verwendet eine eigene Leitung zur Auswahl des Teilnehmers, I2C nicht. Wie lösen die beiden Protokolle dieselbe Aufgabe unterschiedlich, und welche Folgen hat das für die Anzahl der nötigen Leitungen?
14. **[Meilensteinüberprüfung]** Wie unterscheidet sich der Signalverlauf im I2C-Fehlerfall von einer erfolgreichen Übertragung? An welcher Stelle genau bricht sie ab?

---

## Testing

In dieser Einheit schreiben Sie die Tests für alle drei Module eigenständig:

➡ **[Testing – Schnittstellen](Testing/Testing.md)**

## Referenzen

- **MSP430x5xx and MSP430x6xx Family User Guide**, Texas Instruments, Literature Number: SLAU208O, Rev. O, April 2019.  
  Verfügbar unter: [https://www.ti.com/lit/pdf/slau208](https://www.ti.com/lit/pdf/slau208)

- **MSP430F5335 Datasheet**, Texas Instruments, Document Number: SLAS590N, Rev. N, October 2018.  
  Verfügbar unter: [https://www.ti.com/lit/gpn/msp430f5335](https://www.ti.com/lit/gpn/msp430f5335)

- John H. Davies, **MSP430 Microcontroller Basics**, Newnes/Elsevier, ISBN 978‑0‑7506‑8276‑3.  

[⬆ Zurück zum Hauptverzeichnis](../README.md#kapitelübersicht--aufgabenstellungen)