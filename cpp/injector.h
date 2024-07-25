#pragma once
#include <windows.h>
#include <string>

static bool inject(DWORD pid, const std::string& dllPath);