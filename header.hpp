#pragma once

#include "json.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <set>

typedef unsigned char byte;

struct TestFunctions{
   struct Context{
      json::JSON& parent;
      int ffxId;
   };

   typedef std::function<void(json::JSON&, Context)> TFunc;

   static void defaultFunc(json::JSON& obj, Context context){}

   TFunc onData3 = &defaultFunc;
   TFunc onAST = &defaultFunc;
   TFunc onPond1 = &defaultFunc;
   TFunc onPond2 = &defaultFunc;
   TFunc onPond3 = &defaultFunc;
   TFunc onPond2Subtype = &defaultFunc;
};

// utility.cpp

#include "utility.inl"

void getPathInfo(const std::wstring& path, std::wstring& dir, std::wstring& fileName);
std::wstring getExtension(const std::wstring& path);
std::vector<std::wstring> getFileNamesInDir(std::wstring path);

// ffxToJson.cpp

void ffxToJson(const std::wstring& ffxPath, const std::wstring& jsonPath, const TestFunctions& testFunctions = TestFunctions());

// jsonToFfx.cpp

void jsonToFfx(const std::wstring& jsonPath, const std::wstring& ffxPath);

// entryPoint.cpp

extern int jsonVersion;

void ffxReadError(const std::wstring& path, const std::wstring& text);
void ffxWriteError(const std::wstring& path, const std::wstring& text);
void jsonReadError(const std::wstring& path, const std::wstring& text);
void jsonWriteError(const std::wstring& path, const std::wstring& text);
