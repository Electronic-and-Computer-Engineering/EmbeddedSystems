[⬅ Zurück zur Aufgabenstellung](../README.md)

# Testing – Schnittstellen

Ab dieser Einheit schreiben Sie die Testfälle vollständig selbst. Vorgegeben sind nur noch die Anforderungen: was geprüft werden soll und woran ein Testfall taugt.

> Die Einrichtung des Testframeworks ist in [Einheit 2](../../Kapitel_02_GPIO/Testing/Testing.md) beschrieben, das Anlegen eines neuen Testmoduls in [Einheit 3](../../Kapitel_03_TimerB0/Testing/Testing.md).

## Inhalt

- [Was sich an einer Schnittstelle prüfen lässt](#was-sich-an-einer-schnittstelle-prüfen-lässt)
- [Anforderungen an Ihre Testmodule](#anforderungen-an-ihre-testmodule)
- [Aufräumen](#aufräumen)
- [[AUFGABE] Testmodule schreiben](#aufgabe-testmodule-schreiben)

---

## Was sich an einer Schnittstelle prüfen lässt

Bei Timer und GPIO war die Sache eindeutig: Register lesen, mit dem Sollwert vergleichen. Eine Schnittstelle hat zusätzlich einen **Ablauf**, und der ist schwerer zu fassen.

Drei Ebenen, die sich unterscheiden lassen:

**Konfiguration.** Steht das Modul im richtigen Modus? Stimmen Taktquelle, Teiler, Bitreihenfolge, Polarität? Das ist derselbe Registervergleich wie bisher und funktioniert für alle drei Schnittstellen.

**Zustand.** Meldet das Modul im Ruhezustand korrekt, dass es frei ist? Wird es während einer Übertragung als beschäftigt gemeldet, und ist es danach wieder frei? Dafür gibt es bei allen drei Modulen eine Statusabfrage – `TxSuc`, `halUSCIA0TxBusy()`, `halI2CBusy()`.

**Verhalten.** Kommt am anderen Ende das Richtige an? Hier stößt der Test auf dem Controller an eine Grenze: Die Gegenstelle ist das Analog Discovery, und das muss angeschlossen und konfiguriert sein. Ein Testfall, der davon abhängt, schlägt auf jedem Arbeitsplatz ohne angestecktes Messgerät fehl – und ein Test, der nur manchmal läuft, ist kein Test.

Der Nachweis, dass tatsächlich die richtigen Daten übertragen werden, gehört deshalb in die Messung am Analog Discovery, nicht in die Unity-Suite.

**Eine Ausnahme gibt es.** Der I2C-Fehlerfall braucht gerade *keine* Gegenstelle: Ein Zugriff auf eine Adresse, an der nichts hängt, muss mit einem NACK enden, `halI2CError()` setzen und die Übertragung beenden. Dieser Testfall läuft überall und prüft genau den Teil des Zustandsautomaten, der im Normalbetrieb nie durchlaufen wird.

---

## Anforderungen an Ihre Testmodule

Legen Sie drei Testmodule in `test/tstHAL/` an und hängen Sie sie in `testHalGeneral.c` ein:

```
testHalUSCIB1.c/.h      SPI
testHalUSCIA0.c/.h      UART
testHalUSCIB0.c/.h      I2C
```

### `testHalUSCIB1` – SPI, mindestens 5 Fälle

- Das Modul steht nach der Initialisierung im Master-Mode, synchron, 8 Bit, MSB first.
- Taktpolarität und Taktphase entsprechen Ihrer Vorgabe.
- Der RX-Interrupt ist freigegeben.
- Der Teiler entspricht der geforderten Taktfrequenz. Prüfen Sie gegen Ihr `#define`, nicht gegen die ausgerechnete Zahl.
- Nach der Initialisierung meldet `TxSuc` einen abgeschlossenen Zustand – es läuft keine Übertragung.
- Eine Übertragung von zwei Bytes wird innerhalb einer plausiblen Zeitschranke abgeschlossen: `TxSuc` steht danach wieder auf 1 und `TxData.cnt` auf dem erwarteten Wert.

Der letzte Punkt ist der wertvollste, weil er die ISR mitprüft. Rechnen Sie vorher aus, wie lange zwei Bytes bei Ihrer Taktfrequenz dauern, und wählen Sie die Zeitschranke danach – nicht umgekehrt.

### `testHalUSCIA0` – UART, mindestens 4 Fälle

- Das Modul steht im UART-Mode, asynchron, 8 Datenbits, keine Parität, ein Stoppbit.
- Taktquelle und Baudratenteiler entsprechen der eingestellten Baudrate.
- Im Ruhezustand meldet `halUSCIA0TxBusy()` eine 0.
- Nach `halUSCIA0Send()` meldet das Modul sich als beschäftigt und ist nach Ablauf der berechneten Sendedauer wieder frei.
- `halUSCIA0Send()` liefert 0, wenn bereits eine Übertragung läuft.

Der letzte Fall prüft eine Zusicherung der Schnittstelle, nicht ein Register – genau solche Fälle fehlen in der Praxis am häufigsten.

### `testHalUSCIB0` – I2C, mindestens 4 Fälle

- Das Modul steht im I2C-Master-Mode, synchron.
- Der Baudratenteiler passt zur geforderten Busfrequenz.
- Im Ruhezustand meldet `halI2CBusy()` eine 0.
- Ein Zugriff auf eine Adresse, an der kein Teilnehmer hängt, setzt `halI2CError()` und beendet die Übertragung, statt hängen zu bleiben.
- Nach einem solchen Fehlversuch meldet `halI2CBusy()` wieder 0 – das Modul bleibt also benutzbar.

Diese beiden Fälle sind die wichtigsten des ganzen Moduls: Sie weisen nach, dass die NACK-Behandlung funktioniert. Ohne sie bleibt das Fahrzeug in einer späteren Einheit stehen, sobald ein Sensor nicht antwortet.

### Allgemeine Anforderungen

- Jeder Testfall prüft **eine** Aussage und stellt seinen Ausgangszustand selbst her.
- Verglichen wird gegen Makros aus `msp430f5335.h` und gegen Ihre eigenen `#define`, nie gegen Zahlenwerte.
- Registerfelder werden ausmaskiert verglichen, nicht per Bitabfrage – siehe Einheit 3.
- Jede Warteschleife hat eine Abbruchbedingung. Ein Test, der hängt, ist schlimmer als einer, der fehlschlägt.

---

## Aufräumen

Alle drei Module hinterlassen Zustand: laufende Übertragungen, gesetzte Fehler-Flags, freigegebene Interrupts. Ergänzen Sie `testHalCleanUp.c` entsprechend, sodass jeder Testfall unter denselben Bedingungen startet.

Überlegen Sie dabei, was **nicht** zurückgesetzt werden darf. Die Modulkonfiguration aus `halInit()` gehört nicht dazu – sonst prüfen die Konfigurationstests am Ende nur noch Ihr eigenes Aufräumen.

---

## [AUFGABE] Testmodule schreiben

1. Legen Sie die drei Testmodule an und hängen Sie sie in `testHalGeneral.c` ein.
2. Setzen Sie die oben genannten Anforderungen um. Die Mindestanzahl ist eine Untergrenze, keine Zielvorgabe.
3. Ergänzen Sie das Aufräumen.
4. Lassen Sie die gesamte Suite laufen. Die Fälle aus den Einheiten 2 bis 4 müssen weiterhin bestehen.
5. [Meilensteinüberprüfung] Welche Ihrer Testfälle würden auch dann noch bestehen, wenn die Leitung zum Display unterbrochen wäre? Was sagt das über die Aussagekraft dieser Tests?
6. [Meilensteinüberprüfung] Beim I2C lässt sich ein Fehlerfall ohne Gegenstelle auslösen, bei SPI und UART nicht. Woran liegt das?
7. [Meilensteinüberprüfung] Welche Aussagen über Ihre Schnittstellen können **nur** die Messung am Analog Discovery liefern, und welche **nur** die Unity-Suite?

---

[⬆ Zurück zur Aufgabenstellung](../README.md)
