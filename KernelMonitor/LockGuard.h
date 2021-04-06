#pragma once
#include "SpinLock.h"

namespace kstd {
	template<typename TLock>
	struct LockGuard final{
		LockGuard(TLock& lock) : lock_(lock) {
			lock_.lock();
		}
		~LockGuard() {
			lock_.unlock();
		}
	private:
		TLock& lock_;
	};
}