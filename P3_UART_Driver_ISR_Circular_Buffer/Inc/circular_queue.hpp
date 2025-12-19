/*ISR-safe circular queue*/

#pragma once

#include <cstdint>
#include <cstddef>
#include <array>

/*Template based*/
namespace emb{

	namespace data_strucs{

		template<typename T, size_t Capacity>

		class CircularQueue{

		public:
			/*Constructor*/
			CircularQueue(): m_head(0), m_tail(0), m_count(0){}

			/*Destructor*/
			~CircularQueue() = default;

			/*Delete copy constructor and copy assignment operator to prevent accidental copying*/
			CircularQueue(const CircularQueue&) = delete;
			CircularQueue& operator = (const CircularQueue&) = delete;

			/*Delete move constructor and move assignment operator to prevent accidental movement*/
			CircularQueue(const CircularQueue&&) = delete;
			CircularQueue& operator = (const CircularQueue&&) = delete;

			/*@brief is empty method: compare if m_count is equal to 0.
			 *@return True if empty*/
			bool is_empty() const{
				return m_count == 0;
			}

			/*@is full method: compare if m_count is equal to Capacity.
			 *@return True if full*/
			bool is_full() const{
				return m_count == Capacity;
			}

			/*@enqueue method: enqueue new item into the queue (to the tail)
			 *@param item_in is the new item to be enqueue, which is of type T
			 *@return True if successfully enqueued*/
			bool enqueue(const T& item_in){
				/*if full, can't enqueue*/
				if(is_full())
				{
					return false;
				}

				/*Place the element at the tail*/
				m_buffer[m_tail] = item_in;

				/*Increment tail index*/
				m_tail = (m_tail + 1) % Capacity;

				/*Increment Count*/
				m_count ++;

				/*Return flag*/
				return true;
			}

			/*@dequeue method: dequee item from the queue (from the head)
			 *@param item_out is the reference to store the dequeued element
			 *@return 1 if successfully dequeued*/
			bool dequeue(T& item_out){
				/*if empty, can't dequeue*/
				if(is_empty())
				{
					return false;
				}

				/*Retrieve the item from the head and store it in item_out*/
				item_out = m_buffer[m_head];

				/*Increment head index*/
				m_head = (m_head + 1) % Capacity;

				/*Decrement count*/
				m_count --;

				/*Return flag*/
				return true;
			}

			/*@peek method: allows to get the item at the front of the queue without removing it
			 *@param peeked_item: current item at the head
			 *@return true if element was peeked successfully*/
			bool peek(T& peeked_item) const{

				/*make a copy of count because it is volatile*/
				size_t current_count = m_count;

				/*if empty, can't peek*/
				if(current_count == 0)
				{
					return false;
				}

				/*Read the item from the head, without removing it, and store it in peeked_item*/
				peeked_item = m_buffer[m_head];

				/*return flag*/
				return true;
			}

			/*helper function to get the count*/
			size_t get_coun() const{
				return m_count;
			}

			/*helper function to get the capacity*/
			constexpr size_t get_capacity() const{
				return Capacity;
			}

			/*helper function to clear the head and tail indexes and count*/
			void clear(){
				m_head = 0;
				m_tail = 0;
				m_count = 0;
			}

		private:
			/*Buffer to store queue data*/
			std::array<T, Capacity> m_buffer;

			/*Index of the head of the queue: next item to be dequeued*/
			volatile size_t m_head;

			/*Index of the tail of the queue: next position to be enqueued*/
			volatile size_t m_tail;

			/*Count: current number of elements in the queue*/
			volatile size_t m_count;

		};
	}
}
