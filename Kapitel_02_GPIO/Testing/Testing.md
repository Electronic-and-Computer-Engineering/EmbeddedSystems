[⬅ Zurück zur Aufgabenstellung](../README.md)

# Testing – Ersteinrichtung

In dieser Einheit wird das Testframework **Unity** zum ersten Mal in Betrieb genommen. Der Ordner `test` wird Ihnen vollständig bereitgestellt; Ihre Aufgabe ist es, ihn in Ihr Projekt einzubinden und die Testsuite zum Laufen zu bringen.

Eigene Testfälle schreiben Sie ab der nächsten Einheit.

## Inhalt

- [Wozu Unit-Tests](#wozu-unit-tests)
- [Der Ordner `test`](#der-ordner-test)
- [Schritt-für-Schritt: Einrichtung](#schritt-für-schritt-einrichtung)
- [Testlauf](#testlauf)
- [Die Ausgabe lesen](#die-ausgabe-lesen)
- [Aufbau eines Testfalls](#aufbau-eines-testfalls)
- [Assertions](#assertions)
- [Fehlersuche](#fehlersuche)
- [[AUFGABE] Testsuite in Betrieb nehmen](#aufgabe-testsuite-in-betrieb-nehmen)

---

## Wozu Unit-Tests

Eine Funktion im Debugger durchzusteppen zeigt, dass sie in **einem** Fall das Richtige tut. Ein Unit-Test prüft dasselbe automatisch, in Sekunden, und wiederholt es bei jedem Programmstart.

Der eigentliche Nutzen zeigt sich später im Kurs: Wenn Sie in späteren Laboreinheiten Änderungen vornehmen, müssen die Tests der vorherigen Einheit unverändert bestehen bleiben. Genau daran erkennen Sie, dass die Umstellung nichts zerstört hat.

Die Tests laufen (zumindest für HAL und DL) **direkt auf dem Controller**. 

---

## Der Ordner `test`

```
test/
├── unity/
│   ├── unity.c
│   ├── unity.h
│   ├── unity_internals.h
│   └── unity_config.h          Anpassungen für den MSP430
├── tstHAL/
│   ├── testHalGPIO.c           Testfälle für das GPIO-Modul
│   ├── testHalGPIO.h
│   ├── testHalCleanUp.c        setzt die Zustände nach jedem Testfall zurück
│   └── testHalCleanUp.h
├── tstDL/                      noch leer, wird in späteren Einheiten gefüllt
├── testHalGeneral.c            ruft alle HAL-Testmodule auf
└── testHalGeneral.h
```

`testHalGeneral.c` ist das Gegenstück zu `halGeneral.c`: So wie dort pro HAL-Modul ein `halXxxInit()` aufgerufen wird, steht hier pro Testmodul ein `testXxxRun()`.

```c
void setUp(void)    { }

void tearDown(void)
{
    testHalCleanupAll();
}

void halInitTest(void)
{
    UNITY_BEGIN();

    testHalGPIORun();

    UNITY_END();
}
```

`setUp()` und `tearDown()` ruft Unity selbst auf — vor und nach **jedem einzelnen** Testfall. Deshalb darf es beide im gesamten Projekt nur einmal geben. `tearDown()` setzt über `testHalCleanupAll()` die Zustände zurück, damit jeder Testfall unter denselben Bedingungen startet und nach dem Durchlauf nichts hängen bleibt.

Die Dateien `unity.c`, `unity.h` und `unity_internals.h` stammen von [ThrowTheSwitch/Unity](https://github.com/ThrowTheSwitch/Unity) und werden **nicht verändert**.

---

## Schritt-für-Schritt: Einrichtung

### 1. Ordner ins Projekt kopieren

Aus dem bereitgestellten Verzeichnis `Testing` den darin liegenden Ordner **`test`** in Ihr Projektverzeichnis kopieren — auf dieselbe Ebene wie `HAL` und `main.c`. Nicht den Ordner `Testing` selbst.

Anschließend im *Project Explorer* das Projekt anklicken und **F5** drücken. Der Ordner erscheint im Projektbaum.

### 2. Include-Pfade setzen

*Rechtsklick auf das Projekt → Properties → Build → MSP430 Compiler → Include Options*

Im Feld *Add dir to #include search path* über das **+** drei Einträge hinzufügen:

```
${PROJECT_ROOT}/test
${PROJECT_ROOT}/test/unity
${PROJECT_ROOT}/test/tstHAL
```

Ohne diese Pfade findet der Compiler `unity.h` nicht.

### 3. Präprozessorsymbol setzen

*Properties → Build → MSP430 Compiler → Predefined Symbols*

Im Feld *Pre-define NAME* über das **+** eintragen, ohne Anführungszeichen:

```
UNITY_INCLUDE_CONFIG_H
```

Unity liest die Datei `unity_config.h` nur ein, wenn dieses Symbol gesetzt ist. Fehlt es, läuft die Suite zwar, gibt aber nichts aus.

### 4. Heap und Stack vergrößern

*Properties → Build → MSP430 Linker → Basic Options*

| Einstellung | Wert |
|---|---|
| Heap size for C/C++ dynamic memory allocation | `320` |
| Set C system stack size | `256` |

Die Voreinstellung eines neuen CCS-Projekts reicht für die Textausgabe nicht aus. Ist der Heap zu klein, erscheint **kommentarlos nichts** in der Konsole — ohne Fehler und ohne Warnung.

### 5. Einstellungen übernehmen

*Apply and Close*, danach einmal *Project → Clean…*

### 6. Schalter in `main.c` einbauen

Ergänzen Sie Ihr bestehendes `main.c` um die markierten Zeilen. Der Code aus dieser Einheit bleibt dabei unverändert stehen.

```c
#include <msp430.h>
#include "halGeneral.h"
#include "halGPIO.h"

#define RUN_TESTS                   // auskommentieren für den Normalbetrieb

#ifdef RUN_TESTS
#include "testHalGeneral.h"
#endif

extern volatile ButtonCom CCButton;

void main(void)
{
    halInit();

#ifdef RUN_TESTS
    halInitTest();
#endif

    while(1)
    {
        // Ihre Tasterauswertung aus dieser Einheit
    }
}
```

Die Tests laufen einmal beim Start, danach arbeitet das Programm normal weiter — die Taster funktionieren anschließend wie gewohnt. Zum Abschalten genügt es, die Zeile `#define RUN_TESTS` auszukommentieren.

---

## Testlauf

1. Projekt bauen und über *Debug* auf den Controller laden.
2. **Resume** drücken. Das Programm muss tatsächlich laufen — steht es an einem Breakpoint, erscheint keine Ausgabe.
3. In der **Console**-View über das Symbol *Display Selected Console* auf den Eintrag **`<Projektname>:CIO`** umschalten.

> **Hinweis:** Nach jedem Build wechselt CCS automatisch zurück auf die Build-Konsole. Sie müssen also erneut umschalten.

---

## Die Ausgabe lesen

```
testHalGPIO.c:30:startButtonSetsEventFlag:PASS
testHalGPIO.c:36:startButtonReportsCorrectKey:PASS
testHalGPIO.c:42:stopButtonSetsEventFlag:PASS
testHalGPIO.c:48:stopButtonReportsCorrectKey:PASS
testHalGPIO.c:54:interruptFlagIsCleared:PASS

-----------------------
5 Tests 0 Failures 0 Ignored
OK
```

Jede Zeile nennt Datei, Zeilennummer und Testnamen. Schlägt ein Fall fehl, kommen Soll- und Istwert dazu:

```
testHalGPIO.c:48:stopButtonReportsCorrectKey:FAIL: Expected 128 Was 64
```

Prüfen Sie immer die **Gesamtzahl** am Ende. Läuft ein Test nicht, fehlt er in der Liste, ohne dass eine Fehlermeldung erscheint.

---

## Aufbau eines Testfalls

```c
static void startButtonSetsEventFlag(void)
{
    triggerButton(START_BUTTON);
    TEST_ASSERT_EQUAL_UINT8(1, CCButton.active);
}
```

Dahinter steckt die Hilfsfunktion, die den Ausgangszustand herstellt und das Ereignis auslöst:

```c
static void triggerButton(unsigned char button)
{
    unsigned int timeout = 60000;

    CCButton.active = 0;
    CCButton.button = 0;

    P1IFG |= button;                            // trigger interrupt by software

    while (CCButton.active == 0 && timeout > 0) timeout--;
}
```

Zwei Punkte sind daran bemerkenswert:

Das Interrupt-Flag wird **per Software** gesetzt. Für den Controller ist das nicht von einem echten Tastendruck zu unterscheiden — die ISR läuft genauso an. Deshalb lässt sich auch Interruptcode automatisiert prüfen, ohne dass jemand eine Taste drückt.

Die Warteschleife bricht nach einer festen Zahl Durchläufe ab. Läuft die ISR nie an, schlägt der Test fehl, statt das Programm einzufrieren.

Jeder Testfall folgt demselben Dreischritt: **Ausgangszustand herstellen**, **Ereignis auslösen**, **Ergebnis prüfen**. Und jeder prüft genau eine Aussage — schlägt ein Fall fehl, soll sofort klar sein, was nicht stimmt.

Dass `triggerButton()` die Struktur zurücksetzt **und** `tearDown()` dasselbe nochmal tut, ist Absicht: Der Testfall sorgt für seinen eigenen Ausgangszustand, `tearDown()` dafür, dass nach dem Durchlauf nichts hängen bleibt.

---

## Assertions

| Makro | Prüft |
|---|---|
| `TEST_ASSERT_EQUAL_UINT8(soll, ist)` | 8-Bit-Wert |
| `TEST_ASSERT_EQUAL_UINT(soll, ist)` | vorzeichenlose Ganzzahl |
| `TEST_ASSERT_EQUAL_INT(soll, ist)` | vorzeichenbehaftete Ganzzahl |
| `TEST_ASSERT_TRUE(bedingung)` | Bedingung ist wahr |
| `TEST_ASSERT_FALSE(bedingung)` | Bedingung ist falsch |
| `TEST_ASSERT_BITS_HIGH(maske, wert)` | alle Bits der Maske sind gesetzt |
| `TEST_ASSERT_BITS_LOW(maske, wert)` | alle Bits der Maske sind gelöscht |
| `TEST_ASSERT_GREATER_THAN(grenze, ist)` | Wert ist größer |
| `TEST_ASSERT_LESS_THAN(grenze, ist)` | Wert ist kleiner |

Die Reihenfolge ist immer **Sollwert zuerst, Istwert danach**. Vertauscht man sie, läuft der Test zwar, aber die Fehlermeldung nennt die Werte verkehrt herum.

Gleitkomma-Assertions sind in `unity_config.h` abgeschaltet: Sie kosten auf dem MSP430 viel Rechenzeit und Programmspeicher.

---

## Fehlersuche

**Keine Ausgabe in der Konsole**

- Heap zu klein → auf `320` setzen, danach *Clean*
- Programm läuft nicht → nach *Debug* unbedingt *Resume* drücken
- Falsche Konsole → auf `<Projektname>:CIO` umschalten
- Zu viele Breakpoints → die Textausgabe belegt selbst einen Breakpoint. Über *Run → Remove All Breakpoints* alle löschen und neu starten.

**`#include file not found: unity.h`**

Include-Pfade fehlen, siehe Schritt 2.

**`unresolved symbol setUp` / `tearDown`**

`testHalGeneral.c` ist nicht im Build. Prüfen, ob die Datei im Projektbaum erscheint, notfalls **F5** im Project Explorer.

**`unresolved symbol CCButton`**

Schreibweise der globalen Variablen stimmt zwischen Deklaration und Definition nicht überein. Groß- und Kleinschreibung beachten, und `volatile` muss an beiden Stellen gleich sein.

**Alle Tests schlagen fehl**

Prüfen Sie zuerst, ob die Interrupts überhaupt freigegeben sind: `__enable_interrupt()` muss am Ende von `halInit()` stehen. Ohne gesetztes GIE-Bit läuft keine ISR an, und jeder Testfall wartet vergeblich.

---

## [AUFGABE] Testsuite in Betrieb nehmen

1. Richten Sie das Testframework nach den Schritten 1 bis 6 ein.
2. Lassen Sie die Suite laufen. Alle fünf Testfälle müssen `PASS` melden.
3. Kommentieren Sie `#define RUN_TESTS` aus und überzeugen Sie sich, dass das Programm wieder normal arbeitet.
4. [Meilensteinüberprüfung] Sehen Sie sich `testHalGPIO.c` an: Wie löst ein Testfall den Interrupt aus, ohne dass eine Taste gedrückt wird?
5. [Meilensteinüberprüfung] Warum enthält `triggerButton()` einen Zähler als Abbruchbedingung?

---

In der nächsten Einheit schreiben Sie eigene Testfälle und ergänzen damit die bereitgestellte Suite.

[⬆ Zurück zur Aufgabenstellung](../README.md)