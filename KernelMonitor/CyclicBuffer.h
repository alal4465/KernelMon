#pragma once
#include "LockGuard.h"
#include "SpinLock.h"

namespace kstd {
	template <typename T, typename TLock>
	class CyclicBuffer final {
	public:
		explicit CyclicBuffer(size_t size, POOL_TYPE pool_type = PagedPool) :
			buf_(new(pool_type) T[size]),
			max_size_(size)
		{}

		~CyclicBuffer() {
			delete[] buf_;
		}

		void push(const T& item) {
			kstd::LockGuard<TLock> lock(lock_);

			/*
			if (full_) {
				KdPrint(("[-] CyclicBuffer overflow with max size: llu", max_size_));
				tail_ = (tail_ + 1) % max_size_;
			}
			*/
			if (full_) {
				return;
			}

			buf_[head_] = item;

			head_ = (head_ + 1) % max_size_;
			full_ = head_ == tail_;
		}

		bool pop(T& result) {
			kstd::LockGuard<TLock> lock(lock_);

			if (empty())
				return false;

			result = buf_[tail_];
			full_ = false;
			tail_ = (tail_ + 1) % max_size_;

			return true;
		}

		void reset() {
			kstd::LockGuard<TLock> lock(lock_);

			head_ = tail_;
			full_ = false;
		}

		bool empty() const {
			return (!full_ && (head_ == tail_));
		}

		bool full() const {
			return full_;
		}

		size_t capacity() const {
			return max_size_;
		}

		size_t size() const {
			size_t size = max_size_;

			if (!full_) {
				if (head_ >= tail_) {
					size = head_ - tail_;
				}
				else {
					size = max_size_ + head_ - tail_;
				}
			}

			return size;
		}

	private:
		TLock lock_;
		T* buf_;
		size_t head_ = 0;
		size_t tail_ = 0;
		const size_t max_size_;
		bool full_ = 0;
	};
};