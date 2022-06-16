#ifndef SYSINIT_H
#define SYSINIT_H

//--------------------------------------------------------------------------------------------------------------------//
#include "stm32f4xx.h"
#include "main.h"
#include "lan87xxa.h"

//--------------------------------------------------------------------------------------------------------------------//
#define true                            0x01
#define false                           0x00

#define USER_FLASH_FIRST_PAGE_ADDRESS		0x08010000
#define USER_FLASH_LAST_PAGE_ADDRESS	  0x08080000
#define USER_FLASH_END_ADDRESS	        0x0807FFFF

#define PAGE_DATA_OFFSET               	8
#define PAGE_DATA_SIZE             			8


#define P_MACA													0x4D414341
#define P_DHCP													0x44484350
#define P_IPAD													0x49504144
#define P_MASK													0x4D41534B
#define P_GWAD													0x47544144
#define P_NTPA													0x4E545041
#define P_NTPB													0x4E545042
#define VAR_NUM                         7

#define PAGE_0_ADDRESS                  0x08008000
#define PAGE_1_ADDRESS                  0x0800C000
#define PAGE_SIZE                       16384
/* Page status definitions */
#define ERASED                					((uint32_t)0xFFFFFFFF)     /* Page is empty */
#define RECEIVE_DATA          					((uint32_t)0x55555555)     /* Page is marked to receive data */
#define VALID_PAGE            					((uint32_t)0x00000000)     /* Page containing valid data */
#define PAGES_NUM												2

#define RDP_KEY				                  ((uint16_t)0x00A5)
#define FLASH_KEY1											((uint32_t)0x45670123)
#define FLASH_KEY2											((uint32_t)0xCDEF89AB)
#define FLASH_OPT_KEY1									((uint32_t)0x08192A3B)
#define FLASH_OPT_KEY2									((uint32_t)0x4C5D6E7F)
  
#if (INFO == true || WEB_BOOT_DEBUG == true)
  #define ACTIVE_SWO
#endif

#if defined ACTIVE_SWO
  #include "stdio.h"
  #define ITM_Port8(n)                  (*((volatile unsigned char *)(0xE0000000+4*n)))
  #define ITM_Port32(n)                 (*((volatile unsigned long *)(0xE0000000+4*n)))
  #define DEMCR                         (*((volatile unsigned long *)(0xE000EDFC)))
  #define TRCENA                        0x01000000
#endif

//--------------------------------------------------------------------------------------------------------------------//
#define BUILD_YEAR (__DATE__[7] == '?' ? 1900 : (((__DATE__[7] - '0') * 1000 ) + (__DATE__[8] - '0') * 100 + (__DATE__[9] - '0') * 10 + __DATE__[10] - '0'))
#define BUILD_MONTH (__DATE__ [2] == '?' ? 1 : __DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) : __DATE__ [2] == 'b' ? 2 : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
        : __DATE__ [2] == 'y' ? 5 : __DATE__ [2] == 'l' ? 7 : __DATE__ [2] == 'g' ? 8 : __DATE__ [2] == 'p' ? 9 : __DATE__ [2] == 't' ? 10 : __DATE__ [2] == 'v' ? 11 : 12)
#define BUILD_DAY (__DATE__[4] == '?' ? 1 : ((__DATE__[4] == ' ' ? 0 : ((__DATE__[4] - '0') * 10)) + __DATE__[5] - '0'))
#define BUILD_TIME_H (__TIME__[0] == '?' ? 1 : ((__TIME__[0] == ' ' ? 0 : ((__TIME__[0] - '0') * 10)) + __TIME__[1] - '0'))
#define BUILD_TIME_M (__TIME__[3] == '?' ? 1 : ((__TIME__[3] - '0') * 10 + __TIME__[4] - '0'))
#define BUILD_TIME_S (__TIME__[6] == '?' ? 1 : ((__TIME__[6] - '0') * 10 + __TIME__[7] - '0'))

//--------------------------------------------------------------------------------------------------------------------//
#define FLASH_Sector_0     							((uint16_t)0x0000) /*!< Sector Number 0 */
#define FLASH_Sector_1     							((uint16_t)0x0008) /*!< Sector Number 1 */
#define FLASH_Sector_2     							((uint16_t)0x0010) /*!< Sector Number 2 */
#define FLASH_Sector_3     							((uint16_t)0x0018) /*!< Sector Number 3 */
#define FLASH_Sector_4     							((uint16_t)0x0020) /*!< Sector Number 4 */
#define FLASH_Sector_5     							((uint16_t)0x0028) /*!< Sector Number 5 */
#define FLASH_Sector_6     							((uint16_t)0x0030) /*!< Sector Number 6 */
#define FLASH_Sector_7     							((uint16_t)0x0038) /*!< Sector Number 7 */
#define FLASH_Sector_8     							((uint16_t)0x0040) /*!< Sector Number 8 */
#define FLASH_Sector_9     							((uint16_t)0x0048) /*!< Sector Number 9 */
#define FLASH_Sector_10    							((uint16_t)0x0050) /*!< Sector Number 10 */
#define FLASH_Sector_11    							((uint16_t)0x0058) /*!< Sector Number 11 */

typedef enum
{ 
  FLASH_BUSY = 1,
  FLASH_ERROR_PGS,
  FLASH_ERROR_PGP,
  FLASH_ERROR_PGA,
  FLASH_ERROR_WRP,
  FLASH_ERROR_PROGRAM,
  FLASH_ERROR_OPERATION,
  FLASH_COMPLETE
}FLASH_Status;

//--------------------------------------------------------------------------------------------------------------------//
struct ethSettingsInitTypeDef{
  uint32_t ethMAC;
  uint32_t ethDHCP;
  uint32_t ethIP;
	uint32_t ethMASK;
	uint32_t ethGW;
	uint32_t ethNTPA;
	uint32_t ethNTPB;
};

extern struct ethSettingsInitTypeDef ethSettings;
extern uint32_t upTime;

//--------------------------------------------------------------------------------------------------------------------//
void DelayMs(uint32_t ms);
void FlashEepromInit(void);
void Sysinit(void);

#endif /* SYSINIT_H */
