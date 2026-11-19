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

**Zu erreichende Ziele**

Am Ende dieser Einheit sprechen alle drei seriellen Schnittstellen, und Sie haben am
Messgerät nachgewiesen, dass auf der Leitung steht, was dort stehen soll.

- [ ] Für jedes Signal von SPI, UART und I2C ist der Controllerpin bestimmt und in `halGPIOInit()` konfiguriert; gemappte Pins sind über `PMAPKEYID` freigeschaltet und wieder gesperrt.
- [ ] Chip Select ist ein gewöhnlicher Ausgang mit eigenem Makro in `halGPIO.h`.
- [ ] `halUSCIB1.c/.h` ist selbst geschrieben: SPI-Master, 100 kHz, 8 Bit, MSB first, RX-Interrupt freigegeben.
- [ ] Die SPI-ISR schreibt die Folgebytes nach, gibt CS nach jedem Byte frei und meldet das Ende der Übertragung über `TxSuc`.
- [ ] Die vorgegebenen Module für UART und I2C sind eingebunden und werden in `halInit()` initialisiert; `halSharedUART.h` ist nur an einer Stelle inkludiert.
- [ ] Die Testausgabe läuft zyklisch: zwei bekannte Bytes über SPI, eine Zeichenfolge über den Debug-UART, ein Schreib- und ein Lesezugriff über I2C.
- [ ] Je ein Funktionsdiagramm für UART und I2C liegt vor, die Leitfragen sind schriftlich beantwortet.
- [ ] Alle drei Schnittstellen sind am Analog Discovery gemessen und dekodiert; das Gerät war dabei Gegenstelle, nicht nur Mithörer.
- [ ] (Optional) Der I2C-Fehlerfall ist provoziert und aufgezeichnet; `halI2CError()` wird gesetzt.

**Tests:** HAL-Tests – eigene Testmodule für SPI, UART und I2C; die Fälle aus den Einheiten 2 bis 4 bestehen weiterhin.

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
  - Wie wird bei I2C ein Teilnehmer adressiert, wenn es keine Chip-Select-Leitung gibt?
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

In dieser Einheit werden die drei seriellen Schnittstellen des Fahrzeugs in Betrieb genommen. **SPI** implementieren Sie selbst; es ist die überschaubarste der drei und wird in der nächsten Einheit für das Display gebraucht. **UART** und **I2C** werden als fertige Module bereitgestellt; dort liegt die Arbeit darin, den fremden Code zu verstehen und sein Verhalten zu dokumentieren.

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
| ➡ **[UART am MSP430](UART.md)** | Frameaufbau, Baudratenberechnung mit Modulator, USCI_A0, Frameerkennung, Lage des Empfangspuffers |
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
| **DL** | HAL-Funktionen aufrufen, Status abfragen | Register manipulieren, `#include <msp430.h>` nicht mehr notwendig|
| **AL** | DL-Funktionen aufrufen | Keine HAL Funktionen, keine Registermanipulation|

Praktisch heißt das: Jede Schnittstelle bietet nach außen genau drei Dinge an – eine Funktion zum Initialisieren, eine zum Anstoßen einer Übertragung, und eine Abfrage, ob sie fertig ist. Was dazwischen passiert, bleibt im Modul.

```c
halUSCIB1Init();                        // einmal beim Start
halUSCIB1Transmit();                    // Übertragung anstoßen
while (LCD_Data.Status.B.TxSuc == 0);   // auf das Ende warten
```

Dieselbe Dreiteilung finden Sie in den vorgegebenen Modulen wieder: `halUSCIA0Init()` / `halUSCIA0Send()` / `halUSCIA0TxBusy()` und `halUSCIB0Init()` / `halI2CWrite()` / `halI2CBusy()`. Das ist kein Zufall, sondern dasselbe Muster.

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
- Was geschieht mit einem Schreibzugriff auf ein `PxMAPy`-Register, nachdem das Zugriffsfenster geschlossen ist?

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

   // Hinweis: Nicht jedes Signal braucht das Mapping. Prüfen Sie in der
   //          Port-Funktionstabelle, welche Ihrer Pins die Sonderfunktion
   //          fest tragen – dort genügt das PxSEL-Bit.
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
   - CPOL = 1, CPHA = 0
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

4. **Globale Variable `LCD_Data` vom Typ `USCIB1_SPICom` in `halUSCIB1.c` deklarieren** – `volatile` nicht vergessen, die ISR schreibt darauf.

5. **Als `extern volatile` in den Dateien deklarieren, die sie verwenden.**

6. **Das Gerüst für Sende- und Serviceroutine** sieht so aus. Was in die einzelnen TODOs gehört, sagen die Punkte 7 bis 9 – ersetzen Sie jedes davon durch Code und schreiben Sie Ihre Begründung in den Kommentar daneben:

   ```c
   /* Auszug aus halUSCIB1.c – Senden und Serviceroutine */

   extern volatile USCIB1_SPICom LCD_Data;

   /* ---- Übertragung anstoßen ----------------------------------- */

   void halUSCIB1Transmit(void)
   {
       // TODO: Nichts tun, solange noch eine Übertragung läuft.
       //       Welches Bit zeigt das an?

       // TODO: Chip Select aktivieren – Makro aus halGPIO.h

       // TODO: Zähler auf das erste Byte stellen und den
       //       Fertig-Status zurücknehmen

       // TODO: Erstes Byte aus TxData.Data in UCB1TXBUF schreiben.
       //       Alle weiteren Bytes schreibt die ISR.
       //       TxData.len hat der Aufrufer vorher gesetzt.
   }

   /* ---- Serviceroutine ----------------------------------------- */

   #pragma vector = USCI_B1_VECTOR
   __interrupt void usciB1Isr(void)
   {
       switch (UCB1IV)              // Lesen löscht das behandelte Flag
       {
           case USCI_UCRXIFG:

               // TODO: Chip Select freigeben. Das Byte ist jetzt
               //       vollständig durchgelaufen – warum gehört diese
               //       Zeile hierher und nicht in den Fertig-Zweig?

               // TODO: Empfangenes Byte aus UCB1RXBUF abholen.
               //       Warum muss dieses Register in jedem Fall
               //       gelesen werden, obwohl das Display nichts
               //       zurückschickt?

               // TODO: Zähler der übertragenen Bytes fortschreiben

               // TODO: Abbruchbedingung – TxData.cnt gegen TxData.len
               //
               //       Übertragung vollständig:
               //         - TxSuc setzen
               //         - Zähler so hinterlassen, dass die nächste
               //           Übertragung wieder bei Byte 0 beginnt
               //
               //       sonst:
               //         - Chip Select wieder aktivieren
               //         - nächstes Byte aus TxData.Data nachschreiben

               break;

           default:
               break;
       }
   }
   ```

   Die vier Schritte in der ISR – CS freigeben, Byte abholen, Zähler fortschreiben, Abbruchbedingung prüfen – sind in dieser Reihenfolge zu durchlaufen. Wer zuerst prüft und dann zählt, sendet ein Byte zu viel.

7. **Funktion `halUSCIB1Transmit()` schreiben:**
   - Erstes Byte in `UCB1TXBUF` schreiben
   - `TxSuc` auf 0 setzen
   - Alle weiteren Bytes schreibt die ISR, bis `TxData.len` erreicht ist

8. **ISR-Funktion (RX-Interrupt) programmieren:**
   - Die ISR wird über das **RX**-Interrupt angestoßen, nicht über TX – die Begründung steht im Unterkapitel.
   - Abbruchbedingung: Vergleich von `TxData.cnt` mit `TxData.len`
   - Ist die Übertragung vollständig, `TxSuc` auf 1 setzen. Dieses Bit dient später dazu, auf das Ende einer Übertragung zu warten – etwa bei der Displayinitialisierung.
   - Die Abfrage über `UCB1IV` ist dieselbe Konstruktion, die Sie bei `P1IV` in Einheit 2 kennengelernt haben und die Ihnen in den vorgegebenen UART- und I2C-Modulen wieder begegnet. Da hier nur eine Interruptquelle freigegeben ist, wäre eine direkte Abfrage von `UCB1IFG` ebenfalls korrekt – überlegen Sie, warum sie das bei mehreren freigegebenen Quellen nicht mehr wäre.

9. **Chip Select (CS) einbinden** – über das Makro aus `halGPIO.h`, das Sie in der vorigen Aufgabe angelegt haben:
   - vor dem ersten Byte: `CS_LOW`
   - nach jedem vollständig übertragenen Byte: `CS_HIGH`
   - vor jedem nachgeschriebenen Byte wieder: `CS_LOW`

   Dass CS zwischen den Bytes steigt und nicht über die ganze Übertragung aktiv bleibt, hat einen Grund – er steht in ➡ **[SPI am MSP430](SPI.md)**.

10. **[Meilensteinüberprüfung]** Woran erkennt der Driver Layer, dass eine Übertragung abgeschlossen ist?

11. **[Meilensteinüberprüfung]** Was passiert, wenn `halUSCIB1Transmit()` mit `TxData.len == 0` aufgerufen wird? Sehen Sie sich dazu Ihre Abbruchbedingung genau an.

---

## UART und I2C einbinden

Beide Module sind deutlich aufwendiger als SPI: Der UART muss in einem kontinuierlichen Zeichenstrom den Anfang eines Frames erkennen, und die I2C-Serviceroutine ist ein Zustandsautomat mit Adressierung, Repeated Start und Fehlerbehandlung.

Diese Module schreiben Sie **nicht** selbst. Ihre Aufgabe ist es, sie in Betrieb zu nehmen und ihr Verhalten so zu dokumentieren, dass Sie sie in den folgenden Einheiten sicher verwenden können.

> Grundlagen, Register und die vollständige Schnittstellenbeschreibung:
> ➡ **[UART am MSP430](UART.md)** · ➡ **[I2C am MSP430](I2C.md)**

Der UART läuft in dieser Einheit als Debug-Schnittstelle über `halUSCIA0`: über den eZ-FET-Backchannel des Debuggers, am PC als virtueller COM-Port. Er bleibt dabei auf der HAL-Ebene und wird direkt angesprochen – wer ihn später initialisiert und mit welcher Baudrate, ist eine Frage des Driver Layers und kommt in einer späteren Einheit.

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

   > **Ausblick:** Im fertigen Fahrzeug wird der UART nicht hier initialisiert, sondern aus dem Driver Layer heraus – weil erst die Anwendung entscheidet, über welchen Weg und mit welcher Baudrate die Daten gehen. In dieser Einheit bleiben wir auf der HAL-Ebene.

4. Prüfen Sie die Pins aus Ihrer Tabelle noch einmal gegen das, was die beiden Module erwarten. Debug-UART und I2C liegen auf gemappten Pins, die SPI-Signale auf Pins mit fester Funktion – vergleichen Sie, was in `halGPIOInit()` für die beiden Fälle jeweils zu tun war.

5. Schreiben Sie für jede Schnittstelle eine kurze Testausgabe, die zyklisch aus der Endlosschleife läuft:
   - **SPI:** zwei bekannte Bytes, z. B. `0xA5` und `0x3C`
   - **Debug-UART:** eine kurze Zeichenfolge über `halUSCIA0Send()`
   - **I2C:** ein Schreibzugriff und ein Lesezugriff auf eine frei gewählte Adresse

   > **Achtung:** Stoßen Sie eine neue Sendung erst an, wenn `halUSCIA0TxBusy()` eine 0 liefert – sonst überschreiben Sie den Sendezeiger einer noch laufenden Übertragung.

6. **Debug-UART am PC prüfen.** Starten Sie ein Terminalprogramm (z. B. HTerm), wählen Sie den COM-Port des Debuggers – er erscheint als *MSP Application UART* – und stellen Sie 57600 Baud, 8N1 ein. Ihre Zeichenfolge muss dort im Klartext ankommen.

   Senden Sie anschließend in die Gegenrichtung: Geben Sie mit `halUSCIA0Start()` ein Sync-Muster und eine Framelänge vor, tippen Sie den passenden Frame in HTerm ein und holen Sie ihn mit `halUSCIA0GetFrame()` ab. Prüfen Sie den Inhalt im Debugger.

   > Dasselbe prüfen Sie im nächsten Abschnitt noch einmal am Analog Discovery – dort sehen Sie zusätzlich das Bittiming auf der Leitung.

7. **[Meilensteinüberprüfung]** Rechnen Sie den Baudratenteiler für Ihre Taktfrequenz und 57600 Baud nach. Welchen Wert nimmt der Modulator an, und warum ist er nötig?

8. **[Meilensteinüberprüfung]** Rechnen Sie dieselbe Teilung für 9600 Baud nach. Wie groß ist die relative Abweichung jeweils, und welche der beiden Baudraten ist damit unkritischer?

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
1. Der Empfänger sucht in einem fortlaufenden Zeichenstrom nach dem Anfang eines Frames. Wie erkennt er ihn, und was passiert bei einem Zeichen, das nicht passt?
2. Was geschieht, wenn ein Byte eintrifft, das selbst wieder der Frameanfang sein könnte?
3. Warum wird der TX-Interrupt am Ende einer Übertragung wieder abgeschaltet?

*I2C:*

4. `halI2CWriteRead()` schreibt zuerst und liest danach. An welcher Stelle wechselt die ISR die Richtung, und welcher Buszustand wird dabei erzeugt?
5. Was passiert, wenn kein Teilnehmer auf die Adresse antwortet?
6. Beim Lesen eines einzelnen Bytes gibt es einen Sonderfall. Welchen, und warum ist er nötig?

Beantworten Sie die Fragen schriftlich – das Diagramm allein reicht nicht.

---

## Verifikation mit dem Analog Discovery

Ob eine Schnittstelle korrekt konfiguriert ist, zeigt erst die Messung. Alle drei Schnittstellen werden mit demselben Gerät geprüft – dem Digilent Analog Discovery unter WaveForms. Es dient dabei zugleich als Gegenstelle: Mithören zeigt nur, was der Controller sendet. Erst mit einem antwortenden Teilnehmer lässt sich der Empfangspfad prüfen – und beim I2C überhaupt erst eine vollständige Übertragung erzeugen, weil ohne Antwort jeder Zugriff mit einem NACK endet.

> **Anschluss, Pinbelegung und die vollständigen Einstellungen stehen im Unterkapitel:**
> ➡ **[Messen mit dem Analog Discovery](AnalogDiscovery.md)**
>
> Arbeiten Sie es vor dieser Aufgabe durch. Die Schritte unten sagen, *was* zu messen ist; die kursiven Zeilen darunter nennen nur, **wo** die jeweilige Einstellung in WaveForms liegt. Die Begründung der Werte und die Fehlertabelle bleiben im Unterkapitel.

### [AUFGABE] Messen am Analog Discovery

> **Einmal vorab, für alle drei Schnittstellen:** Masse zuerst verbinden. *Logic* und *Protocol* greifen intern auf dieselben Pins zu und dürfen deshalb nicht gleichzeitig offen sein – zum Mitlesen während eines Protocol-Betriebs die Betriebsart **Spy** verwenden. Abtastrate und Puffertiefe stehen im Logic-Fenster oben links unter **Rate** und **Samples**; für alles in dieser Einheit sind 10 MS/s bequem ausreichend.

**SPI**

1. Kanäle auf CLK, MOSI, MISO und CS legen. Unter *Logic* den SPI-Dekoder hinzufügen: 8 Bit, MSB first, CPOL und CPHA passend zu Ihrer Konfiguration, CS aktiv LOW.

   *In WaveForms:* Hauptfenster → **Logic**. Im Signalraster **+ (Add)** → *Protocol* → **SPI** für die Dekoderzeile. Dieselbe Schaltfläche → *Signal* für die vier Rohkanäle – der Dekoder zeigt Bytes, aber nicht, warum er sie so gelesen hat. Die Felder *Select*, *Clock*, *Data* und *Bits per word* liegen in der Dekoderzeile selbst. Trigger auf die **fallende Flanke von CS**, damit das Bild am Anfang der Übertragung beginnt.

2. Zeichnen Sie die zyklische Ausgabe Ihrer beiden Bytes auf. Dokumentieren Sie Taktfrequenz, Bitreihenfolge und das Verhalten der CS-Leitung zwischen den Bytes – und begründen Sie, warum CS dort steigt.

   *In WaveForms:* **Single** statt **Run** – ein stehendes Bild lässt sich vermessen. Für die Taktfrequenz zwei Cursor auf benachbarte Taktflanken setzen; die Anzeige liefert den Abstand, der Kehrwert ist die Frequenz. Bild über *File → Export*.

3. Betreiben Sie das Analog Discovery unter *Protocol → SPI* als **Slave** und lassen Sie es ein bekanntes Byte zurücksenden. Prüfen Sie im Debugger, ob dieses Byte in `RxData.Data` ankommt.

   *In WaveForms:* Logic-Fenster **schließen**. Hauptfenster → **Protocol** → Reiter **SPI** → Betriebsart **Slave**. Das Antwortbyte in die MISO-Daten des Reiters eintragen. Pinzuordnung, Frequenz, Polarität und Bitreihenfolge stehen in denselben Feldern wie beim Dekoder – nur wirken sie hier auf das, was das Gerät tatsächlich treibt.

   Damit weisen Sie nach, dass Ihr Empfangspfad funktioniert – am Display allein ließe sich das nicht zeigen, weil es nichts zurückschickt.

**UART**

4. Kanal auf die TX-Leitung von UCA0 legen, unter *Logic* den UART-Dekoder hinzufügen: 57600 Baud, 8N1. Messen Sie die Dauer eines einzelnen Bits und vergleichen Sie sie mit der eingestellten Baudrate.

   *In WaveForms:* **Logic** → **+ (Add)** → *Protocol* → **UART**. In der Dekoderzeile den Datenkanal, *Bits* 8, *Parity* None, *Stop* 1 und **Rate** 57600. Trigger auf die **fallende Flanke** der TX-Leitung – das ist das Startbit. Bitzeit mit zwei Cursorn auf aufeinanderfolgende Flanken.

5. Stellen Sie den Dekoder anschließend versuchsweise auf 9600 Baud um, ohne am Controller etwas zu ändern. Zeichnen Sie auf, was er dann liefert – das ist das Fehlerbild, das Sie bei einer falsch eingestellten Gegenstelle erwarten dürfen. Rechnen Sie nach, in welchem Verhältnis die beiden Bitzeiten stehen.

   *In WaveForms:* nur das Feld **Rate** der Dekoderzeile ändern und erneut **Single** drücken. Die Rohkanäle bleiben identisch – alles, was sich im Bild ändert, kommt aus der Auswertung, nicht von der Leitung.

6. Betreiben Sie das Analog Discovery unter *Protocol → UART* als **Empfänger** und lassen Sie sich die Zeichenfolge im Klartext anzeigen. Senden Sie anschließend in der Gegenrichtung: Das Gerät schickt einen Frame, Ihr Controller empfängt ihn über `halUSCIA0Start()` und `halUSCIA0GetFrame()`.

   *In WaveForms:* **Protocol** → Reiter **UART** → **Send & Receive**. Baudrate und Pinzuordnung im Reiter prüfen, bevor Sie senden; empfangene Zeichen erscheinen im Receive-Fenster, zu sendende tragen Sie im Send-Feld ein. Beim UART gibt es keine Master-/Slave-Rolle – deshalb heißt die Betriebsart hier anders als bei SPI und I2C.

   Wählen Sie das Sync-Muster so, wie Sie es an `halUSCIA0Start()` übergeben haben – sonst erkennt der Empfänger den Frameanfang nicht.

7. **[Meilensteinüberprüfung]** Der Debug-UART ließ sich mit HTerm am PC prüfen, weil der Debugger einen virtuellen COM-Port bereitstellt. Was müsste am Fahrzeug stecken, damit auch eine Schnittstelle ohne diesen Weg am PC ankommt?

**I2C**

8. Kanäle auf SDA und SCL legen, unter *Logic* den I2C-Dekoder hinzufügen.

   *In WaveForms:* **Logic** → **+ (Add)** → *Protocol* → **I2C**. SCL und SDA in der Dekoderzeile zuordnen, Adressdarstellung auf **7 Bit** stellen. Trigger über den Trigger-Assistenten des I2C-Dekoders auf die **Startbedingung**.

9. Betreiben Sie das Analog Discovery unter *Protocol → I2C* als **Slave** mit einer frei gewählten Adresse. Führen Sie von Ihrem Controller aus einen Schreibzugriff und anschließend einen Lesezugriff auf diese Adresse durch.

   *In WaveForms:* Logic-Fenster schließen. **Protocol** → Reiter **I2C** → Betriebsart **Slave**. Dort drei Dinge eintragen: die Adresse (7 Bit), die Anzahl der zu quittierenden Schreibbytes, und die Bytes, die bei einem Lesezugriff zurückgehen. Erst damit entsteht eine vollständige Übertragung mit ACK.

10. Im aufgezeichneten Diagramm müssen erkennbar sein: Startbedingung, Adressbyte mit R/W-Bit, ACK des Slaves, Repeated Start, gelesene Bytes, Stoppbedingung.

    *In WaveForms:* zum Mitlesen die Betriebsart **Spy** im selben Reiter – sie dekodiert dasselbe wie *Logic*, ohne dass sich die beiden Instrumente um die Pins streiten.

11. **Fehlerfall provozieren:** Greifen Sie auf eine Adresse zu, auf die kein Teilnehmer eingestellt ist. Zeichnen Sie auf, was passiert, und prüfen Sie im Debugger, ob `halI2CError()` gesetzt wird.

    *In WaveForms:* nichts umstellen. Ändern Sie die Adresse **im Controller**, nicht die des Slaves – so bleibt die Aufzeichnung mit der aus Schritt 10 direkt vergleichbar.

**Zu dokumentieren**

12. Je ein Screenshot pro Schnittstelle mit sichtbarer Dekodierung – beim UART zusätzlich der Versuch mit falsch eingestelltem Dekoder – sowie einer vom I2C-Fehlerfall.
13. Stimmen die gemessenen Taktfrequenzen mit Ihren berechneten Werten überein? Woher kommt eine eventuelle Abweichung?
14. **[Meilensteinüberprüfung]** Vergleichen Sie die Diagramme: Woran erkennen Sie im Signalverlauf, ob eine Schnittstelle synchron oder asynchron arbeitet?
15. **[Meilensteinüberprüfung]** SPI verwendet eine eigene Leitung zur Auswahl des Teilnehmers, I2C nicht. Wie lösen die beiden Protokolle dieselbe Aufgabe unterschiedlich, und welche Folgen hat das für die Anzahl der nötigen Leitungen?
16. **[Meilensteinüberprüfung]** Wie unterscheidet sich der Signalverlauf im I2C-Fehlerfall von einer erfolgreichen Übertragung? An welcher Stelle genau bricht sie ab?

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