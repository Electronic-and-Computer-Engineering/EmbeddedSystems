#ifndef __HAL_PMM
#define __HAL_PMM

#define PMM_STATUS_OK     0
#define PMM_STATUS_ERROR  1

/***********************************
 Function Prototypes ***************
***********************************/
void HAL_PMM_Init (void);
unsigned int HAL_PMM_SetVCore (unsigned char level);
unsigned int HAL_PMM_SetVCoreUp (unsigned char level);
unsigned int HAL_PMM_SetVCoreDown (unsigned char level);
void HAL_PMM_MakePOR (void);

#endif
