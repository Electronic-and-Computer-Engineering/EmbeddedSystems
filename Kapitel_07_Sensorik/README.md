[⬅ Zurück zur Kapitelübersicht](../README.md#einheitsübersicht--aufgabenstellungen)
# Abstandssensoren, ADC und DMA

## Inhalt
- [Die Unterkapitel](#die-unterkapitel)
- [Messen, ohne die CPU zu beschäftigen](#messen-ohne-die-cpu-zu-beschäftigen)
- [Der Takt für die Abtastung](#der-takt-für-die-abtastung)
- [Den ADC12 selbst aufsetzen](#den-adc12_a-selbst-aufsetzen)
- [Die Erfassung auf DMA umstellen](#die-erfassung-auf-dma-umstellen)
- [Die Batteriespannung im Driver Layer](#die-batteriespannung-im-driver-layer)
- [Die Abstandssensoren am I2C](#die-abstandssensoren-am-i2c)
- [Verifikation im Debugger](#verifikation-im-debugger)
- [Testing](#testing)

**Zu erreichende Ziele**

Am Ende dieser Einheit misst das Fahrzeug selbsttätig: vier analoge Kanäle im festen
Takt, drei Abstandssensoren über I2C – und die CPU ist daran nicht beteiligt.

- [ ] `TB0.1` erzeugt einen Takt von 120 Hz als Auslöser für den ADC.
- [ ] `halADC12.c/.h` ist selbst geschrieben: vier Kanäle als Sequenz, interne Referenz 2,5 V, 12 Bit, ausgelöst vom Timer.
- [ ] Die vier Analogpins sind in `halGPIOInit()` auf Analogfunktion geschaltet.
- [ ] `halDMAInit()` ist geschrieben: Blockübertragung aus `ADC12MEM0` nach `ADCBuffer`, ausgelöst vom ADC.
- [ ] Nach dem Umbau auf DMA läuft im `ADC12_VECTOR` **keine** Serviceroutine mehr.
- [ ] `driverSensorsGetBatteryMv()` liefert die Batteriespannung in Millivolt, gegen das Multimeter geprüft.
- [ ] Die drei XSHUT-Leitungen sind im HAL schaltbar; jeder ToF-Sensor hat eine eigene I2C-Adresse.
- [ ] Alle drei ToF-Abstände stehen im Debugger und ändern sich plausibel, wenn man die Hand davorhält.
- [ ] Das Verhalten der Sharp-Kennlinie ist aufgenommen: ungleichmäßiger Anstieg, Mehrdeutigkeit unter 20 cm, Unterschied zwischen zwei Exemplaren.

**Tests:** DL-Tests für die Umrechnung – reine Rechnerei, ohne angeschlossenen Akku prüfbar.

**Laborübung**

- *MSP430x5xx and MSP430x6xx Family User Guide Rev. O* – Texas Instruments
  - Kapitel 28: [ADC12_A](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=737)
  - Kapitel 11: [DMA Controller](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=349)
- [VL53L1X Datasheet](https://www.st.com/resource/en/datasheet/vl53l1x.pdf) · [UM2356 – VL53L1X Ultra Lite Driver](https://www.st.com/resource/en/user_manual/um2356-vl53l1x-api-user-manual-stmicroelectronics.pdf)
- Crazy Car Controller FHJ Schaltplan [Crazy Car Schematic](https://fhjoanneum-my.sharepoint.com/:b:/g/personal/florian_mayer_fh-joanneum_at/EfXYu-rqsLRErJbybsbN4AEB_RUMizJhwpb5D_ysimZehA?e=Ti7PtO)

**Wissensüberprüfung**

- Recherche:
  - Wie arbeitet ein SAR-Wandler, und warum braucht er eine Sample-and-Hold-Stufe?
  - Was bedeutet die Auflösung von 12 Bit für den kleinsten unterscheidbaren Spannungsschritt bei 2,5 V Referenz?
  - Was ist Direct Memory Access, und wovon wird die CPU dabei genau entlastet?
  - Warum haben mehrere gleiche I2C-Teilnehmer ein Problem, und wie löst eine XSHUT-Leitung es?
  - Wie verhält sich die Kennlinie eines Sharp-Abstandssensors, und warum lässt sich daraus nicht mit einer Formel auf die Entfernung schließen?
- Arbeiten Sie die drei Unterkapitel durch – siehe unten.

**Video**
 - *folgt*

### Durchzuführende Aufgaben
- [[AUFGABE] TimerB0 als Auslöser einrichten](#aufgabe-timerb0-als-auslöser-einrichten)
- [[AUFGABE] halADC12 implementieren](#aufgabe-haladc12-implementieren)
- [[AUFGABE] halDMA in Betrieb nehmen](#aufgabe-haldma-in-betrieb-nehmen)
- [[AUFGABE] Batteriespannung im Driver Layer](#aufgabe-batteriespannung-im-driver-layer)
- [[AUFGABE] ToF-Sensoren einbinden](#aufgabe-tof-sensoren-einbinden)
- [[AUFGABE] Messwerte im Debugger nachweisen](#aufgabe-messwerte-im-debugger-nachweisen)

---

Bisher hat das Fahrzeug nur ausgegeben: Pulse an Servo und ESC, Bytes ans Display, Rahmen an den PC. In dieser Einheit nimmt es zum ersten Mal die Umgebung auf.

Dafür gibt es zwei ganz unterschiedliche Wege. Die Sharp-Abstandssensoren und die Batteriespannung liefern eine **Spannung**, die gewandelt werden muss – das macht der ADC12_A. Die VL53L1X-Abstandssensoren liefern eine **fertige Zahl in Millimetern** über I2C, dafür muss man sich mit einer fremden Bibliothek und mit drei identischen Teilnehmern an einem Bus auseinandersetzen.

| Quelle | Modul | Weg ins Programm | Ihre Aufgabe |
|---|---|---|---|
| Sharp-Abstand, Batterie | `halADC12` | Spannung → 12-Bit-Zahl | selbst implementieren |
| dieselben Werte, ohne CPU | `halDMA` | Register → Speicher | Serviceroutine vorgegeben, Init selbst |
| Umrechnung in Volt | `driverSensors` | Zahl → Millivolt | selbst implementieren |
| ToF-Abstand | `driverTof` + ULD-API | I2C → Millimeter | einbinden, Adressvergabe verstehen |

---

## Die Unterkapitel

| | Inhalt |
|---|---|
| ➡ **[Der ADC12_A am MSP430](ADC12.md)** | SAR-Prinzip, Sample and Hold, Referenz, Sequenzbetrieb, Triggerquellen, Kennlinie der Sharp-Sensoren |
| ➡ **[Direct Memory Access](DMA.md)** | Warum DMA, Blockübertragung, Trigger, die Falle mit `ADC12ENC` |
| ➡ **[Die ToF-Sensoren](ToF.md)** | Laufzeitmessung, die ULD-API, Adressvergabe über XSHUT, Plattformschicht |

Arbeiten Sie das jeweilige Unterkapitel vor der zugehörigen Aufgabe durch. Die Aufgabenstellungen unten nennen, *was* einzustellen ist; *warum* steht im Unterkapitel.

---

## Messen, ohne die CPU zu beschäftigen

Vier Kanäle, 120 Messungen je Sekunde, das sind 480 Werte in der Sekunde. Man könnte sie abholen, indem man nachfragt – in der Hauptschleife, oder in einer Serviceroutine, die bei jedem fertigen Wert anspringt.

Beides wäre falsch, und zwar aus einem Grund, der in dieser Einheit zum ersten Mal zählt: **Die CPU wird gleich für etwas anderes gebraucht.** In Einheit 8 kommt FreeRTOS, in Einheit 9 der Fahralgorithmus. Was jetzt unnötig Rechenzeit frisst, fehlt dort.

Deshalb wird der Messpfad in dieser Einheit so gebaut, dass die CPU **gar nicht vorkommt**:

```
  TB0.1  alle 8,3 ms ein Impuls
    |
    v
  ADC12  wandelt vier Kanäle nacheinander      <- ohne Zutun der CPU
    |      Ergebnisse in ADC12MEM0 .. MEM3
    v
  DMA    kopiert die vier Woerter in den RAM   <- ohne Zutun der CPU
    |
    v
  DMA-ISR    mittelt, setzt ADCrdy             <- die einzige Stelle, an der
                                                  die CPU kurz arbeitet
```

Die CPU läuft erst wieder an, wenn ein **ganzer Satz** fertig ist – statt 480 mal je Sekunde nur noch 120 mal, und dann für ein paar Dutzend Takte.

Jedes dieser drei Stücke lässt sich einzeln im Debugger nachweisen, bevor das nächste dazukommt – und genau so gehen Sie es an.

| Schicht | Datei | was dazukommt |
|---|---|---|
| **HAL** | `halTimerB0.c` | Ausgangssignal auf `TB0.1`, 120 Hz |
| **HAL** | `halGPIO.c/.h` | vier Analogpins, drei XSHUT-Leitungen |
| **HAL** | `halADC12.c/.h` | der Wandler – **ganz Ihre Arbeit** |
| **HAL** | `halDMA.c` | nur `halDMAInit()`, die Serviceroutine ist vorgegeben |
| **DL** | `driverSensors.c/.h` | Rohwert → Millivolt |
| **DL** | `driverTof.c/.h` | vorgegeben, nur einbinden |

---

## Der Takt für die Abtastung

Der ADC soll nicht losmessen, wann die Hauptschleife gerade Zeit hat, sondern in **festen Abständen**. Nur dann sind die Werte miteinander vergleichbar, und nur dann ergibt eine Mittelung über zwei Messungen überhaupt Sinn.

120 Hz sind dabei kein Zufall: Die Sharp-Sensoren liefern selbst nur rund 25 bis 40 neue Werte je Sekunde. Mit 120 Hz tasten Sie jeden davon mehrfach ab – genug, um zu mitteln, ohne Rechenzeit zu verschwenden.

Den Takt liefert der TimerB0, den Sie in Einheit 3 aufgesetzt haben. Er bekommt jetzt eine zweite Aufgabe: Statt nur zu zählen, erzeugt er auf dem Vergleichskanal `TB0.1` ein Ausgangssignal, und dessen Flanke startet die ADC-Sequenz.

### [AUFGABE] TimerB0 als Auslöser einrichten

1. **Periode auf 120 Hz bringen.** Rechnen Sie den Zählerendwert aus: SMCLK mit 2,5 MHz, Teiler 8 und noch einmal 8 im Erweiterungsregister. Welcher Wert muss in `TB0CCR0`, damit sich 8,3 ms ergeben?

   Schreiben Sie das Ergebnis als `#define` mit nachvollziehbarer Rechnung in den Header, nicht als Zahl in den Code.

2. **Kanal 1 als Ausgang betreiben.** `TB0CCTL1` auf `OUTMOD_7` stellen und `TB0CCR1` auf einen kleinen Wert setzen. Damit entsteht pro Zählerdurchlauf eine Flanke, und mehr braucht der ADC nicht.

3. **[Meilensteinüberprüfung]** Warum genügt dem ADC eine Flanke, und warum muss `TB0CCR1` kleiner als `TB0CCR0` sein?

Der Timer-Testfall aus Einheit 3 prüft die alte Periode und schlägt danach fehl. Passen Sie ihn an.

---

## Den ADC12_A selbst aufsetzen

Dies ist das Modul, das Sie in dieser Einheit vollständig selbst schreiben. Es ist das umfangreichste bisher, aber es folgt demselben Muster wie alle HAL-Module davor: eine Initialisierung, eine Abfrage, und dazwischen arbeitet die Hardware allein.

> Grundlagen, Register und Triggerquellen: ➡ **[Der ADC12_A am MSP430](ADC12.md)**

### [AUFGABE] halADC12 implementieren

1. **Analogpins schalten.** Die vier Eingänge liegen auf Port 6. Suchen Sie im Schaltplan, welcher Pin welches Signal trägt, und tragen Sie es ein:

   | Kanal | Signal | Pin |
   |---|---|---|
   | A0 | Abstand rechts | |
   | A1 | Abstand links | |
   | A2 | Abstand vorne | |
   | A3 | Batteriespannung | |

   Für einen Analogeingang genügt das `PxSEL`-Bit – eine Richtung braucht er nicht. Die Sharp-Sensoren haben zusätzlich **Enable-Leitungen**; prüfen Sie im Schaltplan, welche das sind und wie sie stehen müssen.

2. **`halADC12.c` anlegen.** Der Header `halADC12.h` ist vorgegeben und enthält die Datenstruktur `ADC12Com` – sie ist der Vertrag zwischen ADC, DMA und Driver Layer und wird nicht verändert.

3. **Globale Variable `sensor_Data` vom Typ `ADC12Com` anlegen**, in `halGeneral.c`, nach dem Muster von `LCD_Data` aus Einheit 5. `volatile` nicht vergessen – die DMA-ISR schreibt darauf.

4. **`halADC12Init()` schreiben.** Einzustellen sind:

   | | Einstellung | Register |
   |---|---|---|
   | Referenz | intern, 2,5 V, eingeschaltet | `REFCTL0` |
   | Sample and Hold | lange Haltezeit, für **beide** Felder `ADC12SHT0` und `ADC12SHT1` | `ADC12CTL0` |
   | Mehrfachwandlung | nach einer Wandlung folgt die nächste ohne neuen Impuls | `ADC12CTL0` |
   | Auslöser | Ausgang des TimerB0, Kanal 1 | `ADC12CTL1`, Feld `ADC12SHS` |
   | Taktquelle und Teiler | SMCLK, durch 2 | `ADC12CTL1` |
   | Betriebsart | Sequenz von Kanälen | `ADC12CTL1`, Feld `ADC12CONSEQ` |
   | Auflösung | 12 Bit | `ADC12CTL2` |
   | Kanalzuordnung | A0 … A3 auf `ADC12MCTL0 … 3`, jeweils mit der Referenz | `ADC12MCTLx` |
   | Sequenzende | auf dem **letzten** Kanal markieren | `ADC12MCTL3`, Bit `ADC12EOS` |

   Zwei Regeln, die Sie Zeit kosten, wenn Sie sie nicht kennen:

   - Die meisten Felder lassen sich nur beschreiben, solange `ADC12ENC` **gelöscht** ist. Erst ganz am Ende einschalten.
   - Vergessen Sie das Sequenzende, läuft der Wandler über alle 16 Speicherregister weiter und Ihre vier Werte stehen woanders.

5. **`halADC12GetValue()` schreiben.** Gibt den gefilterten Wert eines Kanals zurück, mit Bereichsprüfung. Drei Zeilen – aber sie sind der einzige Weg, auf dem der Driver Layer an die Messwerte kommt.

6. **Aufruf in `halInit()` eintragen**, nach `halTimerB0Init()`.

7. **Zwischenstand prüfen.** Noch gibt es keine DMA. Setzen Sie einen Haltepunkt und sehen Sie sich `ADC12MEM0` bis `ADC12MEM3` im Debugger an. Dort müssen vier plausible Werte stehen.

   > **Sie sehen genau eine Sequenz, danach ändert sich nichts mehr.** Das ist kein Fehler Ihrer Konfiguration. In der Betriebsart „Sequenz von Kanälen" betrachtet der Wandler die Sequenz als erledigt und muss für die nächste neu freigegeben werden – `ADC12ENC` aus und wieder ein. Wer das übernimmt, klärt die nächste Aufgabe. Für den Zwischenstand genügt: vier Werte, plausible Größenordnung, und wenn Sie das Programm neu starten, passen sie zur Hand vor dem Sensor.

   Erst wenn das steht, geht es weiter. Eine DMA auf einen ADC zu setzen, der nicht misst, ist Fehlersuche an zwei Stellen gleichzeitig.

8. **[Meilensteinüberprüfung]** Welchen Spannungswert in Millivolt stellt der Rohwert 2048 dar? Und welcher Rohwert entspräche 1,0 V?

9. **[Meilensteinüberprüfung]** Was passiert, wenn an einem Analogeingang mehr als die Referenzspannung anliegt? Sehen Sie im Datenblatt nach, welche Spannung der Pin überhaupt verträgt.

---

## Die Erfassung auf DMA umstellen

Bis hierher stehen die Werte in den Registern des ADC – und sie werden dort beim nächsten Durchlauf überschrieben. Jemand muss sie abholen. Dieser Jemand soll nicht die CPU sein.

> Prinzip, Register und die Falle mit `ADC12ENC`: ➡ **[Direct Memory Access](DMA.md)**

### [AUFGABE] halDMA in Betrieb nehmen

1. **Dateien ins Projekt übernehmen:** `halDMA.c` und `halDMA.h` nach `HAL/`, danach **F5**. Die Serviceroutine ist fertig, `halDMAInit()` ist Ihre Arbeit – TODO 1 bis 7.

2. **Die Serviceroutine zuerst lesen.** Sie steht unter Ihrer Arbeit in derselben Datei und erklärt zwei Dinge, die Sie sonst suchen: warum gemittelt wird und warum `ADC12ENC` aus- und wieder eingeschaltet werden muss.

3. **`halDMAInit()` schreiben**, entlang der TODOs. Einzustellen sind Transfermodus, Adressinkremente, Auslöser, Quell- und Zieladresse, Blockgröße und zum Schluss die Freigabe.

4. **Aufruf in `halInit()` eintragen**, **nach** `halADC12Init()`.

5. **Eine Serviceroutine wird überflüssig.** Falls Sie beim Zwischenstand eine ISR für den `ADC12_VECTOR` angelegt haben, kommentieren Sie sie aus – ab jetzt holt die DMA die Werte ab, und die ADC-Flagge dient nur noch als Auslöser.

6. **[Meilensteinüberprüfung]** Die DMA kopiert, während das Programm weiterläuft. Was kann passieren, wenn Ihre Hauptschleife `ADCBuffer` genau in dem Moment liest, in dem die DMA ihn beschreibt? Und warum ist der Zugriff über `halADC12GetValue()` auf `ADCFiltered` unkritisch – obwohl auch dieses Array von einer Unterbrechung beschrieben wird?

7. **[Meilensteinüberprüfung]** Die Serviceroutine mittelt mit `(alt + neu) >> 1`, halbiert den Einfluss eines Werts also mit jeder Messung. Nach wie vielen Messungen ist von einem alten Wert weniger als 1 % übrig, und wie lange dauert das bei 120 Hz? Was bedeutet diese Verzögerung für ein Hindernis, das plötzlich auftaucht?

---

## Die Batteriespannung im Driver Layer

Der ADC liefert eine Zahl zwischen 0 und 4095. Was sie bedeutet, weiß nur der Driver Layer – und genau dort endet die Welt der Zahlen ohne Einheit.

Diese Aufgabe steht bewusst **nach** der DMA: `halADC12GetValue()` liefert den gemittelten Wert aus `ADCFiltered`, und den füllt die DMA-Serviceroutine. Ohne laufende DMA bekämen Sie hier beharrlich eine 0 zurück und würden den Fehler in Ihrer Umrechnung suchen.

### [AUFGABE] Batteriespannung im Driver Layer

1. **Dateien ins Projekt übernehmen:** `driverSensors.c` und `driverSensors.h` nach `DL/`, danach **F5**.

2. **Spannungsteiler ablesen.** Die Batterie liefert mehr Spannung, als der ADC verträgt, deshalb liegt ein Teiler davor. Suchen Sie die beiden Widerstände im Schaltplan und tragen Sie das Verhältnis als `VBAT_DIVIDER_NUM` und `VBAT_DIVIDER_DEN` in `driverSensors.h` ein.

3. **`driverSensorsGetBatteryMv()` schreiben** – TODO 1.

   Zwei Schritte: Rohwert in die Spannung am Pin, Spannung am Pin in die Batteriespannung.

   > **Achtung beim Rechnen.** `4095 * 2500` ergibt über zehn Millionen und passt nicht in einen `unsigned int`. Entweder Sie rechnen in `unsigned long`, oder Sie teilen früher – beides ist vertretbar, aber schreiben Sie dazu, welchen Weg Sie gewählt haben und welchen Rundungsfehler er kostet.

4. **`driverSensorsBatteryLow()` schreiben** – TODO 2. Eine Zeile, die aber hierher gehört und nicht in den Application Layer.

5. **Gegen das Multimeter prüfen.** Messen Sie die Akkuspannung nach und vergleichen Sie mit Ihrem Wert im Debugger. Eine Abweichung von ein paar Dutzend Millivolt ist normal; ein Faktor zwei bedeutet, dass der Teiler nicht stimmt.

6. **[Meilensteinüberprüfung]** Ihr Wert weicht um 3 % ab. Nennen Sie drei mögliche Ursachen und sagen Sie, welche davon sich im Code beheben lässt.

---

## Die Abstandssensoren am I2C

Die drei VL53L1X messen die Laufzeit eines Lichtimpulses und liefern eine Entfernung in Millimetern. Den Sensor selbst bedienen Sie nicht – das übernimmt die **ULD-API von ST**, die Sie fertig bekommen, samt der Plattformschicht, die sie auf Ihr I2C-Modul aus Einheit 5 setzt.

Die eigentliche Schwierigkeit liegt woanders: **Alle drei Sensoren haben ab Werk dieselbe I2C-Adresse.** Am selben Bus ist das eine Kollision. Gelöst wird es über je eine XSHUT-Leitung, mit der sich ein Sensor stillhalten lässt, während ein anderer eine neue Adresse bekommt.

> Laufzeitmessung, API-Aufbau und der Ablauf der Adressvergabe: ➡ **[Die ToF-Sensoren](ToF.md)**

### [AUFGABE] ToF-Sensoren einbinden

1. **XSHUT-Leitungen im HAL einrichten.** Drei gewöhnliche Ausgänge, nach demselben Muster wie die Displayleitungen in Einheit 6:

   ```c
   /* Auszug aus halGPIO.c */

   void halSetToFXshut(unsigned char xshutPin, bool enable)
   {
       // TODO: enable == true  -> Sensor freigeben
       //       enable == false -> Sensor in Reset halten
       //
       //       xshutPin ist die Bitmaske eines der drei Pins.
       //       Pruefen Sie im Schaltplan, welcher Pegel den Sensor
       //       freigibt - raten Sie nicht.
   }
   ```

   Die drei Pins benennen Sie in `halGPIO.h` als `TOF_XSHUT_LEFT`, `TOF_XSHUT_MID` und `TOF_XSHUT_RIGHT` und konfigurieren sie in `halGPIOInit()` als Ausgang.

2. **Dateien ins Projekt übernehmen.** Der Ordner `extLib` kommt **neben** `HAL` und `DL` ins Projektverzeichnis, `driverTof.c` und `driverTof.h` nach `DL/`. Danach **F5**.

   Anschließend den Include-Pfad ergänzen: *Properties → Build → MSP430 Compiler → Include Options* um `${PROJECT_ROOT}` erweitern – sonst findet der Compiler `#include <extLib/VL53L1X/vl53l1_platform.h>` nicht.

3. **`driverTofInit()` in `driverInit()` aufrufen**, nach `driverComInit()`. Die Initialisierung dauert spürbar – rund 50 ms je Sensor.

4. **Den Ablauf nachvollziehen.** `driverTofSetAllAddresses()` ist vorgegeben und in [ToF.md](ToF.md#die-adressvergabe-über-xshut) als Ablaufdiagramm beschrieben. Beantworten Sie dazu schriftlich:

   - Warum werden zuerst **alle** Sensoren in Reset gelegt?
   - Warum wird nach dem Freigeben 2 ms gewartet, bevor der Sensor angesprochen wird?
   - Was passiert, wenn ein Sensor nicht antwortet – und warum wird er dann wieder in Reset gelegt, statt ihn einfach zu überspringen?

5. **Lebenszeichen prüfen, bevor Sie Messwerten glauben.** `driverTofGetSensorId()` liefert die Model-ID; sie muss `TOF_SENSOR_ID` entsprechen. Kommt etwas anderes zurück, stimmt etwas an der Adresse oder der Verkabelung nicht – dann hat es keinen Sinn, an den Entfernungen zu zweifeln.

   Fragen Sie das für alle drei Adressen ab. `driverTofGetStatus()` sagt Ihnen zusätzlich, ob ein Sensor schon bei der Adressvergabe ausgefallen ist.

6. **Abstände abholen.** `driverTofGetDistance()` in der Hauptschleife aufrufen, für jede der drei Adressen. Der Rückgabewert hat drei Bedeutungen, und alle drei muss Ihr Code auseinanderhalten:

   | Rückgabe | bedeutet | was zu tun ist |
   |---|---|---|
   | `0` | neuer, gültiger Wert | übernehmen |
   | `1` | Messung noch nicht fertig | alten Wert behalten |
   | `2` | fertig, aber ungültig – z. B. nichts in Reichweite | alten Wert behalten, **nicht** überschreiben |

   ```c
   unsigned int distMid;

   if (driverTofGetDistance(TOF_ADDR_MID, &distMid) == 0)
   {
       // nur hier steht ein frischer, gültiger Wert
   }
   ```

   Wer den Rückgabewert ignoriert, bekommt bei jedem zweiten Durchlauf einen unveränderten oder unsinnigen Wert und sucht den Fehler im Sensor.

7. **[Meilensteinüberprüfung]** `VL53L1X_SetI2CAddress()` bekommt die Adresse um ein Bit nach links geschoben übergeben. Warum – was steht im I2C-Adressbyte an der untersten Stelle?

8. **[Meilensteinüberprüfung]** Die Adressvergabe läuft bei jedem Start neu. Warum merken sich die Sensoren ihre Adresse nicht?

---

## Verifikation im Debugger

Anders als in Einheit 5 und 6 gibt es hier nichts zu messen, was an einer Leitung sichtbar wäre – der Wert entsteht im Wandler und landet per DMA im Speicher. Der Nachweis läuft deshalb vollständig im Debugger.

### [AUFGABE] Messwerte im Debugger nachweisen

1. **Das Expressions-Fenster einrichten.** Tragen Sie ein:

   ```
   sensor_Data.ADCBuffer
   sensor_Data.ADCFiltered
   sensor_Data.Status.B.ADCrdy
   ```

   *In CCS:* *View → Expressions*, dann Variablennamen eintippen. Ein Array lässt sich mit dem Dreieck aufklappen; über das Kontextmenü *Number Format* ist auch Dezimaldarstellung wählbar.

2. **Continuous Refresh einschalten.** Das Icon mit den zwei kreisenden Pfeilen im Expressions-Fenster liest die Werte auch bei laufendem Programm nach. Damit sehen Sie die Messung in Bewegung, ohne anzuhalten.

3. **Die vier analogen Kanäle plausibilisieren.** Halten Sie die Hand vor **einen** Sharp-Sensor und beobachten Sie alle vier Werte gleichzeitig.

   | Beobachtung | Bedeutung |
   |---|---|
   | nur der zugehörige Wert ändert sich | richtig |
   | zwei Werte ändern sich gemeinsam | Kanäle vertauscht oder Sequenzende falsch gesetzt |
   | der Batteriekanal wandert mit | das Sequenzende sitzt nicht auf `ADC12MCTL3` |
   | gar nichts ändert sich | die DMA läuft nicht, oder `ADC12ENC` wird nicht neu gesetzt |

   Der Batteriekanal ist dabei Ihre Kontrolle: Er darf sich durch eine Hand vor dem Sensor **nicht** bewegen.

4. **Die Filterung sichtbar machen.** Vergleichen Sie `ADCBuffer` und `ADCFiltered` bei einer schnellen Handbewegung. Der gefilterte Wert zieht sichtbar nach.

5. **Das Verhalten der Sharp-Kennlinie aufnehmen.** Drei Beobachtungen, die erklären, warum ein Rohwert allein keine Entfernung ist – die Hintergründe stehen in [ADC12.md](ADC12.md#was-daraus-folgt):

   - Hindernis langsam von 150 cm auf 20 cm heranführen: Der Rohwert wächst **ungleichmäßig**. Notieren Sie je Entfernung einen Wert, etwa alle 20 cm.
   - Weiter auf 10 cm gehen: Der Wert **fällt wieder**. Bei welcher Entfernung hat er denselben Stand wie bei rund 60 cm?
   - Dasselbe Hindernis in gleichem Abstand vor zwei verschiedene Sensoren halten und die Rohwerte vergleichen.

   Daraus wird in einer späteren Einheit eine Umrechnungstabelle. Hier genügt es, die drei Eigenschaften gesehen zu haben.

6. **Die drei ToF-Abstände prüfen.** Ein Maßband genügt: Halten Sie ein Hindernis in bekanntem Abstand vor jeden Sensor und vergleichen Sie. Der VL53L1X misst in **Millimetern**, bei 30 cm steht dort also ein Wert um 300.

   Bleibt ein Wert unverändert, egal wie weit das Hindernis entfernt ist, dann misst dieser Sensor nicht – prüfen Sie `driverTofGetStatus()` und den Rückgabewert von `driverTofGetDistance()`, bevor Sie an der Mechanik suchen.

7. **Den Batteriewert prüfen**, wie in der Driver-Layer-Aufgabe beschrieben.

**Zu dokumentieren**

8. Ein Screenshot des Expressions-Fensters mit allen vier ADC-Kanälen und den drei ToF-Abständen, dazu die gleichzeitig gemessenen Vergleichswerte (Maßband, Multimeter).

   Dazu Ihre Wertetabelle zur Sharp-Kennlinie: Entfernung gegen Rohwert, von 150 cm bis 10 cm.
9. **[Meilensteinüberprüfung]** Setzen Sie einen Haltepunkt und bleiben Sie eine Weile stehen. Ändert sich `ADCBuffer`, während das Programm angehalten ist? Probieren Sie es aus und erklären Sie, was Sie beobachten – und was das für die Aussagekraft von Werten bedeutet, die Sie im angehaltenen Zustand ablesen.
10. **[Meilensteinüberprüfung]** Ein ToF-Sensor liefert dauerhaft 0. Nennen Sie drei Prüfschritte in sinnvoller Reihenfolge und sagen Sie, was jeder davon ausschließt.

---

## Testing

Geprüft wird die Konfiguration von Wandler und DMA sowie die Umrechnung im Driver Layer:

➡ **[Testing – Sensorik](Testing/Testing.md)**

## Referenzen

- **MSP430x5xx and MSP430x6xx Family User Guide**, Texas Instruments, Literature Number: SLAU208O, Rev. O, April 2019.  
  Verfügbar unter: [https://www.ti.com/lit/pdf/slau208](https://www.ti.com/lit/pdf/slau208)

- **MSP430F5335 Datasheet**, Texas Instruments, Document Number: SLAS590N, Rev. N, October 2018.  
  Verfügbar unter: [https://www.ti.com/lit/gpn/msp430f5335](https://www.ti.com/lit/gpn/msp430f5335)

- **VL53L1X Datasheet**, STMicroelectronics, DS13088.  
  Verfügbar unter: [https://www.st.com/resource/en/datasheet/vl53l1x.pdf](https://www.st.com/resource/en/datasheet/vl53l1x.pdf)

- **UM2356 – VL53L1X Ultra Lite Driver API user manual**, STMicroelectronics.  
  Verfügbar unter: [https://www.st.com/resource/en/user_manual/um2356-vl53l1x-api-user-manual-stmicroelectronics.pdf](https://www.st.com/resource/en/user_manual/um2356-vl53l1x-api-user-manual-stmicroelectronics.pdf)

[⬆ Zurück zum Hauptverzeichnis](../README.md#einheitsübersicht--aufgabenstellungen)
