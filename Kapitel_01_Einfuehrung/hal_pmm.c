////////////////////////////////////////////////////////////////////////////////
//
// Projektname: CC Controller FHJV5.0
// Autor: DI(FH) Markus Krenn, MSc
// Datum der letzten Änderung: 26.07.2016
// (c)2016 Markus Krenn
//
////////////////////////////////////////////////////////////////////////////////

#include <msp430.h>
#include "hal_pmm.h"

#define _HAL_PMM_DISABLE_SVML_
#define _HAL_PMM_DISABLE_SVSL_
#define _HAL_PMM_DISABLE_FULL_PERFORMANCE_


//****************************************************************************//
#ifdef _HAL_PMM_DISABLE_SVML_
#define _HAL_PMM_SVMLE SVMLE
#else
#define _HAL_PMM_SVMLE 0
#endif
#ifdef _HAL_PMM_DISABLE_SVSL_
#define _HAL_PMM_SVSLE SVSLE
#else
#define _HAL_PMM_SVSLE 0
#endif
#ifdef _HAL_PMM_DISABLE_FULL_PERFORMANCE_
#define _HAL_PMM_SVSFP SVSLFP
#else
#define _HAL_PMM_SVSFP 0
#endif


////////////////////////////////////////////////////////////////////////////////
// HAL_PMM_Init
// Initialize Power Management
void HAL_PMM_Init (){
  unsigned int CoreStatus = HAL_PMM_SetVCore(3);  
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// HAL_PMM_MakePOR
// Initialize Power Management
void HAL_PMM_MakePOR (void){
  PMMCTL0 |= PMMSWPOR;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// HAL_PMM_SetVCore
// Set VCore of CPU
unsigned int HAL_PMM_SetVCore (unsigned char level)
{
  unsigned int actlevel;
  unsigned int status = 0;
  level &= PMMCOREV_3;                       // Set Mask for Max. level
  actlevel = (PMMCTL0 & PMMCOREV_3);         // Get actual VCore

  while (((level != actlevel) && (status == 0)) || (level < actlevel))		// step by step increase or decrease
  {
    if (level > actlevel)
      status = HAL_PMM_SetVCoreUp(++actlevel);
    else
      status = HAL_PMM_SetVCoreDown(--actlevel);
  }
  return status;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// HAL_PMM_SetVCoreUp
// Set VCore Up
unsigned int HAL_PMM_SetVCoreUp (unsigned char level)
{
  unsigned int PMMRIE_backup,SVSMHCTL_backup;

  // Open PMM registers for write access
  PMMCTL0_H = 0xA5;

  // Disable dedicated Interrupts to prevent that needed flags will be cleared
  PMMRIE_backup = PMMRIE;
  PMMRIE &= ~(SVSMHDLYIE | SVSMLDLYIE | SVMLVLRIE | SVMHVLRIE | SVMHVLRPE);
  // Set SVM highside to new level and check if a VCore increase is possible
  SVSMHCTL_backup = SVSMHCTL;
  PMMIFG &= ~(SVMHIFG | SVSMHDLYIFG);
  SVSMHCTL = SVMHE | SVMHFP | (SVSMHRRL0 * level);
  // Wait until SVM highside is settled
  while ((PMMIFG & SVSMHDLYIFG) == 0);
  // Disable full-performance mode to save energy
  SVSMHCTL &= ~_HAL_PMM_SVSFP ;
  // Check if a VCore increase is possible
  if ((PMMIFG & SVMHIFG) == SVMHIFG){			//-> Vcc is to low for a Vcore increase
  	// recover the previous settings
  	PMMIFG &= ~SVSMHDLYIFG;
  	SVSMHCTL = SVSMHCTL_backup;
  	// Wait until SVM highside is settled
  	while ((PMMIFG & SVSMHDLYIFG) == 0);
  	// Clear all Flags
  	PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG);
  	// backup PMM-Interrupt-Register
  	PMMRIE = PMMRIE_backup;
  	
  	// Lock PMM registers for write access
  	PMMCTL0_H = 0x00;
  	return PMM_STATUS_ERROR;                       // return: voltage not set
  }
  // Set also SVS highside to new level			//-> Vcc is high enough for a Vcore increase
  SVSMHCTL |= SVSHE | (SVSHRVL0 * level);
  // Set SVM low side to new level
  SVSMLCTL = SVMLE | SVMLFP | (SVSMLRRL0 * level);
  // Wait until SVM low side is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG | SVMLIFG);
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait until new level reached
  if (PMMIFG & SVMLIFG)
  while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set also SVS/SVM low side to new level
  PMMIFG &= ~SVSMLDLYIFG;
  SVSMLCTL |= SVSLE | (SVSLRVL0 * level);
  // wait for lowside delay flags
  while ((PMMIFG & SVSMLDLYIFG) == 0);

// Disable SVS/SVM Low
// Disable full-performance mode to save energy
  SVSMLCTL &= ~(_HAL_PMM_DISABLE_SVSL_+_HAL_PMM_DISABLE_SVML_+_HAL_PMM_SVSFP );

  // Clear all Flags
  PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG);
  // backup PMM-Interrupt-Register
  PMMRIE = PMMRIE_backup;

  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
  return PMM_STATUS_OK;                               // return: OK
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// HAL_PMM_SetVCoreDown
// Set VCore Down
unsigned int HAL_PMM_SetVCoreDown (unsigned char level)
{
  unsigned int PMMRIE_backup;

  // Open PMM registers for write access
  PMMCTL0_H = 0xA5;

  // Disable dedicated Interrupts to prevent that needed flags will be cleared
  PMMRIE_backup = PMMRIE;
  PMMRIE &= ~(SVSMHDLYIE | SVSMLDLYIE | SVMLVLRIE | SVMHVLRIE | SVMHVLRPE);

  // Set SVM high side and SVM low side to new level
  PMMIFG &= ~(SVMHIFG | SVSMHDLYIFG | SVMLIFG | SVSMLDLYIFG);
  SVSMHCTL = SVMHE | SVMHFP | (SVSMHRRL0 * level);
  SVSMLCTL = SVMLE | SVMLFP | (SVSMLRRL0 * level);
  // Wait until SVM high side and SVM low side is settled
  while ((PMMIFG & SVSMHDLYIFG) == 0 || (PMMIFG & SVSMLDLYIFG) == 0);

  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;

  // Set also SVS highside and SVS low side to new level
  PMMIFG &= ~(SVSHIFG | SVSMHDLYIFG | SVSLIFG | SVSMLDLYIFG);
  SVSMHCTL |= SVSHE | SVSHFP | (SVSHRVL0 * level);
  SVSMLCTL |= SVSLE | SVSLFP | (SVSLRVL0 * level);
  // Wait until SVS high side and SVS low side is settled
  while ((PMMIFG & SVSMHDLYIFG) == 0 || (PMMIFG & SVSMLDLYIFG) == 0);
  // Disable full-performance mode to save energy
  SVSMHCTL &= ~_HAL_PMM_SVSFP;
// Disable SVS/SVM Low
// Disable full-performance mode to save energy
  SVSMLCTL &= ~(_HAL_PMM_DISABLE_SVSL_+_HAL_PMM_DISABLE_SVML_+_HAL_PMM_SVSFP );
	
  // Clear all Flags
  PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG);
  // backup PMM-Interrupt-Register
  PMMRIE = PMMRIE_backup;
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;

  if ((PMMIFG & SVMHIFG) == SVMHIFG)
    return PMM_STATUS_ERROR;					 	// Highside is still to low for the adjusted VCore Level
  else return PMM_STATUS_OK;						// Return: OK
}
////////////////////////////////////////////////////////////////////////////////

