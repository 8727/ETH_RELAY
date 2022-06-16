#include "sysInit.h"

static __IO uint32_t msTicks;
uint32_t upTime;

static const uint32_t pageAddress[PAGES_NUM] = {PAGE_0_ADDRESS, PAGE_1_ADDRESS};
static const uint32_t varIdList[VAR_NUM] = {P_MACA, P_DHCP, P_IPAD, P_MASK, P_GWAD, P_NTPA, P_NTPB};
struct ethSettingsInitTypeDef ethSettings;
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
void FlashUnlock(void){
  if((FLASH->CR & FLASH_CR_LOCK) != RESET){
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
  }
}

//--------------------------------------------------------------------------------------------------------------------//
void FlashLock(void){
  FLASH->CR |= FLASH_CR_LOCK;
}

//--------------------------------------------------------------------------------------------------------------------//
void FlashObUnlock(void){
  if((FLASH->OPTCR & FLASH_OPTCR_OPTLOCK) != RESET){
    FLASH->OPTKEYR = FLASH_OPT_KEY1;
    FLASH->OPTKEYR = FLASH_OPT_KEY2;
  }  
}

//--------------------------------------------------------------------------------------------------------------------//
void FlashObLock(void){
  FLASH->OPTCR |= FLASH_OPTCR_OPTLOCK;
}

//--------------------------------------------------------------------------------------------------------------------//
FLASH_Status FlashGetStatus(void){
  FLASH_Status flashstatus = FLASH_COMPLETE;
  
  if((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY) {
    flashstatus = FLASH_BUSY;
  }else{  
    if((FLASH->SR & FLASH_SR_WRPERR) != (uint32_t)0x00){ 
      flashstatus = FLASH_ERROR_WRP;
    }else{
      if((FLASH->SR & (uint32_t)0xEF) != (uint32_t)0x00){
        flashstatus = FLASH_ERROR_PROGRAM; 
      }else{
        if((FLASH->SR & FLASH_SR_SOP) != (uint32_t)0x00){
          flashstatus = FLASH_ERROR_OPERATION;
        }else{
          flashstatus = FLASH_COMPLETE;
        }
      }
    }
  }
  return flashstatus;
}

//--------------------------------------------------------------------------------------------------------------------//
FLASH_Status FlashWaitForLastOperation(void){ 
  __IO FLASH_Status status = FLASH_COMPLETE;

  status = FlashGetStatus();

  while(status == FLASH_BUSY){
    status = FlashGetStatus();
  }
  return status;
}

//--------------------------------------------------------------------------------------------------------------------//
FLASH_Status FlashEraseSector(uint32_t FLASH_Sector){
  uint32_t tmp_psize = FLASH_CR_PSIZE_1;
  FLASH_Status status = FLASH_COMPLETE;
  
  status = FlashWaitForLastOperation();
  
  if(status == FLASH_COMPLETE){ 
    /* if the previous operation is completed, proceed to erase the sector */
    FLASH->CR &= 0xFFFFFCFF;
    FLASH->CR |= tmp_psize;
    FLASH->CR &= 0xFFFFFF07;
    FLASH->CR |= FLASH_CR_SER | FLASH_Sector;
    FLASH->CR |= FLASH_CR_STRT;
    
    /* Wait for last operation to be completed */
    status = FlashWaitForLastOperation();
    
    /* if the erase operation is completed, disable the SER Bit */
    FLASH->CR &= (~FLASH_CR_SER);
    FLASH->CR &= 0xFFFFFF07; 
  }
  /* Return the Erase Status */
  return status;
}

//--------------------------------------------------------------------------------------------------------------------//
//FLASH_Status FlashProgramDoubleWord(uint32_t Address, uint64_t Data){
//  FLASH_Status status = FLASH_COMPLETE;
//  
//  if(status == FLASH_COMPLETE){
//    /* if the previous operation is completed, proceed to program the new data */
//    FLASH->CR &= 0xFFFFFCFF;
//    FLASH->CR |= FLASH_CR_PSIZE;
//    FLASH->CR |= FLASH_CR_PG;
//  
//    *(__IO uint64_t*)Address = Data;
//        
//    /* Wait for last operation to be completed */
//    status = FlashWaitForLastOperation();

//    /* if the program operation is completed, disable the PG Bit */
//    FLASH->CR &= (~FLASH_CR_PG);
//  } 
//  return status;
//}

//--------------------------------------------------------------------------------------------------------------------//
FLASH_Status FlashProgramWord(uint32_t Address, uint32_t Data){
  FLASH_Status status = FLASH_COMPLETE;
	
	FLASH->SR |= FLASH_SR_WRPERR;
	FLASH->SR |= FLASH_SR_PGPERR;
	FLASH->SR |= FLASH_SR_PGSERR;
  /* Wait for last operation to be completed */
  status = FlashWaitForLastOperation();
  
  if(status == FLASH_COMPLETE){
    /* if the previous operation is completed, proceed to program the new data */
    FLASH->CR &= 0xFFFFFCFF;
    FLASH->CR |= FLASH_CR_PSIZE_1;
    FLASH->CR |= FLASH_CR_PG;
  
    *(__IO uint32_t*)Address = Data;
        
    /* Wait for last operation to be completed */
    status = FlashWaitForLastOperation();

    /* if the program operation is completed, disable the PG Bit */
    FLASH->CR &= (~FLASH_CR_PG);
  } 
  /* Return the Program Status */
  return status;
}

//--------------------------------------------------------------------------------------------------------------------//
uint32_t FlashRead(uint32_t address){
  return (*(__IO uint32_t*)address);
}

//--------------------------------------------------------------------------------------------------------------------//
void EepromWriteData(uint32_t address, uint32_t varId, uint32_t varValue){
  FlashUnlock();
  FlashProgramWord(address, varId);
	FlashLock();
	address += 0x04;
	FlashUnlock();
	FlashProgramWord((address), varValue);
  FlashLock();
}

//--------------------------------------------------------------------------------------------------------------------//
uint8_t EepromGetActivePageIdx(void){
  uint8_t pageStates[PAGES_NUM];
	uint8_t id;
  for(uint8_t i = 0; i < PAGES_NUM; i++){
    pageStates[i] = FlashRead(pageAddress[i]);
  }
  if((pageStates[0] == VALID_PAGE) && (pageStates[1] != VALID_PAGE)){
    id = 0;
  }else{
    if((pageStates[1] == VALID_PAGE) && (pageStates[0] != VALID_PAGE)){
      id = 1;
    }else{

    }
  }
	return id;
}

//--------------------------------------------------------------------------------------------------------------------//
uint32_t EepromRead(uint32_t varId){
	uint8_t activePage = EepromGetActivePageIdx(); 
  uint32_t startAddr = pageAddress[activePage] + PAGE_DATA_OFFSET;
  uint32_t endAddr = pageAddress[activePage] + PAGE_SIZE - PAGE_DATA_SIZE;
  uint32_t addr = endAddr;
	uint32_t data;
  
  while (addr >= startAddr){
    uint32_t idData = FlashRead(addr);
    if (idData == varId){
      data = FlashRead(addr + 4);
      break;
    }else{
      addr -= PAGE_DATA_SIZE;
    }
  }
	return data;
}

//--------------------------------------------------------------------------------------------------------------------//
void EepromSetPageState(uint8_t id, uint32_t state){  
  FlashUnlock();
  FlashProgramWord(pageAddress[id], state);                   
  FlashLock();
}

//--------------------------------------------------------------------------------------------------------------------//
void EepromClearPage(uint8_t id){
	switch (id){
    case 0:  FlashEraseSector(FLASH_Sector_2);
    break;
    case 1:  FlashEraseSector(FLASH_Sector_3);
    break;
    default:
    break;
  }
}

//--------------------------------------------------------------------------------------------------------------------//
void EepromPageTransfer(uint8_t activePage, uint32_t varId, uint32_t varValue){
  uint8_t oldPage, newPage;
  
  if(activePage == 0){
    oldPage = 0;
    newPage = 1;
  }else{
    if(activePage == 1){
      oldPage = 1;
      newPage = 0;
    }
  }
  EepromSetPageState(newPage, RECEIVE_DATA);
  uint32_t curAddr = pageAddress[newPage] + PAGE_DATA_OFFSET;
  EepromWriteData(curAddr, varId, varValue);
  
  curAddr += PAGE_DATA_SIZE;
  
  for(uint32_t curVar = 0; curVar < VAR_NUM; curVar++){
    if(varIdList[curVar] != varId){
      uint32_t curVarValue = EepromRead(varIdList[curVar]);
      EepromWriteData(curAddr, varIdList[curVar], curVarValue);
      curAddr += PAGE_DATA_SIZE;
    }
  }
  EepromClearPage(oldPage);
  EepromSetPageState(newPage, VALID_PAGE);
}

//--------------------------------------------------------------------------------------------------------------------//
static void EepromWrite(uint32_t varId, uint32_t varValue){  
  uint8_t activePage = EepromGetActivePageIdx();  
  uint32_t startAddr = pageAddress[activePage] + PAGE_DATA_OFFSET;
  uint32_t endAddr = pageAddress[activePage] + PAGE_SIZE - PAGE_DATA_SIZE;
  uint32_t addr = startAddr;
  
  uint8_t freeSpaceFound = 0;
  
  while (addr <= endAddr){
    uint32_t idData = FlashRead(addr);
    if(idData == 0xFFFFFFFF){
      freeSpaceFound = 1;
      break;
    }else{
      addr += PAGE_DATA_SIZE;
    }
  }
  
  if(freeSpaceFound == 1){
		EepromWriteData(addr, varId, varValue);
  }else{
    EepromPageTransfer(activePage, varId, varValue);
  }
}

//--------------------------------------------------------------------------------------------------------------------//
void EepromCopyPageData(uint8_t oldPage, uint8_t newPage){
  uint32_t curAddr = pageAddress[newPage] + PAGE_DATA_OFFSET;
    
  for(uint32_t curVar = 0; curVar < VAR_NUM; curVar++){
    uint32_t curVarValue = FlashRead(varIdList[curVar]);
    EepromWriteData(curAddr, varIdList[curVar], curVarValue);
    curAddr += PAGE_DATA_SIZE;
  }
  EepromSetPageState(newPage, VALID_PAGE);
  EepromClearPage(oldPage);
}

//--------------------------------------------------------------------------------------------------------------------//
void EepromFormat(void){  
  for (uint8_t i = 0; i < PAGES_NUM; i++){
    EepromClearPage(i);
	}
}

//--------------------------------------------------------------------------------------------------------------------//
void EepromInit(void){
  uint32_t pageStates[PAGES_NUM];
  
  for(uint16_t i = 0x00; i < PAGES_NUM; i++){
    pageStates[i] = FlashRead(pageAddress[i]);
  }
  if(((pageStates[0] == ERASED) && (pageStates[1] == ERASED)) ||
      ((pageStates[0] == VALID_PAGE) && (pageStates[1] == VALID_PAGE)) || 
      ((pageStates[0] == RECEIVE_DATA) && (pageStates[1] == RECEIVE_DATA))){
	EepromFormat();
	EepromSetPageState(0, VALID_PAGE);
	
	uint32_t *UniqueID = (uint32_t *)0x1FFF7A10;
	EepromWrite(P_MACA, UniqueID[0] &= 0x00FFFFFF);
	EepromWrite(P_DHCP, DHCP);
	EepromWrite(P_IPAD, IPAD);
	EepromWrite(P_MASK, MASK);
	EepromWrite(P_GWAD, GWAD);
	EepromWrite(P_NTPA, NTPA);
	EepromWrite(P_NTPB, NTPB);					
  }
  if((pageStates[0] == RECEIVE_DATA) && (pageStates[1] == ERASED)){ EepromSetPageState(0, VALID_PAGE); }
  if((pageStates[0] == ERASED) && (pageStates[1] == RECEIVE_DATA)){ EepromSetPageState(1, VALID_PAGE); }
  if((pageStates[0] == RECEIVE_DATA) && (pageStates[1] == VALID_PAGE)){ EepromCopyPageData(1, 0); }
  if((pageStates[0] == VALID_PAGE) && (pageStates[1] == RECEIVE_DATA)){ EepromCopyPageData(0, 1); }
	
	ethSettings.ethMAC = EepromRead(P_MACA);
	ethSettings.ethDHCP = EepromRead(P_DHCP);
	ethSettings.ethIP = EepromRead(P_IPAD);
	ethSettings.ethMASK = EepromRead(P_MASK);
	ethSettings.ethGW = EepromRead(P_GWAD);
	ethSettings.ethNTPA = EepromRead(P_NTPA);
	ethSettings.ethNTPB = EepromRead(P_NTPB);	
}

//--------------------------------------------------------------------------------------------------------------------//
void Sysinit(void){
	if(SysTick_Config(SystemCoreClock / 1000)){  //1ms
		while (1);		/* Capture error */ 
	}
	NVIC_SetPriority (SysTick_IRQn,0);
	EepromInit();
	

	EthInit();
	
}

//--------------------------------------------------------------------------------------------------------------------//
