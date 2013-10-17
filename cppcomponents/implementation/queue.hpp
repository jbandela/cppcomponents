//          Copyright John R. Bandela 2013.
	// Distributed under the Boost Software License, Version 1.0.
	//    (See accompanying file LICENSE_1_0.txt or copy at
	//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef INCLUDE_GUARD_CPPCOMPONENTS_QUEUE_HPP_05_29_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_QUEUE_HPP_05_29_2013_

#include <atomic>

namespace cppcomponents{


	// Multiproducer consumer queue from Herb Sutter
	//http://www.drdobbs.com/parallel/writing-a-generalized-concurrent-queue/211601363

	template <typename T, std::size_t CacheLineSize = 64>
	struct low_lock_queue {
	private:
		struct Node {
			Node(T* val) : value(val), next(nullptr) { }
			T* value;
			std::atomic<Node*> next;
			char pad[CacheLineSize - sizeof(T*) - sizeof(std::atomic<Node*>)];
		};

		char pad0[CacheLineSize];

		// for one consumer at a time
		Node* first;

		char pad1[CacheLineSize
			- sizeof(Node*) ];

		// shared among consumers
		std::atomic<bool> consumerLock;

		char pad2[CacheLineSize
			- sizeof(std::atomic<bool>)];

		// for one producer at a time
		Node* last;

		char pad3[CacheLineSize
			- sizeof(Node*) ];

		// shared among producers
		std::atomic<bool> producerLock;

		char pad4[CacheLineSize
			- sizeof(std::atomic<bool>)];

	public:
		low_lock_queue() {
			first = last = new Node(nullptr);
			producerLock = consumerLock = false;
		}
		~low_lock_queue() {
			while (first != nullptr) {
				// release the list
				Node* tmp = first;
				first = tmp->next;
				delete tmp->value;       // no-op if null
				delete tmp;
			}
		}

		void produce(const T& t) {
			Node* tmp = new Node(new T(t));
			while (producerLock.exchange(true))
			{
			}   // acquire exclusivity
			last->next = tmp;         // publish to consumers
			last = tmp;             // swing last forward
			producerLock = false;       // release exclusivity
		}

		bool consume(T& result) {
			while (consumerLock.exchange(true))
			{
			}    // acquire exclusivity
			Node* theFirst = first;
			Node* theNext = first->next;
			if (theNext != nullptr) {
				// if queue is nonempty
				T* val = theNext->value;    // take it out
				theNext->value = nullptr;  // of the Node
				first = theNext;          // swing first forward
				consumerLock = false;             // release exclusivity
				result = std::move(*val);    // now copy it back
				delete val;       // clean up the value
				delete theFirst;      // and the old dummy
				return true;      // and report success
			}
			consumerLock = false;   // release exclusivity
			return false;                  // report queue was empty
		}

		bool empty(){
			while (consumerLock.exchange(true))
			{
			}    // acquire exclusivity
			bool b = (first->next.load() == nullptr);
			
			consumerLock = false; // release exclusivity

			return b;
			
		}

	};


}


#endif