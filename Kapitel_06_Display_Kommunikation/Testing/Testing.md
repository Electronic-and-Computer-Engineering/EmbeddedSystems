[⬅ Zurück zur Aufgabenstellung](../README.md)

# Testing – Display und Kommunikation

Diese Einheit ist die erste, in der sich der Driver Layer ohne jede Hardware prüfen lässt. Nutzen Sie das.

> Die Einrichtung des Testframeworks ist in [Einheit 2](../../Kapitel_02_GPIO/Testing/Testing.md) beschrieben, das Anlegen eines neuen Testmoduls in [Einheit 3](../../Kapitel_03_TimerB0/Testing/Testing.md).

## Inhalt

- [Was sich ohne Gegenstelle prüfen lässt](#was-sich-ohne-gegenstelle-prüfen-lässt)
- [Anforderungen an Ihre Testmodule](#anforderungen-an-ihre-testmodule)
- [Aufräumen](#aufräumen)
- [[AUFGABE] Testmodule schreiben](#aufgabe-testmodule-schreiben)

---

## Was sich ohne Gegenstelle prüfen lässt

Beim Frameformat ist das Prüfbare besonders klar abgegrenzt: **Packen und Auspacken sind reine Rechnerei.** Dafür braucht es weder einen PC am anderen Ende noch ein Bluetooth-Modul – nur einen Puffer und einen erwarteten Inhalt.

Das ist der Unterschied zu allem bisher. Die Tests der Einheiten 2 bis 5 prüften Registerinhalte, also Konfiguration. Hier prüfen Sie zum ersten Mal **Logik**: eine Umwandlung, die richtig oder falsch sein kann, unabhängig von jeder Hardware.

Beim Display ist die Lage umgekehrt. Ob ein Zeichen an der richtigen Stelle steht, sieht man – ein Test müsste den Bildspeicher zurücklesen, und der ST7565 gibt über die serielle Schnittstelle nichts zurück. Prüfbar ist dort nur, was **vor** dem Senden passiert: die erzeugten Kommandobytes.

---

## Anforderungen an Ihre Testmodule

```
test/tstDL/testDLCom.c/.h       Frameformat
test/tstDL/testDLLcd.c/.h       Textausgabe und Zahlendarstellung
```

### `testDLCom` – mindestens 5 Fälle

Für die Sendeseite müssen Sie an den Framepuffer herankommen, ohne die Übertragung anzustoßen. Überlegen Sie, wie – das ist Teil der Aufgabe.

- Ein Frame mit zwei Werten hat die Länge `COM_TX_LEN(2)` und beginnt mit `COM_CAR_START_1`, `COM_CAR_START_2`.
- Der Wert `4095` erscheint als die beiden Bytes `15` und `255`, in dieser Reihenfolge. Nehmen Sie bewusst diesen Wert und nicht `0` oder `65535` – dort sind beide Bytes gleich, und der Test würde auch bei vertauschter Reihenfolge bestehen.
- Der Frame endet mit `COM_CAR_END_1`, `COM_CAR_END_2`.
- Mehr als `COM_TX_MAX_VAL` Werte werden begrenzt und sprengen den Puffer nicht.
- `driverComReceive()` liefert bei einem Frame mit falschem Frameende eine 0 und schreibt **nichts** nach `*cmd` und `*value`.
- Ein gültiger Frame wird korrekt ausgepackt: Kommando und 16-Bit-Wert stimmen.

Der vierte und der fünfte Fall sind die wertvollsten. Beide prüfen das Verhalten im **Fehlerfall**, und beide fallen im Laborbetrieb nie auf – bis zu dem Tag, an dem ein Byte verloren geht.

### `testDLLcd` – mindestens 3 Fälle

Geprüft wird, was **Sie** geschrieben haben, nicht das Vorgegebene. `driverLCDSetPosition()` und `driverLCDClear()` sind fertig ausgeliefert und brauchen keinen Test; interessant sind die beiden Funktionen mit Logik darin:

- `driverLCDWriteUInt()` zerlegt eine Zahl korrekt in Ziffern. Prüfen Sie `0`, eine einstellige und eine fünfstellige Zahl – die Reihenfolge der Ziffern ist der eigentliche Fehlerkandidat.
- `driverLCDWriteText()` bricht am Zeilenende in die nächste Seite um. Ein Text, der bei Spalte 125 beginnt, muss auf zwei Seiten landen.
- `driverLCDWriteText()` bricht ab, wenn die letzte Seite voll ist, statt über das Display hinauszuschreiben.

Dafür brauchen Sie einen Weg, die erzeugten Bytes abzufangen statt sie ans Display zu schicken – dieselbe Frage wie beim Framepuffer, und die Antwort darauf ist der eigentliche Lerninhalt dieses Testmoduls.

### Allgemeine Anforderungen

- Jeder Testfall prüft **eine** Aussage und stellt seinen Ausgangszustand selbst her.
- Verglichen wird gegen die `#define` aus `driverCom.h` und `driverLCD.h`, nie gegen Zahlenwerte oder Zeichen. Für die Framekennungen stehen `COM_CAR_START_1`, `COM_CAR_END_2` und die übrigen Namen bereit, für die Framelänge `COM_TX_LEN(n)`.
- Kein Testfall darf eine Übertragung auslösen, die auf Hardware wartet.
- `comTxFrame` ist in `driverCom.c` definiert und im Header nicht sichtbar. Wie Ihr Testmodul trotzdem hinkommt, ist Teil der Aufgabe – aber ändern Sie dafür nichts am Treiber, was Sie danach nicht erklären können.

---

## Aufräumen

Diese Tests hinterlassen weniger als die bisherigen – sie fassen keine Register an. Was bleibt, ist der Inhalt von `comTxFrame` und der Empfangszustand aus `halSharedUART.h`. Setzen Sie beides in `testHalCleanUp.c` zurück, damit die Reihenfolge der Testfälle keine Rolle spielt.

---

## [AUFGABE] Testmodule schreiben

1. Legen Sie `testDLCom` und `testDLLcd` an und hängen Sie sie in `testHalGeneral.c` ein.
2. Setzen Sie die Anforderungen um.
3. Lassen Sie die gesamte Suite laufen. Die Fälle aus den Einheiten 2 bis 5 müssen weiterhin bestehen.
4. Ergänzen Sie das Aufräumen.

> **Bei der Überprüfung** sprechen wir über Ihre Testfälle: **warum** Sie etwas **wie** geprüft haben und welche Fälle Vorrang haben.

---

[⬆ Zurück zur Aufgabenstellung](../README.md)
