#pragma once
#include <codecvt>
#include <string>
#include <unordered_map>
#include <Windows.h>
#include "../KernelMonitor/common.h"

std::wstring utf8ToUtf16(const std::string& utf8Str);
std::string utf16ToUtf8(const std::wstring& utf16Str);

extern const std::unordered_map<MonitoredFunctions, std::string_view> monitored_functions_map;
