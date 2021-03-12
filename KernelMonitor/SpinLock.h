#pragma once
#include <ntddk.h>

namespace kstd {
	struct SpinLock {
		SpinLock() {
			init();
		}
		void init() {
			KeInitializeSpinLock(&lock_);
		}
		void lock() {
			KeAcquireSpinLock(&lock_, &old_irql_);
		}
		void unlock() {
			KeReleaseSpinLock(&lock_, old_irql_);
		}
	private:
		KSPIN_LOCK lock_;
		KIRQL old_irql_;
	};
}