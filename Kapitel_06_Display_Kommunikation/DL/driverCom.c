/*
 * driverCom.c
 *
 * UNVOLLSTAENDIG - an jedem TODO fehlt Code.
 *
 * Dieses Modul kennt nur noch halCom...() - welches UART-Modul sich dahinter
 * verbirgt, entscheidet COM_USE_BT in driverCom.h. Schreiben Sie hier deshalb
 * nirgends halUSCIA0... oder halUSCIA1... hin, sonst ist die Umschaltung hin.
 */

#include "driverCom.h"
#include "halUSCIA0.h"
#include "halUSCIA1.h"   // beide einbinden, die Auswahl macht COM_USE_BT

const unsigned char comSyncHost[2] = {COM_HOST_START_1, COM_HOST_START_2};    // Frameanfang vom Host
unsigned char comTxFrame[COM_TX_MAX_LEN];           // wird vom HAL per Zeiger gesendet

/*
 * Fester Satz Testwerte. Vier Faelle, die jeweils etwas anderes pruefen:
 *
 *       0      Randfall, beide Bytes null
 *    4095      braucht beide Bytes; bei vertauschter Reihenfolge
 *              erscheint am PC 65295 statt 4095
 *   65535      Maximalwert, alle 16 Bit gesetzt
 *     -40      negativ. Uebertragen wird das 16-Bit-Muster, also 65496.
 *              Dass -40 gemeint ist, weiss nur, wer den Wert auswertet.
 */
const unsigned int comTestVals[COM_TEST_COUNT] =
{
    0u,
    4095u,
    65535u,
    (unsigned int)(-40)
};


/* ########## vorgegeben ########## */

void driverComInit(void)
{
    halComInit(COM_BAUDRATE);
    halComStart(comSyncHost, 2, COM_RX_LEN);        // ab jetzt sucht der HAL nach 'H','S'
}


/* ########## Ihre Arbeit ########## */

unsigned char driverComSend(const unsigned int *values, unsigned char count)
{
    unsigned char i, n = 0;

    // TODO 1: Laeuft noch eine Uebertragung? -> 0 zurueckgeben, nichts tun.
    //         halComSend() merkt sich den Zeiger auf comTxFrame, nicht den
    //         Inhalt - wer waehrend des Sendens hineinschreibt, verschickt Bruch.

    // TODO 2: Mehr als COM_TX_MAX_VAL Werte -> auf das Maximum begrenzen.

    comTxFrame[n++] = COM_CAR_START_1;              // Car
    comTxFrame[n++] = COM_CAR_START_2;              // Start

    for (i = 0; i < count; i++)
    {
        // TODO 3: Jeden Wert in zwei Bytes zerlegen und ablegen.
        //         Die Gegenstelle erwartet das hoeherwertige zuerst.
        //         n dabei mitzaehlen.
    }

    comTxFrame[n++] = COM_CAR_END_1;                // End
    comTxFrame[n++] = COM_CAR_END_2;                // Car

    return halComSend(comTxFrame, n);               // n ist jetzt die Rahmenlaenge
}


unsigned char driverComReceive(unsigned char *cmd, unsigned int *value)
{
    unsigned char frame[COM_RX_LEN];

    if (halComGetFrame(frame) == 0) return 0;       // kein vollstaendiger Rahmen da

    // TODO 4: Rahmenende pruefen - frame[COM_RX_END_1] und frame[COM_RX_END_2]
    //         muessen COM_HOST_END_1 und COM_HOST_END_2 sein.
    //         Passt es nicht: 0 zurueckgeben, *cmd und *value NICHT anfassen.

    // TODO 5: cmd und value auspacken.
    //         Die beiden Wertbytes wieder zu einem 16-Bit-Wert zusammensetzen,
    //         in derselben Reihenfolge wie beim Senden. Die Indizes stehen
    //         als COM_RX_... in driverCom.h.

    return 1;
}

