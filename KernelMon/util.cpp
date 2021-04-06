#include "util.h"

std::wstring utf8ToUtf16(const std::string& utf8Str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.from_bytes(utf8Str);
}

std::string utf16ToUtf8(const std::wstring& utf16Str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.to_bytes(utf16Str);
}


const std::unordered_map<MonitoredFunctions, std::string_view> monitored_functions_map = {
   {MonitoredFunctions::ZwCreateFile, "ZwCreateFile"},
   {MonitoredFunctions::ZwWriteFile, "ZwWriteFile"},
   {MonitoredFunctions::ZwCreateKey, "ZwCreateKey"}
};