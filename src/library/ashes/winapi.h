#ifndef ASHES_WINAPI_H
#define ASHES_WINAPI_H

#include <string>
#include <windows.h>

namespace ashes { namespace win {

std::string LoadModuleResource(const char* name, const char* type);

bool IsAtLocaleZN();

std::wstring A2W(const std::string& str, UINT code_page);
std::string W2A(const std::wstring& wstr, UINT code_page);

std::wstring A2W(const char* str, int len, UINT code_page);
std::string W2A(const wchar_t* wstr, int wlen, UINT code_page);

}}

#endif