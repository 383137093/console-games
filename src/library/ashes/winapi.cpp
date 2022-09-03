#include "winapi.h"
#include <cwchar>
#include <cassert>

namespace ashes { namespace win {

std::string LoadModuleResource(const char* name, const char* type)
{
	HRSRC handle = ::FindResourceA(NULL, name, type);
	assert(handle != NULL);

	DWORD size = ::SizeofResource(NULL, handle);
	assert(size > 0);

	HGLOBAL data = ::LoadResource(NULL, handle);
	assert(data != NULL);

	LPVOID locked_data = ::LockResource(data);
	assert(locked_data != nullptr);

	std::string str(reinterpret_cast<char*>(locked_data), size);
	::FreeResource(data);

	return str;
}

bool IsAtLocaleZN()
{
	wchar_t name[LOCALE_NAME_MAX_LENGTH];
	::GetUserDefaultLocaleName(name, static_cast<int>(std::size(name)));
	return ::_wcsnicmp(name, L"ZH", 2) == 0;
}

std::wstring A2W(const std::string& str, UINT code_page)
{
	return A2W(str.data(), static_cast<int>(str.size()), code_page);
}

std::string W2A(const std::wstring& wstr, UINT code_page)
{
	return W2A(wstr.data(), static_cast<int>(wstr.size()), code_page);
}

std::wstring A2W(const char* str, int len, UINT code_page)
{
	len = (len < 0 ? static_cast<int>(std::strlen(str)) : len);
	int wlen = ::MultiByteToWideChar(code_page, 0, str, len, nullptr, 0);
	if (wlen <= 0)
		return {};

	std::wstring wstr(wlen, 0);
	wlen = ::MultiByteToWideChar(code_page, 0, str, len, &wstr[0], wlen);
	if (wlen != static_cast<int>(wstr.size()))
		return {};

	return wstr;
}

std::string W2A(const wchar_t* wstr, int wlen, UINT code_page)
{
	wlen = (wlen < 0 ? static_cast<int>(std::wcslen(wstr)) : wlen);
	int len = ::WideCharToMultiByte(code_page, 0, wstr, wlen, nullptr, 0, 0, 0);
	if (len <= 0)
		return {};

	std::string str(len, 0);
	len = ::WideCharToMultiByte(code_page, 0, wstr, wlen, &str[0], len, 0, 0);
	if (len != static_cast<int>(str.size()))
		return {};
	
	return str;
}

}}