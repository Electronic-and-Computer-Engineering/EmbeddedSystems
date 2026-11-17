/*
 * halDMA.c
 *
 * UNVOLLSTAENDIG - an jedem TODO fehlt Code.
 *
 * Vorgegeben: die Serviceroutine. Sie enthaelt zwei Dinge, die man sich nicht
 * herleiten kann - die Mittelung und das Umschalten von ADC12ENC.
 * Ihre Arbeit: halDMAInit().
 */

#include <msp430.h>
#include "halADC12.h"
#include "halDMA.h"

extern volatile ADC12Com sensor_Data;


/* ########## Ihre Arbeit ########## */

void halDMAInit(void)
{
    // TODO 1: Transfermodus waehlen.
    //         Gesucht ist "repeated block transfer": ein ganzer Block wird
    //         uebertragen, danach bleibt der Kanal scharf und der naechste
    //         Block laeuft ohne neues Zutun. Family User Guide, Kapitel DMA,
    //         Register DMAxCTL, Feld DMADT.

    // TODO 2: Quell- und Zieladresse muessen nach jedem Wort weiterruecken.
    //         Zwei Felder im selben Register: DMASRCINCR und DMADSTINCR.

    // TODO 3: Interrupt des Kanals freigeben und ein eventuell anstehendes
    //         Flag loeschen.

    // TODO 4: Ausloeser waehlen. Die DMA soll nicht von Hand gestartet werden,
    //         sondern von der Interrupt-Flagge des ADC12. Das steht in DMACTL0,
    //         Feld DMA0TSEL. Die Nummer der Quelle finden Sie in der
    //         DMA-Trigger-Tabelle des Family User Guide.

    // TODO 5: Quelladresse = erstes Ergebnisregister des ADC (ADC12MEM0),
    //         Zieladresse  = sensor_Data.ADCBuffer[0].
    //
    //         Beide Register sind 20 Bit breit. Der Compiler erwartet dafuer
    //         eine Umwandlung, sonst meckert er oder schreibt eine halbe
    //         Adresse:
    //
    //             DMA0SA = (__SFR_FARPTR)&ADC12MEM0;
    //
    //         Die Zieladresse schreiben Sie nach demselben Muster.

    // TODO 6: Blockgroesse setzen - DMA_BLOCK_SIZE Woerter.

    // TODO 7: Kanal einschalten.
}


/* ########## vorgegeben ########## */

/*
 * Laeuft, sobald ein vollstaendiger Block im Puffer steht - also nach jeder
 * ADC-Sequenz, rund 120 mal je Sekunde. Die CPU hat bis hierher nichts getan.
 */
#pragma vector = DMA_VECTOR
__interrupt void DMAISR(void)
{
    unsigned char i;

    // gleitender Mittelwert ueber zwei Messungen:
    // neuer Wert = (alter Wert + frischer Wert) / 2
    for (i = 0; i < DMA_BLOCK_SIZE; i++)
    {
        sensor_Data.ADCFiltered[i] = (sensor_Data.ADCFiltered[i] + sensor_Data.ADCBuffer[i]) >> 1;
    }

    sensor_Data.Status.B.ADCrdy = 1;        // ein frischer Satz liegt vor

    /*
     * ADC12ENC aus- und wieder einschalten.
     *
     * Ohne diese zwei Zeilen laeuft genau eine Sequenz und danach nie wieder.
     * Der ADC betrachtet die Sequenz als abgeschlossen und muss fuer die
     * naechste neu freigegeben werden. Das steht so in keinem Beispiel, kostet
     * aber einen halben Labornachmittag, wenn es fehlt.
     */
    ADC12CTL0 &= ~ADC12ENC;
    ADC12CTL0 |=  ADC12ENC;

    DMA0CTL &= ~DMAIFG;                     // Flag quittieren
}
