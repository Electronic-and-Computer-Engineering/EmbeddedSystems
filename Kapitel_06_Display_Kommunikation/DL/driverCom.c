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

const unsigned char comSyncHost[2] = {'H', 'S'};    // Rahmenanfang vom Host
unsigned char comTxFrame[COM_TX_MAX_LEN];           // wird vom HAL per Zeiger gesendet


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

    // TODO 3: Rahmen in comTxFrame[] aufbauen:
    //
    //            'C'  'S'   v0_high v0_low   v1_high v1_low   ...   'E'  'C'
    //
    //         Ein unsigned int sind zwei Bytes. Die Gegenstelle erwartet
    //         zuerst das hoeherwertige - siehe Kommunikation.md.
    //         n mitzaehlen, es ist am Ende die Rahmenlaenge.

    // TODO 4: Rahmen abschicken und den Rueckgabewert von halComSend()
    //         durchreichen.

    (void)values; (void)count; (void)n;
    return 0;
}

unsigned char driverComReceive(unsigned char *cmd, unsigned int *value)
{
    unsigned char frame[COM_RX_LEN];

    // TODO 5: mit halComGetFrame() einen Rahmen abholen.
    //         Liefert sie 0, ist keiner da -> 0 zurueckgeben.
    //         Auf den Rahmenanfang 'H','S' muessen Sie nicht pruefen, darauf
    //         hat der HAL bereits synchronisiert.

    // TODO 6: Rahmenende pruefen: frame[5] und frame[6] muessen 'E' und 'H'
    //         sein. Passt es nicht, war es kein gueltiger Rahmen -> 0.
    //         Warum diese Pruefung trotz Sync-Muster noetig ist, steht
    //         im Unterkapitel.

    // TODO 7: cmd und value aus dem Rahmen auspacken und 1 zurueckgeben.
    //         Die Indizes stehen als COM_RX_... in driverCom.h.

    (void)frame; (void)cmd; (void)value;
    return 0;
}
