﻿#ifndef ASHES_WINAPI_H
#define ASHES_WINAPI_H

#include <string>
#include <windows.h>

namespace ashes { namespace win {

std::string LoadModuleResource(const char* name, const char* type);

bool IsAtLocaleZN();

}}

#endif