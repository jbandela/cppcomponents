#pragma once

#ifndef INCLUDE_GUARD_CPPCOMPONENTS_SPINLOCK_HPP_10_30_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_SPINLOCK_HPP_10_30_2013_

#include <atomic>
#include <cstdint>

namespace cppcomponents{

	// Reader/writer spin lock
	// Algorithm from http://locklessinc.com/articles/locks/
	//typedef struct dumbrwlock dumbrwlock;
	//struct dumbrwlock
	//{
	//	spinlock lock;
	//	unsigned readers;
	//};
	//
	//static void dumb_wrlock(dumbrwlock *l)
	//{
	//	/* Get write lock */
	//	spin_lock(&l->lock);
	//
	//	/* Wait for readers to finish */
	//	while (l->readers) cpu_relax();
	//}
	//
	//static void dumb_wrunlock(dumbrwlock *l)
	//{
	//	spin_unlock(&l->lock);
	//}
	//
	//static int dumb_wrtrylock(dumbrwlock *l)
	//{
	//	/* Want no readers */
	//	if (l->readers) return EBUSY;
	//
	//	/* Try to get write lock */
	//	if (spin_trylock(&l->lock)) return EBUSY;
	//
	//	if (l->readers)
	//	{
	//		/* Oops, a reader started */
	//		spin_unlock(&l->lock);
	//		return EBUSY;
	//	}
	//
	//	/* Success! */
	//	return 0;
	//}
	//
	//static void dumb_rdlock(dumbrwlock *l)
	//{
	//	while (1)
	//	{
	//		/* Speculatively take read lock */
	//		atomic_inc(&l->readers);
	//
	//		/* Success? */
	//		if (!l->lock) return;
	//
	//		/* Failure - undo, and wait until we can try again */
	//		atomic_dec(&l->readers);
	//		while (l->lock) cpu_relax();
	//	}
	//}
	//
	//static void dumb_rdunlock(dumbrwlock *l)
	//{
	//	atomic_dec(&l->readers);
	//}
	//
	//static int dumb_rdtrylock(dumbrwlock *l)
	//{
	//	/* Speculatively take read lock */
	//	atomic_inc(&l->readers);
	//
	//	/* Success? */
	//	if (!l->lock) return 0;
	//
	//	/* Failure - undo */
	//	atomic_dec(&l->readers);
	//
	//	return EBUSY;
	//}
	//
	//static int dumb_rdupgradelock(dumbrwlock *l)
	//{
	//	/* Try to convert into a write lock */
	//	if (spin_trylock(&l->lock)) return EBUSY;
	//
	//	/* I'm no longer a reader */
	//	atomic_dec(&l->readers);
	//
	//	/* Wait for all other readers to finish */
	//	while (l->readers) cpu_relax();
	//
	//	return 0;
	//}

	// With improvements to prevent write starvation


	class spinlock{
	private:
		std::atomic<bool> lock_;
		spinlock(const spinlock&);
		spinlock& operator=(const spinlock&);


	public:
		spinlock() :lock_{ false }{}

		void lock(){
			while (lock_.exchange(true));
		}

		void unlock(){
			lock_.store(false);
		}


	};

	struct rw_lock{
	private:
		std::atomic<std::int32_t> readers_;
		std::atomic<bool> writer_;
		std::atomic<std::int32_t> pending_writers_;
		rw_lock(const rw_lock&);
		rw_lock& operator=(const rw_lock&);

	public:

		rw_lock() :readers_{ 0 }, writer_{ false }, pending_writers_{ false }{}

		void read_lock(){
			while (pending_writers_.load());

			for(;;){
				// speculatively take read lock
				readers_.fetch_add(1);

				// Success ?
				if (writer_.load() == 0) return;

				// Failure - undo and wait until we can try again
				readers_.fetch_sub(1);
				while (writer_.load());
			}
		}

		void read_unlock(){
			readers_.fetch_sub(1);
		}

		void write_lock(){

			pending_writers_.fetch_add(1);

			// Get the write lock
			while (writer_.exchange(true));
			// Wait for readers to finish
			while (readers_.load());

			pending_writers_.fetch_sub(1);

		}

		void write_unlock(){

			writer_.store(false);
		}

		void upgrade_lock(){
			//  Try to convert into a write lock 
			while (writer_.exchange(true));

			// I am no longer a reader
			readers_.fetch_sub(1);

			// Wait for other readers to finish
			while (readers_.load());
		}

	};

	struct spin_locker{

	private:
		std::atomic<bool>& lock_;

		spin_locker(const spin_locker&);
		spin_locker& operator=(const spin_locker&);

	public:
		spin_locker(std::atomic<bool>& lock) :lock_(lock ){
			while (lock_.exchange(true));
		}

		~spin_locker(){
			lock_.store(false);
		}


	};

	struct rw_locker{

	private:
		rw_lock& lock_;
		bool writer_;

		rw_locker(const rw_locker&);
		rw_locker& operator=(const rw_locker&);


	public:
		rw_locker(rw_lock& lock, bool writer = false) :lock_( lock ), writer_{ writer }{
			if (writer_){
				lock_.write_lock();
			}
			else{
				lock_.read_lock();
			}
		}

		~rw_locker(){
			if (writer_){
				lock_.write_unlock();
			}
			else{
				lock_.read_unlock();
			}
		}

		void upgrade(){
			lock_.upgrade_lock();
			writer_ = true;
		}




	};
}

#endif
