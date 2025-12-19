#include "uart.hpp"

extern "C" void USART2_IRQHandler(void)
{
	/*Check if the global pointer to the usart2 driver has been assigned to the usart2 instance*/
	if(emb::drivers::g_usart2_driver_ptr != nullptr)
	{
		/*Call RX isr handle*/
		emb::drivers::g_usart2_driver_ptr->isr_handle_rx();

		/*Call TX isr handle*/
		emb::drivers::g_usart2_driver_ptr->isr_handle_tx();
	}
	else
	{
		USART2->CR1 &=~ (USART_CR1_RXNEIE | USART_CR1_TXEIE);
	}
}
