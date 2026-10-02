[⬅ Zurück zur Aufgabenstellung](../README.md)

# Testing – Zweites Testmodul

In Einheit 2 haben Sie die Testsuite in Betrieb genommen. Jetzt kommt ein zweites Testmodul dazu: `testHalTimerB0`. Es ist vollständig vorgegeben — Ihre Aufgabe ist es, das Modul einzuhängen, die Suite laufen zu lassen und zu verstehen, was dort geprüft wird.

> Die Einrichtung des Testframeworks ist in [Einheit 2](../../Kapitel_02_GPIO/Testing/Testing.md) beschrieben. Falls Include-Pfade, Präprozessorsymbol oder Heap noch nicht gesetzt sind, holen Sie das zuerst nach.

## Inhalt

- [Neues Testmodul einhängen](#neues-testmodul-einhängen)
- [`testHalTimerB0.c`](#testhaltimerb0c)
- [Registerfelder statt Bitmasken](#registerfelder-statt-bitmasken)
- [[AUFGABE] Suite erweitern und prüfen](#aufgabe-suite-erweitern-und-prüfen)

---

## Neues Testmodul einhängen

Pro HAL-Modul gibt es ein Testmodul. Es wird an zwei Stellen eingetragen:

**1.** Die Dateien `testHalTimerB0.c` und `testHalTimerB0.h` nach `test/tstHAL/` kopieren, danach **F5** im Project Explorer.

**2.** In `testHalGeneral.c` den Include und den Aufruf ergänzen:

```c
#include "testHalGPIO.h"
#include "testHalTimerB0.h"         // neu

void halInitTest(void)
{
    UNITY_BEGIN();

    testHalGPIORun();
    testHalTimerB0Run();            // neu

    UNITY_END();
}
```

Mehr ist nicht nötig — die Include-Pfade aus Einheit 2 decken den Ordner bereits ab.

---

## `testHalTimerB0.c`

```c
#include <msp430.h>
#include "unity.h"
#include "testHalTimerB0.h"
#include "halTimerB0.h"
#include "halGPIO.h"
#include "halUCS.h"

// ##### Hilfsfunktionen #####

static unsigned char backlightIsOn(void)
{
    return (P8OUT & LCD_BL) ? 1 : 0;
}

static unsigned int timerCountsWithin(unsigned int ms)
{
    unsigned int before = TB0R;
    delayMsBlocked(ms);
    return TB0R - before;
}

// ##### Konfiguration #####

static void timerSourceIsSMCLK(void)
{
    TEST_ASSERT_EQUAL_UINT(TBSSEL__SMCLK, TB0CTL & TBSSEL);
}

static void timerRunsInUpMode(void)
{
    TEST_ASSERT_EQUAL_UINT(MC__UP, TB0CTL & MC);
}

static void inputDividerIsEight(void)
{
    TEST_ASSERT_EQUAL_UINT(ID__8, TB0CTL & ID);
}

static void expansionDividerIsEight(void)
{
    TEST_ASSERT_EQUAL_UINT(TBIDEX__8, TB0EX0 & TBIDEX);
}

static void compareInterruptIsEnabled(void)
{
    TEST_ASSERT_BITS_HIGH(CCIE, TB0CCTL0);
}

static void compareValueMatchesCalculation(void)
{
    TEST_ASSERT_EQUAL_UINT(TimerB0SetDesTime, TB0CCR0);
}

// ##### Verhalten #####

static void timerIsCounting(void)
{
    TEST_ASSERT_GREATER_THAN(0, timerCountsWithin(10));
}

static void backlightTogglesWithinHalfPeriod(void)
{
    unsigned char before = backlightIsOn();

    delayMsBlocked(300);

    TEST_ASSERT_NOT_EQUAL(before, backlightIsOn());
}

void testHalTimerB0Run(void)
{
    RUN_TEST(timerSourceIsSMCLK);
    RUN_TEST(timerRunsInUpMode);
    RUN_TEST(inputDividerIsEight);
    RUN_TEST(expansionDividerIsEight);
    RUN_TEST(compareInterruptIsEnabled);
    RUN_TEST(compareValueMatchesCalculation);
    RUN_TEST(timerIsCounting);
    RUN_TEST(backlightTogglesWithinHalfPeriod);
}
```

Header dazu:

```c
#ifndef TEST_TESTHALTIMERB0_H_
#define TEST_TESTHALTIMERB0_H_

void testHalTimerB0Run(void);

#endif /* TEST_TESTHALTIMERB0_H_ */
```

Die letzten beiden Fälle prüfen nicht die Konfiguration, sondern das Verhalten: ob der Zähler wirklich läuft und ob die ISR die Beleuchtung tatsächlich umschaltet. Dafür wird `delayMsBlocked()` verwendet — im Test ist blockierendes Warten genau richtig, weil nichts anderes nebenher laufen soll.

---

## Registerfelder statt Bitmasken

Die meisten Testfälle vergleichen ein **Feld** innerhalb eines Registers, nicht einzelne Bits:

```c
TEST_ASSERT_EQUAL_UINT(MC__UP, TB0CTL & MC);
```

`MC` ist die Maske des gesamten Modus-Feldes, `MC__UP` der Wert, den es annehmen soll. Beide Makros stammen aus `msp430f5335.h`. Durch das Ausmaskieren bleiben nur die Bits dieses Feldes übrig, der Vergleich trifft also genau eine Aussage.

Mit einer reinen Bitprüfung wäre das nicht der Fall:

```c
TEST_ASSERT_BITS_HIGH(MC__UP, TB0CTL);        // ungenau
```

`MC__UP` setzt Bit 4. Stünde der Timer im Up/Down-Modus, wäre Bit 4 ebenfalls gesetzt — der Test ginge durch, obwohl die Konfiguration falsch ist.

Für einzelne Interrupt-Freigaben wie `CCIE` ist `TEST_ASSERT_BITS_HIGH` dagegen richtig, weil es dort tatsächlich nur um ein Bit geht.

Verwenden Sie in Tests immer die Makros aus dem Herstellerheader, nie die Zahlenwerte dahinter. `0x0010` funktioniert zwar, sagt beim Lesen aber nichts aus und muss bei jeder Änderung von Hand nachgezogen werden.

---

## [AUFGABE] Suite erweitern und prüfen

1. Hängen Sie das Testmodul nach den beiden Schritten oben ein.
2. Lassen Sie die gesamte Suite laufen. Die Fälle aus Einheit 2 müssen weiterhin bestehen, die acht neuen ebenfalls.
3. Ändern Sie versuchsweise `TimerB0time` auf einen anderen Wert und bauen Sie neu. Welche Testfälle schlagen fehl, welche nicht? Setzen Sie den Wert danach zurück.
4. [Meilensteinüberprüfung] `backlightTogglesWithinHalfPeriod()` wartet 300 ms. Warum reicht diese Zeit bei 2 Hz Blinkfrequenz — und warum wären 250 ms riskant?
5. [Meilensteinüberprüfung] `compareValueMatchesCalculation()` vergleicht ein Register gegen ein Makro. Welchen Fehler würde dieser Test **nicht** bemerken?

Punkt 5 ist der Grund, warum die Oszilloskopmessung aus der Hauptaufgabe nicht entfällt: Ein Test kann prüfen, ob im Register das steht, was die Rechnung vorgibt — ob die Rechnung selbst stimmt, zeigt nur die Messung.

---

[⬆ Zurück zur Aufgabenstellung](../README.md)