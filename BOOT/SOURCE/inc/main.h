#ifndef __MAIN_H
#define __MAIN_H

//--------------------------------------------------------------------------------------------------------------------//
#include "stm32f4xx.h"
#include "sysInit.h"

//--------------------------------------------------------------------------------------------------------------------//
   
/* UserID and Password definition *********************************************/
#define USERID													"root"
#define PASSWORD												"8727"
#define LOGIN_SIZE   (15+ sizeof(USERID) + sizeof(PASSWORD))

#define MAX_DHCP_TRIES        					0x04

#define SN                              
#define HW															0x4078720D
#define SWPC														0x00000001
#define SWDT

#define MACA														0x028727  //028727xxxxxx
#define DHCP														0x00004F4E //on
#define IPAD														0xC0A85828
#define MASK														0xFFFFFF00
#define GWAD														0xC0A85801

#define NTPA														0xC0A8580B
#define NTPB														0xC0A8580C

//--------------------------------------------------------------------------------------------------------------------//

  
//--------------------------------------------------------------------------------------------------------------------//


#endif /* __MAIN_H */
