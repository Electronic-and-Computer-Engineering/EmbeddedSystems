[⬅ Zurück zur Aufgabenstellung](../README.md)

# Testing – Driver Layer

Bisher waren alle Testfälle vorgegeben. In dieser Einheit ändert sich das: Die **HAL-Tests schreiben Sie selbst**, für den **Driver Layer sind sie vorgegeben**.

Der Grund für diese Aufteilung: Die HAL-Konfiguration haben Sie jetzt dreimal gesehen — TimerB0, TimerA1, TimerA0 folgen demselben Muster, und die passenden Testfälle ebenfalls. Der Driver Layer ist neu, und seine Tests zeigen, worauf es bei einer Schnittstelle ankommt.

## Inhalt

- [HAL: was sich prüfen lässt](#hal-was-sich-prüfen-lässt)
- [DL: Tests gegen die Schnittstelle](#dl-tests-gegen-die-schnittstelle)
- [Einhängen](#einhängen)
- [`testDriverAktorik.c`](#testdriveraktorikc)
- [Aufräumen](#aufräumen)
- [[AUFGABE] Suite erweitern](#aufgabe-suite-erweitern)

---

## HAL: was sich prüfen lässt

Legen Sie `testHalTimerA1.c/.h` und `testHalTimerA0.c/.h` in `test/tstHAL/` an, nach dem Muster von `testHalTimerB0`. Die folgenden Punkte sind Vorschläge — welche Sie umsetzen, entscheiden Sie.

**`halTimerA1` (PWM)**

- Taktquelle, Modus und gegebenenfalls Teiler stehen auf den gewählten Werten
- `TA1CCR0` entspricht dem berechneten Periodenwert
- Beide Kanäle stehen im richtigen Ausgangsmodus
- Nach der Initialisierung sind beide Vergleichsregister auf 0 — es wird kein Puls erzeugt
- `halTimerA1SetSteering()` schreibt den übergebenen Wert nach `TA1CCR2`, `halTimerA1SetThrottle()` nach `TA1CCR1`
- Der Periodenzähler läuft: nach `halTimerA1ResetPeriodCnt()` und einer kurzen Wartezeit liefert `halTimerA1GetPeriodCnt()` einen plausiblen Wert

Der letzte Punkt ist der interessanteste, weil er die ESC-Sequenz absichert. Rechnen Sie vorher aus, wie viele Perioden Sie bei 60 Hz in einer bestimmten Wartezeit erwarten.

**`halTimerA0` (Drehzahl)**

- Taktquelle, Modus und Teiler
- `TA0CCR0` entspricht der gewählten Fensterlänge
- Der Capture-Kanal ist auf beide Flanken und auf synchrone Erfassung eingestellt
- Beide Interrupts sind freigegeben
- Ohne Radbewegung bleibt der Flankenzähler bei 0

Den Flankenzähler mit echten Impulsen zu füllen, geht im Test nicht — ein Eingangspegel lässt sich nicht per Software erzeugen, anders als ein Interrupt-Flag. Was Sie prüfen können, ist die Konfiguration und der Ruhezustand.

---

## DL: Tests gegen die Schnittstelle

`driverSetSteering()` rechnet einen Wert von −100 bis +100 in eine Pulsbreite um und gibt sie an die HAL weiter. Geprüft wird, was dabei im Register landet:

```c
driverSetSteering(-100);
TEST_ASSERT_EQUAL_UINT(ST_maxLeft, TA1CCR2);
```

Zwei Dinge sind daran bemerkenswert.

**Der Test kennt Ihre Zahlen nicht.** Verglichen wird gegen `ST_maxLeft`, nicht gegen 2750. Jede Gruppe misst andere Endanschläge, die Tests laufen trotzdem bei allen — weil sie die Schnittstelle prüfen, nicht die Messwerte.

**Es muss kein Servo angeschlossen sein.** Geprüft wird die Rechnung, nicht die Mechanik. Genau dafür gibt es den Driver Layer.

---

## Einhängen

Die DL-Tests liegen in `test/tstDL/`, dem Ordner, der bisher leer war.

**1.** `testDriverAktorik.c/.h` und `testDriverCleanUp.c/.h` nach `test/tstDL/` kopieren, danach **F5**.

**2.** Include-Pfad ergänzen: *Properties → Build → MSP430 Compiler → Include Options*

```
${PROJECT_ROOT}/test/tstDL
```

**3.** `test/testDriverGeneral.c` anlegen — das Gegenstück zu `testHalGeneral.c`:

```c
#include "unity.h"
#include "testDriverGeneral.h"
#include "testDriverAktorik.h"

void driverInitTest(void)
{
    UNITY_BEGIN();

    testDriverAktorikRun();

    UNITY_END();
}
```

Dazu der Header mit `void driverInitTest(void);`.

**4.** In `main.c` ergänzen:

```c
#ifdef RUN_TESTS
    halInitTest();
    driverInitTest();        // neu
#endif
```

`setUp()` und `tearDown()` bleiben in `testHalGeneral.c` — es darf sie im Projekt weiterhin nur einmal geben.

---

## `testDriverAktorik.c`

```c
#include <msp430.h>
#include "unity.h"
#include "testDriverAktorik.h"
#include "driverAktorik.h"
#include "halTimerA1.h"

// ##### Lenkung #####

static void steeringCenterSetsMiddle(void)
{
    driverSetSteering(0);
    TEST_ASSERT_EQUAL_UINT(ST_MIDDLE, TA1CCR2);
}

static void steeringFullLeftSetsLeftLimit(void)
{
    driverSetSteering(-100);
    TEST_ASSERT_EQUAL_UINT(ST_maxLeft, TA1CCR2);
}

static void steeringFullRightSetsRightLimit(void)
{
    driverSetSteering(100);
    TEST_ASSERT_EQUAL_UINT(ST_maxRight, TA1CCR2);
}

static void steeringNeverExceedsLimits(void)
{
    driverSetSteering(127);
    TEST_ASSERT_EQUAL_UINT(ST_maxRight, TA1CCR2);

    driverSetSteering(-128);
    TEST_ASSERT_EQUAL_UINT(ST_maxLeft, TA1CCR2);
}

static void steeringIsMonotonic(void)
{
    driverSetSteering(-50);
    unsigned int left = TA1CCR2;

    driverSetSteering(50);
    unsigned int right = TA1CCR2;

    TEST_ASSERT_GREATER_THAN(left, right);
}

// ##### Beschleunigung #####

static void throttleZeroSetsNeutral(void)
{
    driverSetThrottle(0);
    TEST_ASSERT_EQUAL_UINT(MaxBreak, TA1CCR1);
}

static void throttleFullForwardSetsMaximum(void)
{
    driverSetThrottle(100);
    TEST_ASSERT_EQUAL_UINT(MaxFPW, TA1CCR1);
}

static void throttleFullReverseSetsMinimum(void)
{
    driverSetThrottle(-100);
    TEST_ASSERT_EQUAL_UINT(MaxRPW, TA1CCR1);
}

static void throttleNeverExceedsLimits(void)
{
    driverSetThrottle(127);
    TEST_ASSERT_EQUAL_UINT(MaxFPW, TA1CCR1);

    driverSetThrottle(-128);
    TEST_ASSERT_EQUAL_UINT(MaxRPW, TA1CCR1);
}

void testDriverAktorikRun(void)
{
    RUN_TEST(steeringCenterSetsMiddle);
    RUN_TEST(steeringFullLeftSetsLeftLimit);
    RUN_TEST(steeringFullRightSetsRightLimit);
    RUN_TEST(steeringNeverExceedsLimits);
    RUN_TEST(steeringIsMonotonic);

    RUN_TEST(throttleZeroSetsNeutral);
    RUN_TEST(throttleFullForwardSetsMaximum);
    RUN_TEST(throttleFullReverseSetsMinimum);
    RUN_TEST(throttleNeverExceedsLimits);
}
```

`steeringIsMonotonic()` prüft etwas anderes als die übrigen Fälle: nicht einen festen Wert, sondern eine Eigenschaft — ein größerer Eingabewert muss zu einem größeren Registerwert führen. Solche Tests fallen auch dann auf, wenn jemand das Vorzeichen vertauscht und beide Endanschläge trotzdem stimmen.

---

## Aufräumen

Nach dem Testlauf stehen Lenkung und Beschleunigung auf dem Wert des letzten Testfalls. `testDriverCleanUp.c` setzt beides zurück, nach demselben Muster wie `testHalCleanUp.c`:

```c
#include "testDriverCleanUp.h"
#include "driverAktorik.h"

void testDriverCleanupAll(void)
{
    driverSetSteering(0);
    driverSetThrottle(0);
}
```

In `testHalGeneral.c` wird es an `tearDown()` angehängt:

```c
void tearDown(void)
{
    testHalCleanupAll();
    testDriverCleanupAll();
}
```

> ⚠️ **Vor dem ersten Testlauf den ESC ausschalten.** Die Beschleunigungs-Tests setzen kurzzeitig Vollgas und volle Rückwärtsfahrt als Pulsbreite. Bei ausgeschaltetem Fahrtenregler werden die Register trotzdem beschrieben und lesbar — der Motor läuft aber nicht. Erst wenn alle Tests grün sind und das Fahrzeug aufgebockt ist, den ESC wieder einschalten.

---

## [AUFGABE] Suite erweitern

1. Schreiben Sie die HAL-Testmodule für `halTimerA1` und `halTimerA0`. Mindestens vier Testfälle je Modul, darunter einer, der das Verhalten prüft und nicht nur die Konfiguration.
2. Hängen Sie die vorgegebenen DL-Tests nach den vier Schritten oben ein.
3. Lassen Sie die gesamte Suite laufen. Die Fälle aus den Einheiten 2 und 3 müssen weiterhin bestehen.
4. [Meilensteinüberprüfung] Warum vergleichen die DL-Tests gegen `ST_maxLeft` statt gegen den gemessenen Zahlenwert?
5. [Meilensteinüberprüfung] `steeringIsMonotonic()` prüft keinen festen Wert. Welchen Fehler würde dieser Testfall finden, den die anderen vier übersehen?
6. [Meilensteinüberprüfung] Der Flankenzähler in `halTimerA0` lässt sich nicht automatisiert füllen, das Interrupt-Flag aus Einheit 2 dagegen schon. Worin liegt der Unterschied?

---

[⬆ Zurück zur Aufgabenstellung](../README.md)