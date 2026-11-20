/*
 * driverLCD.c
 *
 * UNVOLLSTAENDIG - an jedem TODO fehlt Code.
 *
 * Vorgegeben: der Weg eines Bytes zum Display (lcdSend, WriteCommand,
 * WriteData), die Initialisierungssequenz, das Setzen der Position und
 * das Loeschen.
 * Ihre Arbeit: WriteText und WriteUInt.
 */

#include "driverLCD.h"
#include "driverFonttable.h"
#include "halGPIO.h"
#include "halUSCIB1.h"
#include "halUCS.h"

const unsigned char emptyPage[LCD_MAX_COLM] = {0};

/* ########## vorgegeben ########## */

/*
 * Der Weg jedes Bytes zum Display: warten, bis die vorige Uebertragung
 * durch ist, A0 passend setzen, Nutzdaten in den Sendepuffer des SPI-Moduls
 * kopieren und die Uebertragung anstossen. Das Chip Select setzt
 * halUSCIB1Transmit() aus Einheit 5, nicht diese Funktion.
 */
void driverLCDSend(const unsigned char *data, unsigned char dataLength, unsigned char isData)
{
    unsigned char i;

    while (halUSCIB1Busy());   // SPI Function, reading ==> LCD_Data.Status.B.TxSuc == 0 vorherige Uebertragung abwarten

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
    driverLCDSend(data, dataLength, 0);
}

void driverLCDWriteData(const unsigned char *data, unsigned char dataLength)
{
    driverLCDSend(data, dataLength, 1);
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
    while (halUSCIB1Busy());
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


/*
 * Display loeschen: jede Seite einmal anfahren und mit Nullbytes fuellen.
 *
 * Der Spaltenzaehler des Displays rueckt nach jedem Byte selbsttaetig
 * weiter - deshalb genuegt ein einziger Aufruf von driverLCDWriteData()
 * ueber eine ganze Seite. LCD_MAX_ROW ist die Nummer der LETZTEN Seite,
 * daher <= und nicht <.
 */
void driverLCDClear(void)
{
    unsigned char page;

    for (page = 0; page <= LCD_MAX_ROW; page++)
    {
        driverLCDSetPosition(page, 0);
        driverLCDWriteData(emptyPage, LCD_MAX_COLM);
    }
}


/* ########## Ihre Arbeit ########## */

void driverLCDWriteText(const char *text, unsigned char textLength, unsigned char page, unsigned char col)
{
    unsigned char n;

    driverLCDSetPosition(page, col);

    for (n = 0; n < textLength; n++)
    {
        driverLCDWriteData(font_table[(unsigned char)text[n]], fonts_width_max);
        col += fonts_width_max;

        // TODO: Zeilenende? -> page weiterzaehlen, col auf 0, Cursor neu setzen
        //       letzte Page voll -> abbrechen
    }
}

void driverLCDWriteUInt(unsigned int number, unsigned char page, unsigned char col)
{
    // TODO 2: Zahl in Ziffern zerlegen und als Text ausgeben.
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

