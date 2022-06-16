#ifndef LAN87XX_H_H
#define LAN87XX_H_H

//--------------------------------------------------------------------------------------------------------------------//
#include "stm32f4xx.h"
#include "sysInit.h"
#include "stm32f4xx_eth.h"
//#include "netif.h"
//#include "lwip/opt.h"
//#include "netconf.h"
//#include "lwip/dhcp.h"


#define	LAN87XX_RST_LOW							GPIOC->BSRRH = GPIO_BSRR_BS_0
#define LAN87XX_RST_HIGHT						GPIOC->BSRRL = GPIO_BSRR_BS_0

/* Ethernet Flags for EthStatus variable */   
#define ETH_INIT_FLAG           0x01 /* Ethernet Init Flag */
#define ETH_LINK_FLAG           0x10 /* Ethernet Link Flag */

#define ETHERNET_PHY_ADDRESS       0x00 /* Relative to STM324xG-EVAL Board */
#define GET_PHY_LINK_STATUS()		(ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, PHY_BSR) & 0x00000004)

/*
	ETH_RST --------------------------> PC0
	ETH_MDIO -------------------------> PA2
	ETH_MDC --------------------------> PC1
	ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1
	ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7
	ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4
	ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5
	ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PB11
	ETH_MII_TXD0/ETH_RMII_TXD0 -------> PB12
	ETH_MII_TXD1/ETH_RMII_TXD1 -------> PB13
*/
																	
void EthInit(void);

#endif /* LAN87XX_H_H */
