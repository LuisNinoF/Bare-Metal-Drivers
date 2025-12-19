#include "uart.hpp"

#define GPIOAEN		(1U<<0)
#define UART2EN		(1U<<17)

namespace emb{
	namespace drivers{

		/*global pointer to the usart2 driver*/
		UartDriver* g_usart2_driver_ptr = nullptr;

		UartDriver::UartDriver(USART_TypeDef* usart_instance):m_usart(usart_instance), m_init(false)
		{
			/*assign this instance to the global pointer if it USART2*/
			if(m_usart == USART2)
			{
				g_usart2_driver_ptr = this;
			}
		}

		void UartDriver::configure_gpio()
		{
			/*Enable clock access to GPIOA*/
			RCC->AHB1ENR |= GPIOAEN;

			/*Set the mode of PA2 to alternate function mode (10)*/
			GPIOA->MODER &= ~(3U << (2 * 2));
			GPIOA->MODER |=  (2U << (2 * 2));

			/*Set the mode of PA3 to alternate function mode (10)*/
			GPIOA->MODER &= ~(3U << (3 * 2));
			GPIOA->MODER |=  (2U << (3 * 2));

			/*Set alternate function type to AF7(UART2_TX) (to 0111)*/
			GPIOA->AFR[0] &= ~(0xFU << (2 * 4));
			GPIOA->AFR[0] |=  (0x7U << (2 * 4));

			/*Set alternate function type to AF7(UART2_RX) (to 0111)*/
			GPIOA->AFR[0] &= ~(0xFU << (3 * 4));
			GPIOA->AFR[0] |=  (0x7U << (3 * 4));

			/*Configure PA3 pull-up*/
			GPIOA->PUPDR &= ~(3U << (3 * 2));
			GPIOA->PUPDR |=  (1U << (3 * 2));
		}

		void UartDriver::set_baud_rate(uint32_t baud_rate)
		{
			m_usart->BRR = (APB1_CLOCK_HZ + (baud_rate/2))/baud_rate;
		}

		bool UartDriver::init(uint32_t baud_rate, uint8_t data_bits, Parity parity, StopBits stop_bits)
		{
			if(m_usart != USART2)
			{
				return false;
			}

			/*Enable clock access to UART2*/
		    RCC->APB1ENR |=	UART2EN;

		    /*Configure GPIO pins*/
		    configure_gpio();

		 	/*Disable UART peripheral*/
		    m_usart->CR1 &= ~USART_CR1_UE;

		    /*Configure CR1*/
		    uint32_t cr1_temp = 0;
		    if(data_bits == 9)
		    {
		    	cr1_temp |= USART_CR1_M; 		//word length set to 9 bits (M=1). Default is 8 bits (M=0)
		    }
		    if(parity != Parity::NONE)
		    {
		    	cr1_temp |= USART_CR1_PCE; 		//parity control enable
		    	if(parity == Parity::ODD)
		    	{
		    		cr1_temp |= USART_CR1_PS; 	//odd parity (PS=1). Default is even parity (PS=0)
		    	}
		    }
		    m_usart->CR1 = cr1_temp;

		    /*Configure CR2*/
		    uint32_t cr2_temp = m_usart->CR2;
		    cr2_temp &= ~USART_CR2_STOP;
		    if(stop_bits == StopBits::TWO)
		    {
		    	cr2_temp |= USART_CR2_STOP_1;	//2 stop bits. Default is 1 stop bit.
		    }
		    m_usart->CR2 = cr2_temp;

		    /*Configure CR3*/
		    m_usart->CR3 = 0;

		    /*Set the baud rate*/
		    set_baud_rate(baud_rate);

		    /*Enable TX and RX*/
		    m_usart->CR1 |= USART_CR1_TE | USART_CR1_RE;

		    /*Enable UART interrupts*/
		    enable_uart_interrupts();

		    /*Enable UART peripheral*/
		    m_usart->CR1 |= USART_CR1_UE;

		    /*Mark initialization flag*/
		    m_init = true;

		    /*Return initialization success flag*/
		    return true;
		}

		void UartDriver::enable_uart_interrupts()
		{
			/*Enable RXNE interrupt*/
			m_usart->CR1 |= USART_CR1_RXNEIE;

			/*TXE interrupt will only be enable on demand, when there is data to be transmitted*/

			/*Enable interrupt in NVIC*/
			NVIC_EnableIRQ(USART2_IRQn);
		}

		bool UartDriver::send_byte(uint8_t byte)
		{
			/*Check if UART is initialized*/
			if(!m_init) return true;

			/*Enqueue byte to TX buffer, enable TXEIE to trigger TX interrupt, and return true if successful*/
			if(m_tx_buffer.enqueue(byte))
			{
				m_usart->CR1 |= USART_CR1_TXEIE;
				return true;
			}

			/*If enqueue not successful (e.g. if TX buffer is full), return false*/
			return false;
		}

		size_t UartDriver::send_string(const char* str)
		{
			/*Check if UART is initialized and if str is not NULL*/
			if(!m_init || !str) return 0;

			/*local variable to count the bytes successfully queued*/
			size_t bytes_queued = 0;

			/*loop through the string to send multiple bytes, until the null character is found*/
			while(*str != '\0')
			{
				/*send each byte at a time. If done successfully, increment bytes_queued counter and advance in the string*/
				if(send_byte(static_cast<uint8_t>(*str)))
				{
					bytes_queued ++;
					str ++;
				}
				/*If not successful (e.g. if TX queue is full), break the queuing loop*/
				else
				{
					break;
				}
			}

			/*once the string has been completely queued, return the bytes_queued counter*/
			return bytes_queued;
		}

		size_t UartDriver::send_data(const char* data, size_t len)
		{
			/*Check if UART is initialized and if data string is not NULL, and if len is not 0*/
			if(!m_init || !data || len == 0) return 0;

			/*local variable to count the bytes successfully queued*/
			size_t bytes_queued = 0;

			/*loop through the data to send multiple bytes, even if there are null characters in the middle of the data*/
			for(size_t i = 0; i < len; i++)
			{
				/*send each byte at a time. If done successfully, increment bytes_queued counter and advance in the string*/
				if(send_byte(data[i]))
				{
					bytes_queued ++;
				}
				/*If not successful (e.g. if TX queue is full), break the queuing loop*/
				else
				{
					break;
				}
			}

			/*once the data has been completely queued, return the bytes_queued counter*/
			return bytes_queued;
		}

		bool UartDriver::receive_byte(uint8_t& byte_out)
		{
			/*Check if UART is initialized*/
			if(!m_init) return false;

			/*dequeue byte from RX buffer to byte_out and return success flag*/
			return m_rx_buffer.dequeue(byte_out);
		}

		void UartDriver::isr_handle_rx()
		{
			/*Check if SR RXNE (RX not empty) is set*/
			if((m_usart->SR & USART_SR_RXNE) != 0)
			{
				/*save the data from the DR (data register) a local variable*/
				uint8_t received_byte = static_cast<uint8_t>(m_usart->DR & 0xFF);

				/*Enqueue the read data stored in the local variable to the RX buffer and send error if it fails (e.g. if the buffer is full)*/
				if(!m_rx_buffer.enqueue(received_byte))
				{
					// ERROR MSG
				}
			}
		}

		void UartDriver::isr_handle_tx()
		{
			/*Check if SR TXE (TX Empty) and CR1 TXEIE (TX Empty Interrupt Enable) are set*/
			if((m_usart->SR & USART_SR_TXE) != 0 && (m_usart->CR1 & USART_CR1_TXEIE) != 0)
			{
				/*Local variable to hold the data to send*/
				uint8_t byte_to_send;

				/*Dequeue a byte from the TX buffer (software buffer)*/
				if(m_tx_buffer.dequeue(byte_to_send))
				{
					/*Place the dequeued byte in the data register (hardware buffer)*/
					m_usart->DR = (byte_to_send & 0xFF);
				}
				/*If the dequeue is not successful (e.g. if the TX buffer is empty), disable the TX Empty Interrupt*/
				else
				{
					m_usart->CR1 &=~ USART_CR1_TXEIE;
				}
			}
		}


	}
}
