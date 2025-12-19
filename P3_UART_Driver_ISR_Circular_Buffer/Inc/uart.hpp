#pragma once

#include "stm32f4xx.h"
#include "circular_queue.hpp"
#include "project_config.hpp"
#include <cstddef>
#include <cstdint>

namespace emb{

	namespace drivers{
	/*Define buffer size for UART tx and rx (in bytes)*/
	constexpr size_t UART_RX_BUFFER_SIZE = 64;
	constexpr size_t UART_TX_BUFFER_SIZE = 64;

	class UartDriver{

	public:

		/*Enum class for UART parity options*/
		enum class Parity{
			NONE,
			EVEN,
			ODD
		};

		/*Enum class for UART stop bits options*/
		enum class StopBits{
			ONE,
			TWO
		};

		/*Constructor*/
		UartDriver(USART_TypeDef* usart_instance);

		/*Destructor*/
		~UartDriver() = default;

		/*Delete copy*/
		UartDriver(const UartDriver&) = delete;
		UartDriver& operator = (const UartDriver&) = delete;

		/*Delete move*/
		UartDriver(const UartDriver&&) = delete;
		UartDriver& operator = (const UartDriver&&) = delete;

		/*@brief: Initialize UART peripheral settings
		 *@params: baud_rate, data_bits, parity, stop_bits
		 *@return: true if initialization was successful*/
		bool init(uint32_t baud_rate,
				  uint8_t data_bits = 8,
				  Parity parity = Parity::NONE,
				  StopBits stop_bits = StopBits:: ONE);

		/*@brief: send a byte asynchronously, by adding it to the TX queue (which is then sent via ISR)
		 *@params: byte, data to send
		 *@return: true if byte sent successfully (e.g. tx queue not full)*/
		bool send_byte(uint8_t byte);

		/*@brief: send a null terminated string asynchronously
		 *@params: str, pointer to string to send
		 *@return: number of bytes successfully queued for transmission, like a sending counter*/
		size_t send_string(const char* str);

		/*@brief: send a block of data asynchronously
		 *@params: data, pointer to the data buffer
		 *@params: len, length number of bytes to send
		 *@return: number of bytes successfully queued for transmission, like a sending counter*/
		size_t send_data(const char* data, size_t len);

		/*@brief: receive a byte, by removing it from the RX queue
		 *@params: byte_out, reference to store the received byte
		 *@return: true if byte received successfully*/
		bool receive_byte(uint8_t& byte_out);

		/*ISR (Interrupt Service Routine) access to RX queue*/
		void isr_handle_rx();

		/*ISR (Interrupt Service Routine) access to TX queue*/
		void isr_handle_tx();

	private:
		/*Configure GPIO pins for UART*/
		void configure_gpio();

		/*Set UART baud rate*/
		void set_baud_rate(uint32_t baud_rate);

		/*Enable UART and its interrupts*/
		void enable_uart_interrupts();

		/*Pointer to the UART peripheral registers (e.g. USART2)*/
		USART_TypeDef* const m_usart;

		/*RX circular queue*/
		emb::data_strucs::CircularQueue<uint8_t, UART_RX_BUFFER_SIZE> m_rx_buffer;

		/*TX circular queue*/
		emb::data_strucs::CircularQueue<uint8_t, UART_TX_BUFFER_SIZE> m_tx_buffer;

		/*Flag to indicate if UART is initialized*/
		bool m_init;
	};

	extern UartDriver* g_usart2_driver_ptr;

	}
}
