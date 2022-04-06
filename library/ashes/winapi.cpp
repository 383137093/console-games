﻿#include "winapi.h"
#include <cassert>
#include <tchar.h>

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

}}