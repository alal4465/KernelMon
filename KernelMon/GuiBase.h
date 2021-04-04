#pragma once
#include <Windows.h>

namespace Gui {

	template <typename THandle>
	class GuiBase {
	public:
		GuiBase(const GuiBase& rhs) = delete;
		GuiBase& operator=(const GuiBase& rhs) = delete;
		virtual ~GuiBase() = default;
		THandle handle() const { return handle_; }

	protected:
		GuiBase(THandle handle) : handle_(handle) {}

	private:

		THandle handle_;
	};

}