[⬅ Zurück zur Kapitelübersicht](../README.md#kapitelübersicht--aufgabenstellungen)
# Clock System, TimerB0 Konfiguration

## Inhalt

**Laborübung**

- *MSP430x5xx and MSP430x6xx Family User Guide Rev. O* – Texas Instruments
  - Kapitel 5: Unified Clock System [Family Guide - Unified Clock System](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=158)
  - Kapitel 18: Timer B [Family Guide - Timer B](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=484)
- Crazy Car Controller FHJ Schaltplan (Quarz, Clock) [Crazy Car Schematic](https://fhjoanneum-my.sharepoint.com/:b:/g/personal/florian_mayer_fh-joanneum_at/EfXYu-rqsLRErJbybsbN4AEB_RUMizJhwpb5D_ysimZehA?e=Ti7PtO)

**Wissensüberprüfung**

- Kapitel 5.1: [Unified Clock System (UCS) Introduction](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=159)
  - Clock Sources
  - UCS Clock Signals
- Kapitel 18.1: [Timer B Introduction](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=485)
  - Beschreibung
  - Timer-Bitbreite
  - Blockschaltbild
- Kapitel 18.2.3: [Timer B Mode Control](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=488)
  - Table 18-1. Timer Modes
- Kapitel 18.2.3.1: [Up Mode](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=488)
  - Zusammenhang: Eingangsfrequenz → Timer-Teiler (TBxCL0, TBxCCR0) → Ausgangsfrequenz

**!!Taschenrechner für Frequenzberechnung empfohlen!!**

**Video**
 - [Einführungsvideo - Einheit 3](https://youtu.be/PKbX4UOLLyc?si=cjtgWto6aFb2LdDT)

## Durchzuführende Aufgaben
- [[AUFGABE] Konfiguration des UCS](#durchzuführende-arbeit--dokumentation-für-die-meilensteinüberprüfung)
- [[AUFGABE] Konfiguration und Aufsetzen des TimerB0](#durchzuführende-arbeit--dokumentation-für-die-überprüfung-der-meilensteine)
---

## Power Management Module (PMM)

Das PMM wird in dieser Laborübung nicht detailliert behandelt, eine Grundkonfiguration ist jedoch notwendig, um den MSP430 mit 20 MHz betreiben zu können. Binden Sie hierzu `hal_pmm.c` und die zugehörige Header-Datei gemäß [Anlegen eines Projektes](../Kapitel_01_Einfuehrung/README.md#projekt-erstellen) ein.

---

## Unified Clock System (UCS)

Um die Peripherie und Systemkomponenten im MSP430 korrekt zu betreiben, muss das Taktungssystem konfiguriert werden. Zwar ist initial eine Default-Konfiguration aktiv, für Hochfrequenzbetrieb (z. B. 20 MHz) sind jedoch explizite Schritte notwendig.
Ist ein Quarz angeschlossen, so muss dieser per Pinmux freigeschaltet und das UCS-Modul entsprechend parametriert werden. Achten Sie auf Frequenzvorgaben, Genauigkeit und Energieverbrauch bei der Auswahl der Taktquelle.

### Durchzuführende Arbeit & Dokumentation für die Meilensteinüberprüfung

1. **Konfigurieren Sie die XT2-Pins** in `hal_gpio.c`, sodass der Quarz korrekt mit dem UCS-Modul verbunden ist. *([siehe MSP430F5335 Datasheet, S. 92](https://www.ti.com/lit/ds/symlink/msp430f5335.pdf))*

2. **Neues HAL-Modul:** `hal_ucs.c/.h`
   - Implementieren Sie `hal_ucsInit()` und rufen Sie diese Funktion innerhalb von `hal_Init()` auf.

   **Hilfestellung:** Register laut [Family User Guide Kapitel 5](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=159): `UCSCTLx`

   ```c
   void hal_ucsInit(void)
   {
       UCSCTL6 &= ~XT2OFF;             // XT2 einschalten
       UCSCTL3 |= SELREF_2;            // FLL Referenz = REFO
       UCSCTL4 |= SELA_2;              // ACLK = REFO

       // Warten bis Fehlerflags gelöscht sind
       while (SFRIFG1 & OFIFG)
       {
           UCSCTL7 &= ~(XT2OFFG + DCOFFG + XT1HFOFFG + XT1LFOFFG);
           SFRIFG1 &= ~OFIFG;
       }

       UCSCTL6 &= ~(XT2DRIVE_3);       // Drive-Strength reduzieren (nach Anlauf)
       UCSCTL4 |= SELS_5 + SELM_5;     // SMCLK und MCLK = XT2
       UCSCTL5 |= DIVS_3;              // SMCLK Divider setzen (z. B. /8)
   }
   ```
   **Was passiert hier?** Der Oszillator XT2 wird aktiviert, Fehlerflags werden gelöscht, und die Systemtakte (MCLK, SMCLK) werden auf die Quarzquelle umgelegt. Der korrekte Betrieb wird durch Abfrage der Fault-Flags abgesichert.

3. **Taktkonfiguration**
   - MCLK = 20 MHz
   - SMCLK = 2.5 MHz (z. B. über Divider /8)
   - ACLK = beliebig (z. B. REFO = 32.768 Hz)

4. **Frequenzmessung:**
   - SMCLK an GPIO ausgeben und mit Oszilloskop messen. Dazu den jeweiligen Pin in `hal_gpio.c` korrekt konfigurieren.

5. **Frequenzen im Header definieren:**
   ```c
   #define XTAL_FREQU   20000000
   #define MCLK_FREQU   20000000
   #define SMCLK_FREQU   2500000
   ```

   *(Optional: Prüfen Sie, ob sich durch Anpassen des SMCLK-Dividers verschiedene Timerfrequenzen effizient ableiten lassen.)*

---

## Timer Interrupts – Timer B0

Applikationen, die auf Mikrocontrollern ablaufen, erfordern meist ein genaues Timing. Dieses Timing bzw. zyklische Aufrufen von Funktionen, Routinen oder Triggern kann durch einen Timer realisiert werden. Diese Timer sind Teil der Peripherie des Mikrocontrollers und laufen in Hardware – verbrauchen somit keine CPU-Rechenzeit.

Der TimerB0 soll so konfiguriert werden, dass die Hintergrundbeleuchtung des Displays im 2 Hz-Takt blinkt. Verwenden Sie dazu das Capture/Compare-Register 0 (`TBCCR0`) und den `SMCLK` als Eingangstaktquelle.

**Hilfestellung:** [Family User Guide (Kapitel 18, Seite 482)](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=482). Achten Sie auf die Konfiguration von: `TBxCTL`, `TBxCCTL0`, `TBxEX0`, `TBxCCR0`

Interrupt Vector: `TIMERB0_VECTOR` (für CCR0)

### Durchzuführende Arbeit & Dokumentation für die Überprüfung der Meilensteine

1. Erstellen Sie ein neues HAL-Modul `hal_timerB0.c` und `hal_timerB0.h`, und schreiben Sie eine Funktion `hal_TimerB0Init()`. Diese soll in der `hal_Init()` Funktion aufgerufen werden.

2. Konfigurieren Sie den Timer entsprechend der Vorgaben. Schalten Sie die Hintergrundbeleuchtung des Displays mittels Makros in der ISR des Timers ein bzw. aus. **Tipp:** Toggle I/O Pin.

3. Verwenden Sie für die Berechnung des Timer-Werts die im UCS-Modul definierten Werte. Definieren Sie auch die Ausgangsfrequenz des Timers mittels `#define`.

4. Fragen Sie in der ISR das richtige Interrupt-Flag des Timers ab.

5. Kontrollieren und dokumentieren Sie die Frequenz der Hintergrundbeleuchtung mit dem Oszilloskop.

### Zusätzlich Fragen für die Meilensteinüberprüfung

- Welche Vorteile bietet Timer-Hardware gegenüber softwarebasierter Zeitsteuerung?
- Wie viele unabhängige Timer besitzt der MSP430F5335, und wie unterscheiden sich TimerA und TimerB?
- Wie beeinflusst die Auswahl von SMCLK-Divider und TBxEX0-Divider die maximale Auflösung und Reichweite des Timers?
- Kann ein zweiter Timer für PWM-Erzeugung parallel verwendet werden, ohne den ersten zu beeinflussen?
---

Falls im späteren Verlauf PWM, ADC-Trigger oder Watchdog-Funktionalitäten benötigt werden, sollten die vorhandenen Timerkanäle effizient verwaltet werden.

[⬆ Zurück zum Hauptverzeichnis](../README.md#kapitelübersicht--aufgabenstellungen)
