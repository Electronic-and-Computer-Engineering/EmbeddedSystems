[⬅ Zurück zur Aufgabenstellung](README.md)

# DMA am MSP430

## Inhalt
- [Wozu überhaupt](#wozu-überhaupt)
- [Was ein Kanal braucht](#was-ein-kanal-braucht)
- [Transfermodi](#transfermodi)
- [Adressmodi](#adressmodi)
- [Triggerquelle](#triggerquelle)
- [Register](#register)
- [Die Kopplung an den ADC12](#die-kopplung-an-den-adc12)

**Family User Guide**
- Kapitel 11: DMA Controller *(im [Family Guide](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf) über das Inhaltsverzeichnis)*
  - 11.1 Introduction und Figure 11-1 (Blockdiagramm)
  - 11.2.1 Addressing Modes
  - 11.2.2 Transfer Modes

---

## Wozu überhaupt

Ihre ADC-Serviceroutine tut nichts anderes, als vier Register in ein Array zu kopieren. Dafür wird bei jeder Sequenz der laufende Code unterbrochen, der Kontext gesichert, ein paar Zuweisungen ausgeführt und der Kontext wiederhergestellt.

Genau diese Arbeit kann der **DMA-Controller** übernehmen: Er kopiert Daten zwischen Speicherstellen, ohne dass die CPU beteiligt ist. Der MSP430F5335 hat dafür **sechs unabhängige Kanäle** (DMA0 bis DMA5).

Der Gewinn ist dreifach:

- **Keine Interruptlast pro Messwert.** Bei 120 Hz und vier Kanälen sind das 480 eingesparte Registerzugriffe pro Sekunde samt Kontextwechsel.
- **Die Daten sind vollständig oder gar nicht da.** Die CPU sieht den Puffer erst, wenn der ganze Block übertragen ist – kein halb gefülltes Array.
- **Der Controller kann schlafen.** Ein DMA-Transfer läuft auch im Low-Power-Modus, die CPU muss dafür nicht aufwachen.

> Der DMA stiehlt sich seine Takte vom Bus: Während eines Transfers steht die CPU für einen Takt. Bei vier Words pro Sequenz ist das nicht messbar – bei einem Burst-Block über hunderte Words schon.

---

## Was ein Kanal braucht

Ein DMA-Kanal ist mit vier Angaben vollständig beschrieben:

| Angabe | Register | Beispiel |
|---|---|---|
| **Woher** | `DMAxSA` | `&ADC12MEM0` |
| **Wohin** | `DMAxDA` | `&sensor_Data.ADCBuffer[0]` |
| **Wie viel** | `DMAxSZ` | 4 Words |
| **Wann** | `DMACTL0` | ADC12IFG |

Dazu kommt in `DMAxCTL`, **wie** übertragen wird: Transfermodus, Verhalten der beiden Adressen, Wortbreite und ob ein Interrupt kommen soll.

---

## Transfermodi

`DMADTx` in `DMAxCTL` legt fest, wie viel pro Triggerereignis läuft und ob der Kanal danach aktiv bleibt:

| Makro | Modus | Pro Trigger | Danach |
|---|---|---|---|
| `DMADT_0` | Single | ein Wort | `DMAEN` wird nach `DMAxSZ` Transfers gelöscht |
| `DMADT_1` | Block | ganzer Block | `DMAEN` gelöscht |
| `DMADT_2`, `DMADT_3` | Burst-Block | ganzer Block, CPU blockiert | `DMAEN` gelöscht |
| `DMADT_4` | Repeated Single | ein Wort | bleibt aktiv |
| `DMADT_5` | **Repeated Block** | ganzer Block | bleibt aktiv |
| `DMADT_6`, `DMADT_7` | Repeated Burst-Block | ganzer Block, CPU blockiert | bleibt aktiv |

Die Unterscheidung, die hier zählt, ist **einmalig gegen wiederholt**. Bei allen nicht wiederholten Modi löscht die Hardware `DMAEN`, sobald `DMAxSZ` abgearbeitet ist – der nächste Transfer müsste von Hand wieder freigegeben werden. Für eine laufende Messung ist das falsch.

---

## Adressmodi

`DMASRCINCRx` und `DMADSTINCRx` bestimmen, was nach jedem einzelnen Transfer mit der jeweiligen Adresse passiert:

| Bits | Makro | Verhalten |
|---|---|---|
| `00`, `01` | `_0`, `_1` | Adresse bleibt stehen |
| `10` | `_2` | Adresse wird verringert |
| `11` | `_3` | Adresse wird erhöht |

Wichtig: Beim Start eines Blocks werden beide Adressen auf die Werte zurückgesetzt, die in `DMAxSA` und `DMAxDA` stehen. Der nächste Block schreibt also wieder an den Anfang des Puffers – ohne dass im Programm etwas nachgeführt werden müsste.

Für vier aufeinanderfolgende Ergebnisregister in ein Array heißt das: **beide Seiten inkrementierend**. `ADC12MEM0` bis `ADC12MEM3` liegen lückenlos hintereinander im Adressraum, genau wie die vier Elemente des Zielarrays.

> **Deshalb ist der Zielpuffer ein Array und kein Struct mit vier benannten Feldern.** Bei einem Array garantiert die Sprache, dass die Elemente lückenlos und in Reihenfolge liegen. Bei einem Struct entscheidet der Compiler über Ausrichtung und Reihenfolge – der DMA würde dann irgendwohin schreiben.

Die Wortbreite steht in `DMASRCBYTE` und `DMADSTBYTE`. Der ADC liefert 12 Bit in einem 16-Bit-Register, Quelle und Ziel sind also beide **Word** – der Reset-Zustand, es ist nichts zu setzen.

---

## Triggerquelle

Welches Ereignis den Kanal auslöst, steht nicht in `DMAxCTL`, sondern in den Registern `DMACTL0` bis `DMACTL2` – zwei Kanäle pro Register:

```c
DMACTL0 |= DMA0TSEL_24;      // Kanal 0 wird von ADC12IFG getriggert
```

Die Kennzahl `24` ist der Eintrag für den ADC12 in der Trigger-Tabelle des Family User Guide. Dort stehen auch alle anderen: Timer, USCI, Komparator, ein externer Pin, oder `DMAREQ` als reiner Softwareauslöser.

> **Das Flag wird nicht von Ihnen gelöscht.** Der DMA quittiert `ADC12IFG` selbst, indem er `ADC12MEMx` liest. Wer zusätzlich den ADC-Interrupt freigibt, bekommt deshalb eine ISR, die ein längst geleertes Register ausliest.

---

## Register

| Register | Relevante Bits | Bedeutung |
|---|---|---|
| `DMAxCTL` | `DMADTx` | Transfermodus |
| | `DMASRCINCRx`, `DMADSTINCRx` | Verhalten der Adressen |
| | `DMASRCBYTE`, `DMADSTBYTE` | Wortbreite, gelöscht = Word |
| | `DMAEN` | Kanal aktiv |
| | `DMAIE` | Interrupt nach vollständigem Block |
| | `DMAIFG` | Interrupt-Flag |
| | `DMAABORT` | Transfer wurde abgebrochen |
| `DMAxSA` | – | Quelladresse, 20 Bit |
| `DMAxDA` | – | Zieladresse, 20 Bit |
| `DMAxSZ` | – | Anzahl der Transfers je Block |
| `DMACTL0` … `2` | `DMAxTSELx` | Triggerquelle je Kanal |
| `DMAIV` | – | Vektorregister, alle sechs Kanäle teilen sich einen Interruptvektor |

Adressregister sind 20 Bit breit, Zeiger im Small-Memory-Model 16 Bit. Die Zuweisung braucht deshalb eine Typumwandlung:

```c
DMA0SA = (__SFR_FARPTR)&ADC12MEM0;
```

Ohne sie warnt der Compiler – und je nach Einstellung landet eine abgeschnittene Adresse im Register.

---

## Die Kopplung an den ADC12

Alles zusammen ergibt diese Kette:

```
TB0.1  ──►  ADC12 misst A0..A3  ──►  ADC12IFG  ──►  DMA0 kopiert 4 Words
                                                         │
                                                         ▼
                                              Block fertig  →  DMA-ISR
                                                         │
                                                         ├─ Mittelwert bilden
                                                         ├─ ADCrdy = 1
                                                         └─ ADC12ENC aus/ein
```

Die CPU läuft erst beim letzten Pfeil wieder los – einmal pro Sequenz statt viermal.

**Der letzte Schritt ist der unauffälligste und der wichtigste.** Nach einer abgeschlossenen Sequenz nimmt der ADC12 keinen neuen Trigger an, solange `ADC12ENC` gesetzt bleibt. Das Bit muss gelöscht und wieder gesetzt werden, sonst läuft die Messung genau einmal:

```c
ADC12CTL0 &= ~ADC12ENC;
ADC12CTL0 |=  ADC12ENC;
```

Ein Fehler an dieser Stelle sieht im Debugger aus wie ein DMA-Problem: Der Puffer enthält plausible Werte, sie ändern sich nur nie wieder.

---

[⬆ Zurück zur Aufgabenstellung](README.md)
