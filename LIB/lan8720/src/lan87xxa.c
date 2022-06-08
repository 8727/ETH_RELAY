#include "lan87xxa.h"

ETH_InitTypeDef ETH_InitStructure;
__IO uint32_t  EthStatus = 0;
__IO uint8_t EthLinkStatus = 0;

void EthInit(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	GPIOA->MODER |= GPIO_MODER_MODER8_1;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR8;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8;
  RCC->CFGR |= RCC_CFGR_MCO1_1;	
	
	GPIOA->MODER |= GPIO_MODER_MODER1_1 | GPIO_MODER_MODER2_1 | GPIO_MODER_MODER7_1;
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_0 | GPIO_PUPDR_PUPDR2_0 | GPIO_PUPDR_PUPDR7_0;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR1 | GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR7;
	GPIOA->AFR[0x01 >> 0x03] |= ((uint32_t)0x000000B0);
	GPIOA->AFR[0x02 >> 0x03] |= ((uint32_t)0x00000B00);
	GPIOA->AFR[0x07 >> 0x03] |= ((uint32_t)0xB0000000);
	
	GPIOB->MODER |= GPIO_MODER_MODER11_1 | GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR11_0 | GPIO_PUPDR_PUPDR12_0 | GPIO_PUPDR_PUPDR13_0;
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR11 | GPIO_OSPEEDER_OSPEEDR12 | GPIO_OSPEEDER_OSPEEDR13;
	GPIOB->AFR[0x0B >> 0x03] |= ((uint32_t)0x0000B000);
	GPIOB->AFR[0x0C >> 0x03] |= ((uint32_t)0x000B0000);
	GPIOB->AFR[0x0D >> 0x03] |= ((uint32_t)0x00B00000);
	
	GPIOC->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_1 | GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1;
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR1_0 | GPIO_PUPDR_PUPDR4_0 | GPIO_PUPDR_PUPDR5_0;
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR1 | GPIO_OSPEEDER_OSPEEDR4 | GPIO_OSPEEDER_OSPEEDR5;
	GPIOC->AFR[0x01 >> 0x03] |= ((uint32_t)0x000000B0);
	GPIOC->AFR[0x04 >> 0x03] |= ((uint32_t)0x000B0000);
	GPIOC->AFR[0x05 >> 0x03] |= ((uint32_t)0x00B00000);
	
	LAN87XX_RST_HIGHT;
	DelayMs(10);
	LAN87XX_RST_LOW;
	DelayMs(10);
	LAN87XX_RST_HIGHT;
	
	RCC->AHB1ENR |= RCC_AHB1ENR_ETHMACEN | RCC_AHB1ENR_ETHMACTXEN | RCC_AHB1ENR_ETHMACRXEN;
	
	RCC->AHB1RSTR |= RCC_AHB1RSTR_ETHMACRST;
	RCC->AHB1RSTR &= ~RCC_AHB1RSTR_ETHMACRST;
	
	ETH->DMABMR |= ETH_DMABMR_SR;
	while(ETH->DMABMR & ETH_DMABMR_SR);
	
	ETH_StructInit(&ETH_InitStructure);
	
	  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
	/* 开启网络自适应功能 */
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
//  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
//  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
//  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   
  /* 关闭反馈 */
  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
	/* 关闭重传功能 */
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
	/* 关闭自动去除PDA/CRC功能  */
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
	/* 关闭接收所有的帧 */
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
	/* 允许接收所有广播帧 */
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
	/* 关闭混合模式的地址过滤  */
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
	/* 对于组播地址使用完美地址过滤    */
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
	/* 对单播地址使用完美地址过滤  */
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
	/* 开启ipv4和TCP/UDP/ICMP的帧校验和卸载   */
  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

  /*------------------------   DMA   -----------------------------------*/  

  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
	/*当我们使用帧校验和卸载功能的时候，一定要使能存储转发模式,存储转发模式中要保证整个帧存储在FIFO中,
	这样MAC能插入/识别出帧校验值,当真校验正确的时候DMA就可以处理帧,否则就丢弃掉该帧*/
	
	/* 开启丢弃TCP/IP错误帧 */
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
	/* 开启接收数据的存储转发模式  */
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
	/* 开启发送数据的存储转发模式   */
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

	/* 禁止转发错误帧 */
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
	/* 不转发过小的好帧 */
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
	/* 打开处理第二帧功能 */
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
	/* 开启DMA传输的地址对齐功能 */
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
	/* 开启固定突发功能 */
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
	/* DMA发送的最大突发长度为32个节拍 */
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
	/*DMA接收的最大突发长度为32个节拍 */
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

  /* Configure Ethernet */
	/* 配置ETH */
  EthStatus = ETH_Init(&ETH_InitStructure, ETHERNET_PHY_ADDRESS);
	
	if(GET_PHY_LINK_STATUS()){
    EthStatus |= ETH_LINK_FLAG;
  }
	
}  
