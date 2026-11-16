[⬅ Zurück zur Kapitelübersicht](../README.md#einheitsübersicht--aufgabenstellungen)
# Display und Kommunikation

## Inhalt
- [Die Unterkapitel](#die-unterkapitel)
- [Die Anwendung ruft nur den Driver Layer](#die-anwendung-ruft-nur-den-driver-layer)
- [Steuerleitungen des Displays](#steuerleitungen-des-displays)
- [Displaytreiber vervollständigen](#displaytreiber-vervollständigen)
- [Kommunikation in Betrieb nehmen](#kommunikation-in-betrieb-nehmen)
- [Verifikation mit dem Analog Discovery](#verifikation-mit-dem-analog-discovery)
- [Testing](#testing)

**Zu erreichende Ziele**

Am Ende dieser Einheit zeigt das Fahrzeug eigene Werte auf dem Display an und tauscht
Daten mit dem PC aus – über USB oder über Bluetooth, entschieden durch ein einziges `#define`.

- [ ] A0- und Reset-Leitung sind in `halGPIOInit()` als Ausgänge konfiguriert und über `halSetLCDCmd()`, `halSetLCDData()` und `halSetLCDReset()` schaltbar.
- [ ] `driverLCD.c` ist vervollständigt: löschen, Text und Zahlen ausgeben.
- [ ] Text läuft am Zeilenende in die nächste Seite um und bricht am Displayende ab.
- [ ] `driverCom.c` ist vervollständigt: Telemetrierahmen packen, Kommandorahmen auspacken und auf das Rahmenende prüfen.
- [ ] `driverLCDInit()` und `driverComInit()` werden aus `driverInit()` aufgerufen; die Hauptschleife empfängt, zeigt an und sendet.
- [ ] Das MATLAB-Skript zeigt die gesendeten Werte; ein Kommando vom PC verändert Lenkung oder Gas.
- [ ] Die Umschaltung auf Bluetooth ist durch Ändern **einer** Zeile nachgewiesen, ohne Eingriff in `driverCom.c`.
- [ ] Telemetrierahmen und Kommandorahmen sind am Analog Discovery dekodiert; an der A0-Leitung ist erkennbar, welche SPI-Bytes Befehle und welche Bilddaten sind.

**Tests:** DL-Tests – erstmals ohne Hardware, weil Packen und Auspacken reine Logik sind; die Fälle aus den Einheiten 2 bis 5 bestehen weiterhin.

**Laborübung**

- [ST7565 Dot Matrix LCD Controller Datasheet](https://www.hpinfotech.ro/ST7565.pdf)
  - Seite 13: [MPU-Interface](https://www.hpinfotech.ro/ST7565.pdf#page=13)
  - Seite 15: [Display Data RAM](https://www.hpinfotech.ro/ST7565.pdf#page=15)
  - Seiten 32–41: [Befehlstabelle](https://www.hpinfotech.ro/ST7565.pdf#page=32)
- Crazy Car Controller FHJ Schaltplan [Crazy Car Schematic](https://fhjoanneum-my.sharepoint.com/:b:/g/personal/florian_mayer_fh-joanneum_at/EfXYu-rqsLRErJbybsbN4AEB_RUMizJhwpb5D_ysimZehA?e=Ti7PtO)
- Digilent **Analog Discovery** mit **WaveForms** – [Referenzhandbuch](https://digilent.com/reference/test-and-measurement/analog-discovery-3/reference-manual)

**Wissensüberprüfung**

- Recherche:
  - Wie ist der Bildspeicher eines ST7565 organisiert, und warum braucht die Spaltenadresse zwei Befehlsbytes?
  - Wozu dient die A0-Leitung, und warum genügt SPI allein nicht?
  - Warum braucht ein Datenstrom über UART einen Rahmen, ein SPI-Transfer aber nicht?
  - Was bedeutet Big Endian, und warum legt man das bei einem Protokoll fest?
- Arbeiten Sie die beiden Unterkapitel durch – siehe unten.

> **VORSICHT:** Bleibt das Programm beim ersten Displayzugriff stehen, setzt Ihr `halUSCIB1Init()` aus Einheit 5 das Statusbit `TxSuc` nicht auf 1. Nach einem Reset steht es auf 0, und `lcdSend()` wartet auf ein Ende, das nie kommt.

**Video**
 - *folgt*

### Durchzuführende Aufgaben
- [[AUFGABE] A0- und Reset-Leitung setzen](#aufgabe-a0--und-reset-leitung-setzen)
- [[AUFGABE] driverLCD – Text und Zahlen ausgeben](#aufgabe-driverlcd--text-und-zahlen-ausgeben)
- [[AUFGABE] driverCom – Rahmenformat implementieren](#aufgabe-drivercom--rahmenformat-implementieren)
- [[AUFGABE] Fernsteuerung und Telemetrie](#aufgabe-fernsteuerung-und-telemetrie)
- [[AUFGABE] Messen am Analog Discovery](#aufgabe-messen-am-analog-discovery)

---

In dieser Einheit bekommen die Schnittstellen aus Einheit 5 ihre Gegenstelle. Was am Messgerät ein Rechteckmuster war, wird zu Text auf einer Anzeige; was in HTerm eine Zeichenfolge war, wird zu einem Protokoll mit Rahmen, einem Plot am PC und der Wahl zwischen Kabel und Funk.

| Komponente | Modul | Verwendung | Ihre Aufgabe |
|---|---|---|---|
| Steuerleitungen | `halGPIO` | A0 und Reset des Displays | selbst ergänzen |
| Display | `driverLCD` | Anzeige am Fahrzeug | drei Funktionen selbst schreiben |
| Kommunikation | `driverCom` | Telemetrie und Fernsteuerung | zwei Funktionen selbst schreiben |
| AUX-UART | `halUSCIA1` | Bluetooth-Modul | aus Einheit 5, nur umschalten |

Beide Treiber werden am Ende mit dem Analog Discovery gemessen und dekodiert – diesmal nicht als Funktionsnachweis einer Schnittstelle, sondern als Prüfung Ihres eigenen Protokolls.

---

## Die Unterkapitel

Die Grundlagen stehen in zwei eigenen Unterkapiteln. Das erste behandelt den Displaycontroller, das zweite das Protokoll; beide enden mit einer Übersicht, was vorgegeben ist und was nicht.

| | Inhalt |
|---|---|
| ➡ **[Das Display am SPI](Display.md)** | Seiten und Spalten, A0-Leitung, Initialisierungssequenz, Fonttabelle, Fehlerbilder |
| ➡ **[Kommunikation mit dem Fahrzeug](Kommunikation.md)** | Rahmenformat, Big Endian, Umschaltung USB/Bluetooth, Gegenstelle am PC |

Arbeiten Sie das jeweilige Unterkapitel vor der zugehörigen Aufgabe durch. Die Aufgabenstellungen unten setzen voraus, dass Sie den Bildspeicheraufbau und das Rahmenformat kennen – sie wiederholen die Grundlagen nicht.

---

## Die Anwendung ruft nur den Driver Layer

In Einheit 5 galt: Registerzugriffe gehören in den HAL. Jetzt kommt die Gegenrichtung dazu – **ein Treiber darf nicht wissen, an welchem Modul er hängt.**

Sichtbar wird das an der Kommunikation. `driverCom.c` ruft nie `halUSCIA0Send()` oder `halUSCIA1Send()` auf, sondern nur `halComSend()`. Welches Modul dahintersteht, entscheidet ein einziges `#define` in `driverCom.h`:

```c
#define COM_USE_BT  0       // 0 = USB (UCA0), 1 = Bluetooth (UCA1)
```

Steht dort eine `1`, geht alles über Bluetooth – und **keine Zeile** in `driverCom.c` ändert sich. Dasselbe gilt beim Display: `driverLCD.c` kennt `driverLCDWriteData()` und `halSetLCDData()`, aber kein einziges USCI-Register.

Daraus ergibt sich, wo in dieser Einheit was entsteht:

| Schicht | Datei | was dazukommt |
|---|---|---|
| **HAL** | `halGPIO.c/.h` | A0- und Reset-Leitung, drei Schaltfunktionen |
| **DL** | `driverLCD.c` | `Clear`, `WriteText`, `WriteUInt` |
| **DL** | `driverCom.c` | `driverComSend()`, `driverComReceive()` |
| **AL** | `main.c` | Hauptschleife: empfangen, anzeigen, senden |

Die beiden Treiber werden in `driverInit()` gestartet, nach `halInit()` – `driverLCDInit()` braucht sowohl das SPI-Modul als auch die beiden GPIO-Leitungen:

```c
void driverInit(void)
{
    driverSetSteering(0);
    // driverESCinit();
    driverLCDInit();        // << neu
    driverComInit();        // << neu
}
```

Die Hauptschleife besteht aus drei Schritten, immer in derselben Reihenfolge:

```c
while (1)
{
    if (driverComReceive(&cmd, &val))       // 1. empfangen
    {
        // Kommando auf Lenkung oder Gas abbilden
    }

    driverLCDWriteUInt(val, 2, 0);          // 2. anzeigen

    tele[0] = /* z. B. Lenkwinkel      */ 0;
    tele[1] = /* z. B. Flankenzahl, E4 */ 0;
    driverComSend(tele, 2);                 // 3. senden

    delayMsBlocked(100);                    // rund zehn Rahmen je Sekunde
}
```

Die Wartezeit am Ende ist nötig: Ohne sie versucht die Schleife tausende Male pro Sekunde zu senden. `driverComSend()` liefert dann `0` zurück, solange der Sender beschäftigt ist – kaputt geht nichts, aber die CPU dreht leer und die Anzeige flackert.

> **In den Serviceroutinen schreiben Sie nichts.** Die Empfangs-ISR des UART sammelt den Rahmen selbsttätig ein, sobald `driverComInit()` ihr über `halComStart()` Sync-Muster und Rahmenlänge mitgeteilt hat; die SPI-ISR schiebt die Displaybytes nach. Ihr Code fragt nur ab und stößt an.

---

## Steuerleitungen des Displays

Das Display hängt am SPI, aber zwei seiner Leitungen gehören nicht dazu. Beide sind gewöhnliche GPIO-Ausgänge, und beide hat bisher niemand eingerichtet:

| Leitung | Aufgabe | wann geschaltet |
|---|---|---|
| **A0** (im Datenblatt auch D/C) | unterscheidet Befehl von Bilddaten | **vor** jedem Senden |
| **Reset** | Hardware-Reset des Controllers | einmal beim Start |

In Einheit 5 haben Sie das Chip Select auf demselben Weg angelegt. Das vorgegebene `driverLCD.c` ruft die drei Schaltfunktionen bereits auf – ohne sie lässt es sich nicht übersetzen. Deshalb steht diese Aufgabe am Anfang.

### [AUFGABE] A0- und Reset-Leitung setzen

1. **Pins ermitteln** und das Ergebnis in die Tabelle aus Einheit 5 nachtragen:

   | Signal | Pin | Sonderfunktion über | Richtung |
   |---|---|---|---|
   | LCD A0 (Data/Command) | | gewöhnlicher I/O | Ausgang |
   | LCD Reset | | gewöhnlicher I/O | Ausgang |

2. **In `halGPIO.h` benennen**, nach dem Muster der Makros aus Einheit 2:

   ```c
   #define LCD_DATACMD     BITx        // A0  – Befehl oder Bilddaten
   #define LCD_RESET_PIN   BITx        // Hardware-Reset des Controllers
   ```

3. **In `halGPIOInit()` als Ausgang konfigurieren.** Beide brauchen ein gesetztes `PxDIR`-Bit und **kein** `PxSEL` – es sind einfache Ausgänge, keine Sonderfunktion.

4. **Drei Funktionen in `halGPIO.c` schreiben**, Prototypen in `halGPIO.h`:

   ```c
   void halSetLCDCmd(void);                 // A0 = 0 -> die folgenden Bytes sind Befehle
   void halSetLCDData(void);                // A0 = 1 -> die folgenden Bytes sind Bildpunkte
   void halSetLCDReset(bool lcdResetOn);    // true = Reset aktiv, false = Reset gelöst
   ```

   Für `bool` muss `<stdbool.h>` in `halGPIO.h` eingebunden sein.

   > **Achtung:** Die Reset-Leitung ist **aktiv LOW**. `halSetLCDReset(true)` muss den Pin auf 0 ziehen, nicht auf 1. Gehen Sie nach dem Schaltplan, nicht nach dem Funktionsnamen.

5. **Prüfen Sie das Ergebnis im Debugger**, bevor Sie weitermachen: Nach `halGPIOInit()` müssen beide Bits in `PxDIR` stehen. Rufen Sie `halSetLCDData()` und `halSetLCDCmd()` nacheinander auf und beobachten Sie das Bit in `PxOUT`. Ein falsch gewählter Pin erzeugt später ein Fehlerbild, das wie ein Fehler im Displaytreiber aussieht – und dort suchen Sie dann vergeblich.

6. **[Meilensteinüberprüfung]** Warum gehören diese beiden Leitungen in den HAL und nicht in `driverLCD.c`, obwohl sie ausschließlich vom Display gebraucht werden?

---

## Displaytreiber vervollständigen

Der Controller kennt keine Zeichen, keine Zahlen und keine Zeilen – er kennt Bytes im Bildspeicher. Alles andere macht Ihr Treiber.

Vorgegeben ist, was im Datenblatt steht und nur abgeschrieben werden könnte: der Weg eines Bytes zum Display, die Initialisierungssequenz, die Aufteilung der Spaltenadresse und die Fonttabelle.

```c
void driverLCDWriteCommand(const unsigned char *data, unsigned char dataLength);  // A0 = 0
void driverLCDWriteData   (const unsigned char *data, unsigned char dataLength);  // A0 = 1
void driverLCDSetPosition (unsigned char page, unsigned char col);                // Cursor
```

Die ersten beiden warten intern auf das Ende der vorigen SPI-Übertragung, schalten A0 um und stoßen `halUSCIB1Transmit()` aus Einheit 5 an; das Chip Select wird dort gesetzt, nicht hier.

> Bildspeicher, A0-Leitung und Fonttabelle: ➡ **[Das Display am SPI](Display.md)**

### [AUFGABE] driverLCD – Text und Zahlen ausgeben

1. **Dateien ins Projekt übernehmen:** `driverFonttable.h`, `driverLCD.h` und `driverLCD.c` nach `DL/`, danach **F5**.

2. **`driverLCDClear(void)` schreiben** – TODO 1 in der Datei.
   - Alle Seiten nacheinander anfahren, je Seite `LCD_MAX_COLM` Bytes aus `emptyPage` senden
   - Ein Aufruf von `driverLCDWriteData()` je Seite genügt – der Spaltenzähler rückt selbsttätig weiter
   - `LCD_MAX_ROW` ist die Nummer der **letzten** Seite, nicht die Anzahl der Seiten

   `driverLCDInit()` ruft diese Funktion bereits auf. Sobald sie steht, ist die Anzeige nach dem Start leer statt zufällig gefüllt – der erste sichtbare Erfolg.

3. **`driverLCDWriteText()` schreiben** – TODO 2.
   - Cursor setzen, dann je Zeichen die sechs Spalten aus `font_table[]` als Bilddaten senden
   - Die Zeichenbreite steht als `fonts_width_max` im Fontheader, `col` führen Sie selbst mit

   Drei Fälle sind abzufangen:

   | Fall | Reaktion |
   |---|---|
   | Zeichen passt nicht mehr in die Zeile | nächste Seite, Spalte 0, Cursor neu setzen |
   | letzte Seite ist voll | abbrechen |
   | Zeichen nicht in der Tabelle | Ersatzzeichen, z. B. `'?'` |

   `char` ist auf dem MSP430 vorzeichenbehaftet – ein Zeichen über 127 ergibt sonst einen negativen Index.

4. **`driverLCDWriteUInt()` schreiben** – TODO 3.
   - `number % 10 + '0'` liefert die **letzte** Ziffer; füllen Sie einen Puffer von hinten nach vorne
   - Ein `unsigned int` hat höchstens fünf Stellen
   - Ausgeben über `driverLCDWriteText()`, nicht über eine eigene Zeichenschleife
   - Der Fall `number == 0` braucht eine eigene Behandlung – eine abweisende Schleife läuft dort kein einziges Mal

5. **`driverLCDInit()` in `driverInit()` eintragen** und einen Startbildschirm ausgeben: Teamname und eine Zahl, an einer Position Ihrer Wahl.

6. **[Meilensteinüberprüfung]** Sie schreiben 128 Bytes in eine Seite, ohne die Position dazwischen neu zu setzen. Warum funktioniert das, und was passiert beim 129. Byte?

7. **[Meilensteinüberprüfung]** Warum ist `font_table` als `[128][6]` angelegt und nicht als `[6][128]`? Was würde die andere Reihenfolge beim Senden kosten?

---

## Kommunikation in Betrieb nehmen

Ein UART überträgt einen Strom von Bytes ohne Anfang und Ende. Damit daraus Telemetrie wird, braucht es einen Rahmen – und auf beiden Seiten dieselbe Festlegung, in welcher Reihenfolge die Bytes eines 16-Bit-Werts gehen.

Vorgegeben sind der Header mit den Rahmenlängen und der Umschaltung, die Initialisierung und die Gegenstelle am PC. Ihre Arbeit sind die beiden Funktionen, die den Rahmen packen und auspacken:

```c
unsigned char halComSend(const unsigned char *data, unsigned char length);  // 0 = Sender belegt
unsigned char halComTxBusy(void);                                          // 1 = sendet gerade
unsigned char halComGetFrame(unsigned char *dest);                         // 0 = kein Rahmen da
```

> **Achtung:** `halComSend()` merkt sich den **Zeiger** auf Ihren Puffer, nicht dessen Inhalt – dieselbe Regel wie bei den Modulen aus Einheit 5. Wer `comTxFrame` überschreibt, während noch gesendet wird, verschickt Datenmüll.

> Rahmenformat, Umschaltung und Gegenstelle: ➡ **[Kommunikation mit dem Fahrzeug](Kommunikation.md)**

### [AUFGABE] driverCom – Rahmenformat implementieren

1. **Dateien ins Projekt übernehmen:** `driverCom.h` und `driverCom.c` nach `DL/`, danach **F5**.

   `halUSCIA1.c/.h` liegt noch einmal bei, ist aber identisch mit Einheit 5. Haben Sie es dort bereits eingebunden, überspringen Sie das – ein zweites Exemplar führt zu doppelten Symbolen beim Linken.

2. **`driverComInit()` in `driverInit()` aufrufen.** Die Funktion ist vorgegeben; sehen Sie sich an, was `halComStart()` mit dem Sync-Muster macht. Ab diesem Aufruf sucht die Empfangs-ISR nach `'H'`, `'S'`.

3. **`driverComSend()` schreiben** – TODO 1 bis 4.
   - Läuft noch eine Übertragung, nichts tun und `0` zurückgeben
   - Mehr als `COM_TX_MAX_VAL` Werte auf das Maximum begrenzen
   - Rahmen in `comTxFrame[]` aufbauen: `'C'`, `'S'`, je Wert zwei Bytes, `'E'`, `'C'`
   - `n` mitzählen – es ist am Ende die Rahmenlänge für `halComSend()`

   Die Reihenfolge der beiden Bytes je Wert ist festgelegt; `crazyCarCom.m` setzt sie genau so wieder zusammen.

4. **`driverComReceive()` schreiben** – TODO 5 bis 7.
   - Rahmen mit `halComGetFrame()` abholen; liefert sie `0`, ist keiner da
   - **Rahmenende prüfen** – `frame[5]` und `frame[6]` müssen `'E'` und `'H'` sein
   - Bei ungültigem Rahmen `0` zurückgeben und `*cmd` und `*value` **nicht** anfassen
   - Sonst Kommando und Wert über die `COM_RX_…`-Indizes auspacken und `1` zurückgeben

   Auf den Rahmenanfang müssen Sie nicht prüfen, darauf hat der HAL bereits synchronisiert. Warum die Prüfung des Rahmenendes trotzdem nötig ist, steht im [Unterkapitel](Kommunikation.md#die-beiden-rahmenformate).

5. **Hauptschleife aufbauen** und zwei Werte zyklisch senden – zum Beispiel Lenkwinkel und die Flankenzahl Ihrer Drehzahlmessung aus Einheit 4.

6. **Gegenstelle starten.** `crazyCarCom.m` öffnen und `PORT`, `BAUD` und `NUM_VALS` anpassen. `NUM_VALS` muss zu Ihrem `count` passen, sonst verwirft das Skript stillschweigend jeden Rahmen.

7. **[Meilensteinüberprüfung]** Was passiert, wenn ein Sensorwert zufällig die Bytes `'E'` und `'C'` enthält? Beschreiben Sie das Fehlerbild und einen Weg, es zu vermeiden.

8. **[Meilensteinüberprüfung]** Die Umschaltung zwischen USB und Bluetooth kostet zur Laufzeit nichts. Warum, und welchen Preis zahlt man dafür?

### [AUFGABE] Fernsteuerung und Telemetrie

> **Fahrzeug aufbocken.** Die Räder müssen frei drehen können.

Natürlich ist klar, dass wir das CrazyCar grundsätzlich nicht mit dem PC fernsteuern möchten, jedoch als Test passt es sehr gut. 

1. **Kommandotabelle festlegen.** Welche Nummer was bedeutet, entscheiden Sie – aber schreiben Sie es auf, die Gegenstelle braucht dieselbe Tabelle:

   | `cmd` | Bedeutung | Wertebereich |
   |---|---|---|
   | `1` | Lenkwinkel | Ihre Festlegung |
   | `2` | Gas | Ihre Festlegung |

2. **Kommandos auswerten.** Im `if`-Zweig der Hauptschleife die Kommandonummer auf `driverSetSteering()` oder `driverSetThrottle()` abbilden.

   > Der Wert kommt als `unsigned int` an, die beiden Funktionen erwarten `signed char` von −100 bis +100. Wie schickt der PC eine −50 durch ein vorzeichenloses 16-Bit-Feld? Legen Sie die Codierung fest und halten Sie sie in Ihrer Kommandotabelle fest.

3. **Vom PC senden.** Im unteren Teil von `crazyCarCom.m` steht der Kommandorahmen als eigener Abschnitt; Nummer und Wert eintragen und den Abschnitt einzeln ausführen.

4. **Auf dem Display mitschreiben.** Zuletzt empfangenes Kommando und seinen Wert anzeigen. Damit sehen Sie ohne Debugger, ob das Fahrzeug verstanden hat, was gemeint war.

5. **[Meilensteinüberprüfung]** Was passiert bei einem Wert außerhalb des zulässigen Bereichs – und an welcher Stelle Ihrer Schichten gehört die Prüfung hin?

**Wenn Zeit bleibt**, zwei Erweiterungen:

- **Ein Balken statt einer Zahl.** Der Lenkwinkel als Balken über eine Displayseite: Jedes Byte ist eine Spalte, `0xFF` ist voll, `0x18` ein dünner Strich in der Mitte. Dafür braucht es keine Fonttabelle, nur `driverLCDWriteData()`.
- **Ein Nothalt.** Ein eigenes Kommando, das `driverSetThrottle(0)` auslöst, und ein Zeichen auf dem Display, solange es aktiv ist.

---

## Verifikation mit dem Analog Discovery

In Einheit 5 haben Sie nachgewiesen, dass die Schnittstellen korrekte Bytes erzeugen. Jetzt prüfen Sie Ihr **eigenes Protokoll**: ob auf der Leitung genau der Rahmen steht, den Ihr Code bauen wollte, und ob die Umschaltung auf Bluetooth tatsächlich die Leitung wechselt.

> Anschluss, Pinbelegung und Dekoder-Einstellungen stehen im Unterkapitel von Einheit 5:
> ➡ **[Messen mit dem Analog Discovery](../Kapitel_05_Schnittstellen/AnalogDiscovery.md)**
>
> Die Schritte unten sagen, *was* zu messen ist; die kursiven Zeilen nennen, **wo** die Einstellung in WaveForms liegt.

### [AUFGABE] Messen am Analog Discovery

**Telemetrie über USB**

1. Kanal auf die TX-Leitung von UCA0 legen, UART-Dekoder auf 57600 Baud, 8N1. Zeichnen Sie **einen** Telemetrierahmen auf. Im Mitschnitt müssen `'C'`, `'S'`, Ihre Werte und `'E'`, `'C'` als Bytes erkennbar sein.

   *In WaveForms:* **Logic** → **+ (Add)** → *Protocol* → **UART**. Trigger auf die **fallende Flanke** der TX-Leitung, dann **Single** – ein stehendes Bild lässt sich vermessen.

2. Vergleichen Sie die Bytefolge mit dem, was `crazyCarCom.m` anzeigt. Rechnen Sie nach: Wie viele Bytes hat Ihr Rahmen bei `NUM_VALS` Werten, und wie lange dauert seine Übertragung bei 57600 Baud?

**Umschaltung auf Bluetooth**

3. `COM_USE_BT` in `driverCom.h` auf `1` setzen, neu bauen, flashen. **In `driverCom.c` darf sich keine Zeile ändern** – das ist der eigentliche Nachweis dieser Aufgabe.

4. Legen Sie den Kanal auf die TX-Leitung von UCA1 und stellen Sie den Dekoder auf 9600 Baud um. Zeichnen Sie denselben Rahmen noch einmal auf.

   Messen Sie in beiden Aufzeichnungen die Dauer eines einzelnen Bits. Das Verhältnis muss zu den beiden Baudraten passen – rechnen Sie nach.

**Kommandorahmen in der Gegenrichtung**

5. Betreiben Sie das Analog Discovery unter *Protocol → UART* und schicken Sie einen Kommandorahmen an das Fahrzeug: `'H'`, `'S'`, Kommandonummer, zwei Wertbytes, `'E'`, `'H'`. Prüfen Sie im Debugger, dass `driverComReceive()` eine 1 liefert und die richtigen Werte ausgibt.

   *In WaveForms:* Logic-Fenster schließen. **Protocol** → Reiter **UART** → **Send & Receive**, Baudrate passend zur aktuellen Einstellung von `COM_USE_BT`.

6. Schicken Sie anschließend einen Rahmen mit **falschem Rahmenende**. `driverComReceive()` muss eine 0 liefern und Kommando und Wert unverändert lassen.

**Befehl oder Bilddaten am Display**

7. Legen Sie Kanäle auf CLK, MOSI, CS und zusätzlich auf die **A0-Leitung**. SPI-Dekoder wie in Einheit 5 einstellen und einen Displayzugriff aufzeichnen.

   Im Bild müssen die drei Kommandobytes des Cursors und die darauffolgenden Bilddaten unterscheidbar sein – allein am Pegel der A0-Leitung. Damit ist gezeigt, dass Ihre Schaltfunktionen aus der ersten Aufgabe zum richtigen Zeitpunkt umschalten.

**Zu dokumentieren**

8. Je ein Screenshot: Telemetrierahmen über USB, derselbe über Bluetooth, Kommandorahmen in der Gegenrichtung, Displayzugriff mit A0-Leitung.
9. **[Meilensteinüberprüfung]** Im Mitschnitt stehen zwei Bytes je Wert. Woran erkennen Sie, welches das höherwertige ist – ohne in Ihren Code zu sehen?
10. **[Meilensteinüberprüfung]** Die Umschaltung hat die Leitung gewechselt, nicht das Protokoll. Woran ist das in den beiden Aufzeichnungen zu erkennen?

---

## Testing

In dieser Einheit lassen sich die Treiber erstmals ohne Hardware prüfen:

➡ **[Testing – Display und Kommunikation](Testing/Testing.md)**

## Referenzen

- **ST7565 65 × 132 Dot Matrix LCD Controller**, Sitronix, Version 1.0a.  
  Verfügbar unter: [https://www.hpinfotech.ro/ST7565.pdf](https://www.hpinfotech.ro/ST7565.pdf)

- **MSP430x5xx and MSP430x6xx Family User Guide**, Texas Instruments, Literature Number: SLAU208O, Rev. O, April 2019.  
  Verfügbar unter: [https://www.ti.com/lit/pdf/slau208](https://www.ti.com/lit/pdf/slau208)

- **MSP430F5335 Datasheet**, Texas Instruments, Document Number: SLAS590N, Rev. N, October 2018.  
  Verfügbar unter: [https://www.ti.com/lit/gpn/msp430f5335](https://www.ti.com/lit/gpn/msp430f5335)

[⬆ Zurück zum Hauptverzeichnis](../README.md#einheitsübersicht--aufgabenstellungen)
