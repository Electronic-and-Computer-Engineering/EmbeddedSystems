[⬅ Zurück zur Kapitelübersicht](README.md#kapitelübersicht--aufgabenstellungen)

# MSP430F5335 Interrupt Vectors

## Ports
- `PORT4_VECTOR` /* 0xFFCA Port 4 */
- `PORT3_VECTOR` /* 0xFFCC Port 3 */
- `PORT2_VECTOR` /* 0xFFD8 Port 2 */
- `PORT1_VECTOR` /* 0xFFDE Port 1 */

## Timers
- `TIMER2_A1_VECTOR` /* 0xFFCE Timer2_A5 CC1-4, TA */
- `TIMER2_A0_VECTOR` /* 0xFFD0 Timer2_A5 CC0 */
- `TIMER1_A1_VECTOR` /* 0xFFE0 Timer1_A3 CC1-2, TA1 */
- `TIMER1_A0_VECTOR` /* 0xFFE2 Timer1_A3 CC0 */
- `TIMER0_A1_VECTOR` /* 0xFFE8 Timer0_A5 CC1-4, TA */
- `TIMER0_A0_VECTOR` /* 0xFFEA Timer0_A5 CC0 */
- `TIMER0_B1_VECTOR` /* 0xFFF4 Timer0_B7 CC1-6, TB */
- `TIMER0_B0_VECTOR` /* 0xFFF6 Timer0_B7 CC0 */

## Modules
- `USCI_B1_VECTOR` /* 0xFFDA USCI B1 Receive/Transmit */
- `USCI_A1_VECTOR` /* 0xFFDC USCI A1 Receive/Transmit */
- `USCI_B0_VECTOR` /* 0xFFEE USCI B0 Receive/Transmit */
- `USCI_A0_VECTOR` /* 0xFFF0 USCI A0 Receive/Transmit */
- `ADC12_VECTOR` /* 0xFFEC ADC */
- `COMP_B_VECTOR` /* 0xFFF8 Comparator B */
- `WDT_VECTOR` /* 0xFFF2 Watchdog Timer */
- `LDO_PWR_VECTOR` /* 0xFFE6 LDO Power Management event */
- `DMA_VECTOR` /* 0xFFE4 DMA */
- `RTC_VECTOR` /* 0xFFD4 RTC */

## System and Reset
- `UNMI_VECTOR` /* 0xFFFA User Non-maskable */
- `SYSNMI_VECTOR` /* 0xFFFC System Non-maskable */
- `RESET_VECTOR` /* 0xFFFE Reset [Highest Priority] */