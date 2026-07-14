[⬅ Zurück zur Kapitelübersicht](../README.md#kapitelübersicht--aufgabenstellungen)

# DMA – Direktes Speicherzugriffsmodul (MSP430)

## Inhalt
- [Theorie: DMA – Direktzugriffsmodul (MSP430)](#theorie-dma--direktzugriffsmodul-msp430)
- [Grundprinzip](#grundprinzip)
- [Transfer Modes](#transfer-modes)
- [Addressing Modes](#addressing-modes)
- [Weitere zentrale DMA-Register (pro Kanal)](#weitere-zentrale-dma-register-pro-kanal)
- [DMA Konfiguration](#dma-konfiguration)
- [Optional: Erweiterungsideen](#optional-erweiterungsideen)

**Laborübung**

- *MSP430x5xx and MSP430x6xx Family User Guide Rev. O* – Texas Instruments  
  - Kapitel 11.1: [Introduction](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=381)  
    - Beschreibung  
    - Figure 11-1: DMA Controller Block Diagram  
  - Kapitel 11.2.1: [Addressing Modes](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=383)  
  - Kapitel 11.2.2: [Transfer Modes](https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/166/MSP430x6-Family-User-Guide.pdf#page=385)   

**Wissensüberprüfung**

- Was ist DMA und wozu wird es verwendet?
- Wie unterscheiden sich die verschiedenen Adressierungs- und Transfermodi?
- Warum ist DMA ressourcenschonender als CPU-gesteuerter Speichertransfer?

**Video**  
- [Einführungsvideo – Einheit 8](https://youtu.be/qNMKzkcaDQw?si=dnp_B49QR75IQHAJ)

---
## Theorie: DMA – Direktzugriffsmodul (MSP430)

DMA steht für *Direct Memory Access* und ermöglicht es, Daten zwischen Peripheriegeräten (z. B. ADC12MEMx) und dem RAM zu übertragen, **ohne dass die CPU aktiv eingreift**. Dadurch kann der Mikrocontroller effizient im Low-Power-Modus verbleiben und Rechenzeit einsparen.
Der **DMA-Controller** im MSP430F5335 besitzt **6 unabhängige Kanäle** (DMA0 bis DMA5), die jeweils über eigene Konfigurationsregister verfügen und unterschiedliche Aufgaben parallel ausführen können.

### Grundprinzip

Ein DMA-Transfer läuft schematisch wie folgt ab:

1. **Trigger-Ereignis** (z.B. ADC12IFG, DMAREQ, Timer, UART RX)
2. **Quelladresse** (z.B. ADC12MEM0)
3. **Zieladresse** (z.B. ADC12Com.ADCBuffer oder einzelne Elemene im Struct, wichtig ist dabei zu bedenken wo / wie die einzelnen Elemente im Speicher abgelegt sind.)
4. **Transfergröße & Modus**
5. **DMA erledigt Transfer autonom** → ISR optional (z. B. zur Setzung eines Ready-Flags)
---

### Transfer Modes

Die Transfermodi legen fest, wie und wann Daten vom Quell- zum Zielbereich übertragen werden. Sie werden im **`DMAxCTL`**-Register (x = 0..5) konfiguriert:

#### `DMAxCTL` – Bitfelder:

- **DMADTx (Bits 1–3)**: Transfer-Modus
- **DMAEN (Bit 0)**: DMA aktivieren
- **DMAIFG (Bit 0)**: Interrupt-Flag
- **DMAIE (Bit 4)**: Interrupt aktivieren

#### Übersicht der Modi:

| Modus | Kürzel  | Beschreibung                            |
|-------|---------|-----------------------------------------|
| `000` | Single Transfer     | Ein Transfer pro Trigger-Ereignis          |
| `001` | Block Transfer      | Mehrere Transfers pro Trigger              |
| `010`,`010` | Burst Block Transfer        | Schnelle Blockübertragung in einem Burst, blockiert CPU|
| `100` | Repeated Single     | Kontinuierlich Einzelübertragungen         |
| `101` | Repeated Block      | Kontinuierlich Blockübertragungen          |
| `101`,`111` | Repeated burst-block transfer | Schnelle Blockübertragung in einem Burst, blockiert CPU                                   |

```c
DMA0CTL = DMADT_4 | DMAEN | DMAIE;  // Repeated Block, DMA aktiv, Interrupt aktiv
```
---

### Addressing Modes

DMA kann entweder mit **fixen Adressen** (z. B. Peripherieregister) oder **inkrementierenden Speicheradressen** (z. B. Arrays) arbeiten. Die Modi werden über folgende Bits im `DMAxCTL`-Register gesetzt:

- **DMASRCINCR**: Quelladressierung
- **DMADSTINCR**: Zieladressierung

### Optionen:
| Bits     | Modus               | Beschreibung                                   |
|----------|---------------------|------------------------------------------------|
| `00`     | Static              | Adresse bleibt konstant                        |
| `01`     | Incremented         | Adresse wird nach jedem Transfer erhöht        |
| `10`     | Decremented         | Adresse wird reduziert                         |
| `11`     | Unchanged           | Reserviert oder nicht unterstützt              |

### Weitere zentrale DMA-Register (pro Kanal)

| Register        | Funktion                             |
|-----------------|--------------------------------------|
| `DMAxCTL`       | Konfiguration (Enable, Modus, IRQ)   |
| `DMAxSA`        | Source Address (z. B. ADC12MEM0)     |
| `DMAxDA`        | Destination Address (z. B. RAM)      |
| `DMAxSZ`        | Transfergröße (z. B. 4 Words)        |
| `DMACTL0`–`2`   | Triggerquellen pro Kanal             |

---
### Beispiel: DMA für ADC12

```c
// Quelladresse: Start ADC Speicher
// Zieladresse: Globaler RAM-Buffer
// 4 Transfers (z. B. 3 Sensoren + VBAT)

// Repeated Block Mode
// Zieladresse auto-increment
// Quelladresse fix
// Interrupt aktivieren
// DMA aktivieren
```
---
### Hinweis

- **DMA interrupt** (z. B. `DMA0_VECTOR`) kann genutzt werden, um nach erfolgreichem Transfer ein Flag zu setzen (`ADCrdy = 1`).
- **DMA kann auch mit Timer, UART, SPI oder externen Triggern** verwendet werden.

---
## DMA Konfiguration

Um die CPU bei der Datenübertragung vom ADC zu entlasten, wird ein DMA-Kanal (z. B. DMA0) eingesetzt. Dieser übernimmt nach Abschluss der Sample-Sequenz automatisch den Transfer der ADC-Daten in eine globale Speicherstruktur.

---
## [AUFGABE] Durchzuführende Arbeit & Dokumentation für die Meilensteinüberprüfung

1. **Neues HAL-Modul erstellen**  
   - `hal_dma.c` und `hal_dma.h`  
   - Struktur analog zu anderen HAL-Modulen

2. **Funktion `hal_Init()` implementieren**  
   - Konfigurieren Sie DMA0 für die Zusammenarbeit mit dem ADC  
   - Alternativ kann dies innerhalb der ADC-Initialisierung erfolgen – **Sehr empfehlenswert** ist es jedoch ein separates Modul, um den Verwendungszweck klar zu trennen

3. **ADC-Konfiguration anpassen**  
   - Ändern Sie `hal_ADCInit()` so, dass am Ende der Sample-Sequenz automatisch ein DMA-Transfer ausgelöst wird

4. **Status-Rückmeldung implementieren**  
   - Sobald der DMA den Transfer abgeschlossen hat, soll das globale Flag `ADCrdy` gesetzt werden  
   - Dieses Flag wird in der `main()`-Schleife abgefragt, um zu erkennen, ob neue Daten vorliegen

5. **Globale Speicherstruktur anlegen**  
   - Beispiel: Ringpuffer oder Array `adcResult[NUM_CHANNELS]`  
   - Zieladresse im DMA-Kanal entsprechend setzen

6. **Test & Dokumentation**
   - Nach erfolgreichem Übertragen soll wieder das `ADCrdy`-Flag gesetzt werden, um der main Funktion mitzuteilen, dass neue Sensorwerte eingelesen wurden.
   - Überprüfen Sie mit Breakpoints oder Speicheransicht die Aktualisierung der Zielstruktur
---

## Optional: Erweiterungsideen
- Mehrere DMA-Kanäle parallel nutzen (z. B. für UART oder SPI)
- DMA-Trigger auf Timer- oder Comparator-Events setzen (je nach Anwendungsfall)
---
## Referenzen

- **MSP430x5xx and MSP430x6xx Family User Guide**, Texas Instruments, Literature Number: SLAU208O, Rev. O, April 2019.  
  Verfügbar unter: [https://www.ti.com/lit/pdf/slau208](https://www.ti.com/lit/pdf/slau208)

- **MSP430F5335 Datasheet**, Texas Instruments, Document Number: SLAS590N, Rev. N, October 2018.  
  Verfügbar unter: [https://www.ti.com/lit/gpn/msp430f5335](https://www.ti.com/lit/gpn/msp430f5335)

[⬆ Zurück zum Hauptverzeichnis](../README.md#kapitelübersicht--aufgabenstellungen)
