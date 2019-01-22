#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <set>

typedef unsigned char byte;

#include "utility.inl"

// ffxToJson.cpp

void ffxToJson(const std::wstring& ffxPath, const std::wstring& jsonPath);

// jsonToFfx.cpp

void jsonToFfx(const std::wstring& jsonPath, const std::wstring& ffxPath);

// entryPoint.cpp

extern int jsonVersion;

void ffxReadError(const std::wstring& path, const std::wstring& text);
void ffxWriteError(const std::wstring& path, const std::wstring& text);
void jsonReadError(const std::wstring& path, const std::wstring& text);
void jsonWriteError(const std::wstring& path, const std::wstring& text);
