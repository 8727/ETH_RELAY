#include "sysInit.h"

static __IO uint32_t msTicks;
uint32_t upTime;

//--------------------------------------------------------------------------------------------------------------------//
#if defined ACTIVE_SWO
  struct __FILE { int handle; };
  FILE __stdout;
  int fputc(int ch, FILE *f){ if(DEMCR & TRCENA){ while(ITM_Port32(0x00) == 0x00);
    ITM_Port8(0x00) = ch; }
    return(ch);
  }
#endif

//--------------------------------------------------------------------------------------------------------------------//
void SysTick_Handler(void){ msTicks++; }

//--------------------------------------------------------------------------------------------------------------------//
void DelayMs(uint32_t ms){ uint32_t tickStart = msTicks;
  while((msTicks - tickStart) < ms){}
}

//--------------------------------------------------------------------------------------------------------------------//
void Sysinit(void){
	if (SysTick_Config(SystemCoreClock / 1000)){  //1ms
		while (1);		/* Capture error */ 
	}
	NVIC_SetPriority (SysTick_IRQn,0);
	EthInit();

  
}

//--------------------------------------------------------------------------------------------------------------------//
