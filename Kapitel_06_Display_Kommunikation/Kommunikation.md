[⬅ Zurück zur Aufgabenstellung](README.md)

# Kommunikation mit dem Fahrzeug

## Inhalt
- [Warum ein Rahmen nötig ist](#warum-ein-rahmen-nötig-ist)
- [Die beiden Rahmenformate](#die-beiden-rahmenformate)
- [Ein Zahlenwert, zwei Bytes](#ein-zahlenwert-zwei-bytes)
- [Negative Zahlen](#negative-zahlen)
- [USB oder Bluetooth: die Umschaltung](#usb-oder-bluetooth-die-umschaltung)
- [Der zweite UART](#der-zweite-uart)
- [Die Gegenstelle am PC](#die-gegenstelle-am-pc)

---

## Warum ein Rahmen nötig ist

In Einheit 5 haben Sie Zeichenfolgen über den UART geschickt und in HTerm angesehen. Das reicht für einen Funktionsnachweis, nicht für Telemetrie. Ein UART überträgt einen **Strom von Bytes ohne Anfang und Ende** – der Empfänger weiß nicht, wo ein Datensatz beginnt, und bei einem verlorenen Byte verschiebt sich alles Folgende dauerhaft.

Die Lösung ist ein **Rahmen**: eine feste Struktur mit erkennbarem Anfang, bekannter Länge und erkennbarem Ende. Der Empfangsteil in `halUSCIA0` sucht bereits nach einem Sync-Muster – jetzt bekommt dieses Muster einen Inhalt.

---

## Die beiden Rahmenformate

Beide Richtungen haben ein eigenes Format, und beide sind absichtlich unterschiedlich gekennzeichnet:

**Vom Fahrzeug zum PC** – Telemetrie, beliebig viele Werte:

<p align="center">
  <img src="./media/frameCar.svg" alt="Rahmen vom Auto zum PC">
</p>

**Vom PC zum Fahrzeug** – ein Kommando mit einem Wert, feste Länge:

<p align="center">
  <img src="./media/frameHost.svg" alt="Rahmen vom PC zum Auto">
</p>

Was in beiden Bildern **nicht** vorkommt, ist so wichtig wie das, was drinsteht: keine Längenangabe, keine Prüfsumme, kein Feld, das den Inhalt beschreibt. Der Rahmen trägt eine Kennung, die Nutzbytes und wieder eine Kennung – mehr nicht. Alles andere ist **Vereinbarung**: Wie viele Werte kommen, in welcher Reihenfolge sie stehen und was sie bedeuten, wissen beide Seiten aus ihrem eigenen Code. Im Fahrzeug steht es als `count` in Ihrem Aufruf von `driverComSend()`, am PC als `NUM_VALS` beziehungsweise `NAMEN` in der Gegenstelle.

Das ist keine Sparsamkeit, sondern eine Entscheidung. Ein selbstbeschreibender Rahmen müsste auf beiden Seiten ausgewertet werden, und ein Empfänger, der auf eine angekündigte Länge wartet, bleibt stehen, wenn die Ankündigung verlorengeht. Hier gibt es nichts, worauf man vergeblich warten könnte: Kommt Unsinn, setzt der Empfang beim nächsten Sync-Muster wieder auf.

Dafür zahlen Sie einen Preis, und den sollten Sie kennen: **Ändern Sie die Anzahl der Werte im Fahrzeug, müssen Sie sie auch in der Gegenstelle ändern.** Dieselbe Festlegung steht an zwei Stellen. Vergisst man eine davon, verwirft der PC stillschweigend jeden Rahmen – ein Fehlerbild, das aussieht, als wäre die Leitung tot.

`C` steht für Car, `H` für Host. Dass die beiden Richtungen verschiedene Kennungen haben, ist kein Schmuck: Hängt versehentlich eine Schleife zwischen TX und RX, oder lauscht ein Gerät auf der falschen Leitung, fällt es sofort auf, statt plausibel aussehenden Unsinn zu erzeugen.

> **Warum das Rahmenende prüfen, wenn der Anfang doch schon erkannt wurde?** Weil das Sync-Muster auch mitten in den Nutzdaten auftauchen kann. Ein Sensorwert von **18515** wird als die beiden Bytes 72 und 83 übertragen – und das sind genau die ASCII-Codes von `'H'` und `'S'`. Der Empfänger hält diese zwei Datenbytes für einen Rahmenanfang und beginnt an der falschen Stelle. Erst die Prüfung des Rahmenendes deckt das auf: Dort steht dann kein `'E'`, `'H'`, und der Rahmen wird verworfen statt falsch ausgewertet.

---

## Ein Zahlenwert, zwei Bytes

Ein `unsigned int` ist auf dem MSP430 16 Bit breit, ein UART überträgt 8 Bit. Jeder Wert wird deshalb in zwei Bytes zerlegt, und **beide Seiten müssen sich über die Reihenfolge einig sein**:

```c
frame[n++] = value >> 8;        // high byte zuerst
frame[n++] = value & 0xFF;      // dann low byte
```

Die Gegenstelle setzt entsprechend zusammen:

```matlab
vals = double(payload(1:2:end)) * 256 + double(payload(2:2:end));
```

Diese Festlegung heißt **Big Endian** und ist bei Übertragungsprotokollen die übliche. Der MSP430 legt Zahlen im Speicher andersherum ab – würde man den Speicherinhalt einfach so hinausschicken, kämen die Bytes vertauscht an. Deshalb zerlegt der Code die Zahl ausdrücklich, statt sich auf die Speicherlage zu verlassen.

Was dabei schiefgehen kann, sieht man am besten an echten Zahlen. Nehmen Sie den vorgegebenen Testsatz:

| gesendet | richtig zusammengesetzt | bei vertauschten Bytes |
|---|---|---|
| 0 | 0 | 0 |
| 4095 | 4095 | **65295** |
| 65535 | 65535 | 65535 |
| −40 | −40 | **55551** |

Zwei der vier Werte sind unauffällig: Bei 0 und 65535 sind beide Bytes gleich, da fällt eine Vertauschung nicht auf. Bei 4095 und −40 schon, und zwar sofort und ohne Rechnerei. Genau deshalb stehen diese vier Zahlen im Testsatz.

---

## Negative Zahlen

Der Rahmen überträgt **keine Vorzeichen**. Er überträgt 16 Bit, mehr nicht. Ob diese 16 Bit eine Zahl von 0 bis 65535 bedeuten oder eine von −32768 bis +32767, steht nirgends im Rahmen – das entscheidet erst, wer die Zahl ausliest.

Auf der Fahrzeugseite heißt das: Sie schicken den Wert einfach als `unsigned int` weg, auch wenn er negativ gemeint ist. Das Bitmuster bleibt dasselbe:

```c
signed int lenkwinkel = -40;
werte[0] = (unsigned int)lenkwinkel;    // dasselbe Bitmuster, jetzt als 65496 gelesen
```

Auf der PC-Seite wird es wieder eingerenkt. Die Regel ist eine Zeile: **Alles über 32767 ist in Wirklichkeit negativ**, und man zieht 65536 ab.

```matlab
if wert > 32767
    wert = wert - 65536;        % 65496 -> -40
end
```

Im MATLAB-Skript steht dafür die Variable `SIGNED`: ein Eintrag je Wert, `1` bedeutet „diesen als vorzeichenbehaftet lesen". Beim Testsatz ist das nur der letzte.

Das ist keine Notlösung, sondern genau das, was jedes Protokoll tut. Die Bedeutung von Bits steht in der Vereinbarung zwischen den beiden Seiten, nicht in den Bits selbst. Dieselbe Frage stellt sich in Einheit 7 noch einmal beim VL53L1X, dort in der Gegenrichtung.

---

## USB oder Bluetooth: die Umschaltung

Das Fahrzeug hat zwei UARTs: den Debug-UART über den Debugger und den AUX-UART auf der Steckerleiste, an den sich ein Funkmodul stecken lässt. Beide sollen **dasselbe** Protokoll sprechen, und der Treiber soll nicht wissen, über welchen er gerade redet.

> Ob zum Zeitpunkt Ihrer Übung ein Funkmodul am Fahrzeug steckt, sagt Ihnen die Betreuung. Für den Nachweis der Umschaltung brauchen Sie keines: Dass der Rahmen danach auf der **anderen Leitung** und mit **anderem Bittiming** erscheint, sehen Sie am Analog Discovery.

Gelöst ist das mit einem Satz Makros, der zur Übersetzungszeit umgelegt wird:

```c
#define COM_USE_BT  0       // 0 = USB (UCA0), 1 = Bluetooth (UCA1)

#if COM_USE_BT
    #define COM_BAUDRATE   9600
    #define halComInit     halUSCIA1Init
    #define halComSend     halUSCIA1Send
    …
#else
    #define COM_BAUDRATE   57600
    #define halComInit     halUSCIA0Init
    #define halComSend     halUSCIA0Send
    …
#endif
```

`driverCom.c` ruft nur noch `halComInit()`, `halComSend()`, `halComGetFrame()` auf. **Ein einziges `#define` legt den Weg fest**, und keine Zeile des Treibers ändert sich dabei.

Drei Dinge folgen daraus:

**Es kostet zur Laufzeit nichts.** Der Präprozessor ersetzt die Namen, im übersetzten Code steht direkt die richtige Funktion – keine Abfrage, kein Funktionszeiger.

**Es ist keine Laufzeitumschaltung.** Welcher Weg gilt, entscheidet sich beim Übersetzen. Um zu wechseln, muss neu gebaut und geflasht werden. Für dieses Fahrzeug ist das richtig so: Beim Fahren hängt kein USB-Kabel dran, in der Werkstatt kein Bluetooth-Modul.

**Beide können nicht gleichzeitig laufen.** Die beiden UART-Module teilen sich ihren Empfangs- und Sendezustand in `halSharedUART.h` – das ist die Datei, die Ihnen in Einheit 5 schon begegnet ist und deren Zweck jetzt sichtbar wird. Ein Empfangspuffer statt zweier, zugunsten des Speichers.

---

## Der zweite UART

`halUSCIA1` bekommen Sie fertig. Es ist dasselbe Modul wie `halUSCIA0` aus Einheit 5, mit drei Unterschieden:

| | `halUSCIA0` | `halUSCIA1` |
|---|---|---|
| Anschluss | eZ-FET des Debuggers, virtueller COM-Port | Steckerleiste, Bluetooth-Modul |
| Pins | P2.0 / P2.1, über Port-Mapping | P8.2 / P8.3, feste Pinfunktion |
| Baudrate im Projekt | 57600 | 9600 |

Die niedrigere Baudrate am Bluetooth ist kein Zufall: Viele Module laufen ab Werk mit 9600, und die Luftstrecke verträgt weniger als ein Kabel.

Der Unterschied bei den Pins ist der Grund, warum für diesen UART **kein Port-Mapping** nötig ist: P8.2 und P8.3 tragen die UCA1-Funktion fest, dort genügt das `PxSEL`-Bit. Das Schlüsselregister `PMAPKEYID` aus Einheit 5 brauchen Sie hier nicht.

> **Vergleichen Sie die beiden Empfangszweige Zeile für Zeile.** Sie sind fast identisch – an einer Stelle aber nicht. Spielen Sie beide Varianten von Hand durch: Sync-Muster `'H','S'`, eintreffender Zeichenstrom `H H S …`. Welche der beiden erkennt den Rahmen, welche nicht, und woran liegt es? Das ist genau der Fall aus dem Kasten oben, diesmal im Code.

---

## Die Gegenstelle am PC

Zum Fahrzeug gehören zwei Gegenstellen. Beide sprechen dasselbe Protokoll, sie sind für verschiedene Arbeitsschritte gedacht.

**Zum Lesen: [`tools/crazyCarCom.m`](tools/crazyCarCom.m)**

Ein kurzes MATLAB-Skript. Es öffnet den seriellen Port, synchronisiert auf `'C','S'`, prüft das Rahmenende, setzt die Werte zusammen und zeichnet sie auf. Absichtlich kurz und absichtlich ohne Oberfläche: **Sie sollen die Gegenseite Ihres eigenen Protokolls lesen können.** Dieselbe Festlegung steht dort in MATLAB, die Sie in C getroffen haben – wenn eine der beiden Seiten nicht stimmt, sehen Sie an welcher Stelle. Fangen Sie damit an.

```matlab
PORT      = "COM7";        % Gerätemanager -> MSP Application UART
BAUD      = 57600;         % 57600 über USB, 9600 über Bluetooth
NUM_VALS  = 2;             % so viele Werte schickt Ihr driverComSend()
```

Drei Zeilen sind anzupassen, und alle drei müssen zu Ihrem Fahrzeug passen. Stimmt `NUM_VALS` nicht, läuft die Synchronisation aus dem Tritt und das Skript verwirft jeden Rahmen – ein guter Testfall für Ihre eigene Fehlerprüfung.

**Zum Arbeiten: [`tools/ccMonitor.py`](tools/ccMonitor.py)**

Eine kleine Oberfläche in Python. Links läuft mit, was das Fahrzeug sendet; rechts schicken Sie Kommandorahmen hin, ohne jedes Mal ein Skript zu ändern. Beide Richtungen zeigt sie zusätzlich in Hex an:

```
43 53 | FF D8  00 32 | 45 43
```

Damit sehen Sie Ihren Rahmen so, wie er auf der Leitung steht – dieselbe Ansicht wie später im UART-Dekoder des Analog Discovery, nur ohne Messgerät. Das macht sie zum nützlichsten Werkzeug bei der Fehlersuche: Steht links ein Rahmen, der anders aussieht als gedacht, liegt der Fehler in `driverComSend()`. Steht gar keiner, liegt er davor.

Angepasst wird auch hier nur der Block am Dateianfang:

```python
PORT  = "COM7"                       # Geraetemanager -> MSP Application UART
BAUD  = 57600                        # 57600 ueber USB, 9600 ueber Bluetooth
NAMEN = ["Wert 0", "Wert 1", ...]    # so viele Werte schickt Ihr driverComSend()
VORZEICHEN = [0, 0, 0, 1]            # 1 = diesen Wert als vorzeichenbehaftet lesen
TYPEN = {1: ("Lenkwinkel", -100, 100),
         2: ("Gas",        -100, 100)}   # Ihre Kommandotabelle
```

`TYPEN` ist Ihre Kommandotabelle aus der Aufgabenstellung, noch einmal – je Eintrag erscheint rechts eine Zeile mit Schieberegler und Knopf. Für eine Nummer, die es im Fahrzeug noch gar nicht gibt, gibt es unten eine freie Zeile: Typ und Wert eintippen, senden, am Fahrzeug im Debugger nachsehen. So lässt sich ein Kommando ausprobieren, bevor es im C-Code existiert.

Zwei Dinge sind zu beachten. `pyserial` und `matplotlib` müssen installiert sein (`pip install pyserial matplotlib`), und **nur ein Programm kann den Port öffnen** – MATLAB, HTerm und der Monitor vertragen sich nicht gleichzeitig.

Ohne Fahrzeug lässt sich die Oberfläche mit `python ccMonitor.py --demo` ansehen, und `python ccMonitor.py --selftest` prüft Packen und Auspacken gegen genau den Testsatz, der weiter oben in der Tabelle steht – ohne Port, ohne Fenster.

In beiden Werkzeugen steht dieselbe Vereinbarung wie in Ihrem C-Code, und in beiden steht sie an genau einer Stelle. Das ist der praktische Teil dessen, was im Abschnitt über die Rahmenformate steht: Was nicht im Rahmen steht, muss gepflegt werden.

---

[⬆ Zurück zur Aufgabenstellung](README.md)
