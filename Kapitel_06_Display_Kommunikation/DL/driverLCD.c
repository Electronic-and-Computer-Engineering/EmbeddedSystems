/*
 * driverLCD.c
 *
 * UNVOLLSTAENDIG - an jedem TODO fehlt Code.
 *
 * Vorgegeben: der Weg eines Bytes zum Display (lcdSend, WriteCommand,
 * WriteData), die Initialisierungssequenz und das Setzen der Position.
 * Ihre Arbeit: Clear, WriteText, WriteUInt.
 */

#include "driverLCD.h"
#include "driverFonttable.h"
#include "halGPIO.h"
#include "halUSCIB1.h"
#include "halUCS.h"

extern volatile USCIB1_SPICom LCD_Data;        // aus Ihrem SPI-Modul, Einheit 5

const unsigned char emptyPage[LCD_MAX_COLM] = {0};
#define FONT_CHAR_COUNT  (sizeof(font_table) / sizeof(font_table[0]))


/* ########## vorgegeben ########## */

/*
 * Der Weg jedes Bytes zum Display: warten, bis die vorige Uebertragung
 * durch ist, A0 passend setzen, Nutzdaten in den Sendepuffer des SPI-Moduls
 * kopieren und die Uebertragung anstossen. Das Chip Select setzt
 * halUSCIB1Transmit() aus Einheit 5, nicht diese Funktion.
 */
void lcdSend(const unsigned char *data, unsigned char dataLength, unsigned char isData)
{
    unsigned char i;

    while (LCD_Data.Status.B.TxSuc == 0);   // vorherige Uebertragung abwarten

    if (isData) halSetLCDData();            // A0 = 1: die Bytes sind Bildpunkte
    else        halSetLCDCmd();             // A0 = 0: die Bytes sind Befehle

    for (i = 0; i < dataLength; i++)
    {
        LCD_Data.TxData.Data[i] = data[i];
    }
    LCD_Data.TxData.len = dataLength;

    halUSCIB1Transmit();
}

void driverLCDWriteCommand(const unsigned char *data, unsigned char dataLength)
{
    lcdSend(data, dataLength, 0);
}

void driverLCDWriteData(const unsigned char *data, unsigned char dataLength)
{
    lcdSend(data, dataLength, 1);
}

/*
 * Hardware-Reset ueber eine eigene Leitung, danach die neun Befehle aus dem
 * Datenblatt. Die Folge haengt davon ab, wie das Panel auf der Platine
 * verbaut ist - sie laesst sich nicht herleiten, deshalb ist sie vorgegeben.
 */
void driverLCDInit(void)
{
    const unsigned char initCmds[9] = {
        LCD_RESET, LCD_BIAS, ADC_SEL_NORMAL, COMMON_REVERSE, RES_RATIO,
        ELEC_VOL_MODE, ELEC_VOL_VALUE, POWER_CONT, DISPLAY_ON};             // LCD Init just runs once!

    halSetLCDReset(true);       // Reset aktiv
    delayMsBlocked(25);         // 25 ms warten
    halSetLCDReset(false);      // Reset loesen

    driverLCDWriteCommand(initCmds, 9);
    driverLCDClear();
    while (LCD_Data.Status.B.TxSuc == 0);
}

/*
 * Cursor setzen: drei Kommandobytes.
 *
 * In einem Befehlsbyte stehen nur vier Bit fuer den Wert zur Verfuegung,
 * die oberen vier kennzeichnen den Befehl. Eine Spaltenadresse von 0 bis 127
 * braucht sieben Bit und wird deshalb auf zwei Befehle aufgeteilt:
 * obere vier Bit (col >> 4) und untere vier Bit (col & 0x0F).
 */
void driverLCDSetPosition(unsigned char page, unsigned char col)
{
    unsigned char pos[3];

    pos[0] = SET_PAGE     + page;           // Seite 0 .. LCD_MAX_ROW
    pos[1] = MSB_COL_ADDR + (col >> 4);     // obere vier Bit der Spalte
    pos[2] = LSB_COL_ADDR + (col & 0x0F);   // untere vier Bit der Spalte

    driverLCDWriteCommand(pos, 3);
}


/* ########## Ihre Arbeit ########## */

void driverLCDClear(void)
{
    // TODO 1: alle Seiten nacheinander anfahren und mit emptyPage
    //         ueberschreiben.
    //
    //         Achtung: LCD_MAX_ROW ist die Nummer der LETZTEN Seite,
    //         nicht die Anzahl der Seiten.
    //
    //         Je Seite: Position auf Spalte 0 setzen, dann LCD_MAX_COLM
    //         Bytes als Daten schicken. Der Spaltenzaehler des Displays
    //         ruckt selbsttaetig weiter, deshalb genuegt ein Aufruf
    //         von driverLCDWriteData() pro Seite.
}

void driverLCDWriteText(const char *text, unsigned char textLength, unsigned char page, unsigned char col)
{
    // TODO 2: Cursor auf (page, col) setzen, dann Zeichen fuer Zeichen die
    //         sechs Spalten aus font_table[] als Daten senden.
    //         Die Breite eines Zeichens steht als fonts_width_max im
    //         Fontheader. col nach jedem Zeichen mitfuehren.
    //
    //         Drei Faelle, die Sie abfangen muessen:
    //           - das Zeichen passt nicht mehr in die Zeile
    //                 -> naechste Seite, Spalte 0, Cursor neu setzen
    //           - die letzte Seite ist voll
    //                 -> abbrechen
    //           - das Zeichen steht nicht in der Tabelle (Index >= FONT_CHAR_COUNT)
    //                 -> Ersatzzeichen ausgeben, z. B. '?'
    //
    //         Vorsicht: char ist signed. Ein Zeichen ueber 127 ergibt sonst
    //         einen negativen Index in die Tabelle.
}

void driverLCDWriteUInt(unsigned int number, unsigned char page, unsigned char col)
{
    // TODO 3: Zahl in Ziffern zerlegen und als Text ausgeben.
    //
    //         number % 10 + '0' liefert die LETZTE Ziffer als ASCII,
    //         number /= 10 rueckt eine Stelle weiter. Die Ziffern fallen
    //         also von hinten an - fuellen Sie einen Puffer von hinten
    //         nach vorne und geben Sie ihn dann mit driverLCDWriteText()
    //         aus. Ein unsigned int hat hoechstens fuenf Stellen.
    //
    //         Denken Sie an den Fall number == 0: eine abweisende Schleife
    //         laeuft dort kein einziges Mal.
}
