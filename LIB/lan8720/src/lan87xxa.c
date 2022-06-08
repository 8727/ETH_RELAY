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
	/* ������������Ӧ���� */
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
//  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
//  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
//  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   
  /* �رշ��� */
  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
	/* �ر��ش����� */
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
	/* �ر��Զ�ȥ��PDA/CRC����  */
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
	/* �رս������е�֡ */
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
	/* ����������й㲥֡ */
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
	/* �رջ��ģʽ�ĵ�ַ����  */
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
	/* �����鲥��ַʹ��������ַ����    */
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
	/* �Ե�����ַʹ��������ַ����  */
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
	/* ����ipv4��TCP/UDP/ICMP��֡У���ж��   */
  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

  /*------------------------   DMA   -----------------------------------*/  

  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
	/*������ʹ��֡У���ж�ع��ܵ�ʱ��һ��Ҫʹ�ܴ洢ת��ģʽ,�洢ת��ģʽ��Ҫ��֤����֡�洢��FIFO��,
	����MAC�ܲ���/ʶ���֡У��ֵ,����У����ȷ��ʱ��DMA�Ϳ��Դ���֡,����Ͷ�������֡*/
	
	/* ��������TCP/IP����֡ */
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
	/* �����������ݵĴ洢ת��ģʽ  */
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
	/* �����������ݵĴ洢ת��ģʽ   */
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

	/* ��ֹת������֡ */
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
	/* ��ת����С�ĺ�֡ */
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
	/* �򿪴���ڶ�֡���� */
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
	/* ����DMA����ĵ�ַ���빦�� */
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
	/* �����̶�ͻ������ */
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
	/* DMA���͵����ͻ������Ϊ32������ */
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
	/*DMA���յ����ͻ������Ϊ32������ */
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

  /* Configure Ethernet */
	/* ����ETH */
  EthStatus = ETH_Init(&ETH_InitStructure, ETHERNET_PHY_ADDRESS);
	
	if(GET_PHY_LINK_STATUS()){
    EthStatus |= ETH_LINK_FLAG;
  }
	
}  
