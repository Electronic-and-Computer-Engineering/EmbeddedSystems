[⬅ Zurück zur Aufgabenstellung](../README.md)

# Testing – Sensorik

Der Schwerpunkt liegt in dieser Einheit auf dem Driver Layer: Am Wandler selbst lässt sich nur die Konfiguration prüfen, an der Umrechnung dagegen alles.

> Die Einrichtung des Testframeworks ist in [Einheit 2](../../Kapitel_02_GPIO/Testing/Testing.md) beschrieben, das Anlegen eines neuen Testmoduls in [Einheit 3](../../Kapitel_03_TimerB0/Testing/Testing.md).

## Inhalt

- [Was sich am ADC prüfen lässt](#was-sich-am-adc-prüfen-lässt)
- [Anforderungen an Ihre Testmodule](#anforderungen-an-ihre-testmodule)
- [Aufräumen](#aufräumen)
- [[AUFGABE] Suite anpassen und erweitern](#aufgabe-suite-anpassen-und-erweitern)

---

## Was sich am ADC prüfen lässt

Nicht viel – und das ist eine ehrliche Aussage, keine Ausrede.

| | prüfbar? |
|---|---|
| Registerkonfiguration des Wandlers | ja, wie bei jedem HAL-Modul bisher |
| dass eine Wandlung stattfindet | nur bedingt – ohne definierte Eingangsspannung ist das Ergebnis unbestimmt |
| dass der Messwert *stimmt* | nein – dafür bräuchte es eine bekannte Referenzspannung am Pin |
| die Umrechnung in Millivolt | **ja, vollständig** – reine Rechnerei |

Deshalb liegt der Schwerpunkt dieser Einheit auf dem **Driver Layer**. Dort steht eine Funktion, die aus einer Zahl eine physikalische Größe macht, und die lässt sich ohne angeschlossenen Akku bis in die Randfälle prüfen.

---

## Anforderungen an Ihre Testmodule

```
test/tstHAL/testHalADC12.c/.h   Konfiguration von Wandler und DMA
test/tstDL/testDLSensors.c/.h   Umrechnung in Millivolt
```

### `testHalADC12` – mindestens 4 Fälle

- Referenz und Auflösung stehen auf den gewählten Werten.
- Die Triggerquelle zeigt auf den TimerB0, nicht auf den Softwarestart.
- Der letzte Kanal der Sequenz trägt das Sequenzende-Bit, die anderen drei nicht.
- Nach `halADC12Init()` ist der Wandler freigegeben.

Prüfen Sie im selben Modul auch die DMA: Blockgröße, Transfermodus und Auslöser stehen nach `halDMAInit()` auf den erwarteten Werten, und die Zieladresse zeigt auf `ADCBuffer`.

### `testDLSensors` – mindestens 4 Fälle

Hier liegt die eigentliche Arbeit, und hier sind die interessanten Fälle:

- Ein Rohwert in der Mitte des Bereichs ergibt die erwartete Spannung.
- Rohwert `0` ergibt 0 mV.
- Rohwert `ADC_MAX_VALUE` ergibt die volle Spannung – und **hier** zeigt sich, ob Ihre Zwischenrechnung übergelaufen ist.
- `driverSensorsBatteryLow()` schaltet genau an der Grenze `VBAT_WARN_MV` um, nicht einen Millivolt daneben.

Der dritte Fall ist der wertvollste. `4095 * 2500` passt nicht in 16 Bit; wer das übersieht, bekommt bei kleinen Werten plausible Zahlen und beim Maximum Unsinn – ein Fehler, der im Fahrbetrieb genau dann zuschlägt, wenn der Akku voll ist.

> Die Umrechnung liest ihren Rohwert über `halADC12GetValue()`. Für den Test brauchen Sie einen Weg, dort einen bekannten Wert hineinzubekommen, ohne zu messen. Das ist dieselbe Frage wie beim Rahmenpuffer in Einheit 6 – und die Antwort darauf ist wieder der eigentliche Lerninhalt.

### Allgemeine Anforderungen

- Jeder Testfall prüft **eine** Aussage und stellt seinen Ausgangszustand selbst her.
- Verglichen wird gegen die `#define` aus `halADC12.h` und `driverSensors.h`, nie gegen Zahlenwerte.
- Die Fälle aus den Einheiten 2 bis 6 bestehen weiterhin, der angepasste Timer-Testfall eingeschlossen.

---

## Aufräumen

Diese Tests hinterlassen mehr als die der letzten Einheit, weil sie wieder Register anfassen. Nach einem Lauf stehen `sensor_Data.ADCBuffer` und `ADCFiltered` auf Testwerten, und das `ADCrdy`-Bit kann gesetzt sein. Setzen Sie beides in `testHalCleanUp.c` zurück.

Denken Sie auch an die DMA: Ein Kanal, der im Test eingeschaltet wurde und danach weiterläuft, schreibt Ihnen während der folgenden Testfälle in den Speicher.

---

## [AUFGABE] Suite anpassen und erweitern

1. Passen Sie den TimerB0-Testfall aus Einheit 3 an die neue Periode an.
2. Legen Sie `testHalADC12` und `testDLSensors` an und hängen Sie sie ein.
3. Setzen Sie die Anforderungen um.
4. Lassen Sie die gesamte Suite laufen. Alles außer dem angepassten Fall muss unverändert bestehen.
5. Ergänzen Sie das Aufräumen.

> **Bei der Überprüfung** sprechen wir über Ihre Testfälle: **warum** Sie etwas **wie** geprüft haben und welche Fälle Vorrang haben.

---

[⬆ Zurück zur Aufgabenstellung](../README.md)
