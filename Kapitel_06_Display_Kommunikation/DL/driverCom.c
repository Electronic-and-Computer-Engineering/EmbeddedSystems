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
    unsigned char n = 0;

    // TODO 1: laeuft noch eine Uebertragung?  -> 0 zurueckgeben, nichts tun.
    //         Der HAL merkt sich den Zeiger auf comTxFrame, nicht den Inhalt -
    //         wer den Puffer ueberschreibt, waehrend gesendet wird, verschickt Bruch.

    // TODO 2: mehr Werte als COM_TX_MAX_VAL  -> auf das Maximum begrenzen.

    // TODO 3: Frame in comTxFrame[] aufbauen:
    //
    //            'C'  'S'   v0_high v0_low   v1_high v1_low   ...   'E'  'C'
    //
    //         Ein unsigned int sind zwei Bytes. Die Gegenstelle erwartet
    //         zuerst das hoeherwertige - siehe Kommunikation.md.
    //         Verwenden Sie COM_CAR_START_1/_2 und COM_CAR_END_1/_2,
    //         nicht die Zeichen selbst.
    //         n mitzaehlen, es ist am Ende die Framelaenge.

    // TODO 4: Frame abschicken und den Rueckgabewert von halComSend()
    //         durchreichen.

    (void)values; (void)count; (void)n;
    return 0;
}

unsigned char driverComReceive(unsigned char *cmd, unsigned int *value)
{
    unsigned char frame[COM_RX_LEN];

    // TODO 5: mit halComGetFrame() einen Frame abholen.
    //         Liefert sie 0, ist keiner da -> 0 zurueckgeben.
    //         Auf den Frameanfang 'H','S' muessen Sie nicht pruefen, darauf
    //         hat der HAL bereits synchronisiert.

    // TODO 6: Frameende pruefen: frame[COM_RX_END_1] und frame[COM_RX_END_2]
    //         muessen COM_HOST_END_1 und COM_HOST_END_2 sein. Passt es
    //         nicht, war es kein gueltiger Frame -> 0.
    //         Warum diese Pruefung trotz Sync-Muster noetig ist, steht
    //         im Unterkapitel.

    // TODO 7: cmd und value aus dem Frame auspacken und 1 zurueckgeben.
    //         Die Indizes stehen als COM_RX_... in driverCom.h.

    (void)frame; (void)cmd; (void)value;
    return 0;
}
