#pragma once

#include "json.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <numeric>
#include <set>
#include <limits>

typedef unsigned char byte;

struct TestFunctions{
   struct Context{
      json::JSON& parent;
      json::JSON& root;
      std::string varName;
   };

   enum : int{
      Pre1,
      Pre2,
      Blossom,
      Type37,
      Type38,
   };

   typedef std::function<void(json::JSON&, Context)> TFunc1;
   typedef std::function<void(json::JSON&, Context, int, int)> TFunc2;

   static void defaultFunc1(json::JSON& obj, Context context){}
   static void defaultFunc2(json::JSON& obj, Context context, int sourceType, int arg){}

   // These are called by ffxToJson
   TFunc1 onRoot = &defaultFunc1;
   TFunc1 onData3 = &defaultFunc1;
   TFunc2 onAST = &defaultFunc2;
   TFunc1 onPond1 = &defaultFunc1;
   TFunc1 onPond2 = &defaultFunc1;
   TFunc1 onPond3 = &defaultFunc1;
   TFunc1 onPond2Subtype = &defaultFunc1;
};

// utility.cpp

void getPathInfo(const std::wstring& path, std::wstring& dir, std::wstring& fileName);
std::wstring getExtension(const std::wstring& path);
std::vector<std::wstring> getFileNamesInDir(std::wstring path);
std::vector<byte> getFileBytes(const std::wstring& filePath);

// ffxToJson.cpp

void ffxToJson(const std::wstring& ffxPath, const std::wstring& jsonPath, const TestFunctions& testFunctions = TestFunctions());

// jsonToFfx.cpp

void jsonToFfx(const std::wstring& jsonPath, const std::wstring& ffxPath);

// testing.cpp

void testing();

// analysisSpreadsheet.cpp

void generateAnalysisSpreadsheet(std::wstring originalDir, std::wstring jsonDir);

// entryPoint.cpp

extern int jsonVersion;

void ffxReadError(const std::wstring& path, const std::wstring& text);
void ffxWriteError(const std::wstring& path, const std::wstring& text);
void jsonReadError(const std::wstring& path, const std::wstring& text);
void jsonWriteError(const std::wstring& path, const std::wstring& text);

#include "utility.inl"
