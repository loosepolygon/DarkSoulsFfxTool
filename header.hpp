#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <set>


typedef unsigned char byte;


template<typename TArray, typename TElement>
bool includes(const TArray& arr, TElement element){
   return std::find(std::begin(arr), std::end(arr), element) != std::end(arr);
}

template<typename TArray>
int findIndex(const TArray& arr, decltype(arr[0]) element){
   int findResult = -1;
   for(int n = 0; n < (int)std::size(arr); ++n){
      if(arr[n] == element){
         findResult = n;
         break;
      }
   }

   return findResult;
}

template<typename TArray>
int findIndex(const TArray& arr, std::function<bool(decltype(arr[0])&)> lambda){
   int findResult = -1;
   for(int n = 0; n < (int)std::size(arr); ++n){
      if(lambda(arr[n])){
         findResult = n;
         break;
      }
   }

   return findResult;
}

struct DataReader{
   std::vector<byte> bytes;
   std::vector<bool> bytesRead;

   int readInt(int addr){
      std::vector<bool>& br = this->bytesRead;
      br[addr+3] = br[addr+2] = br[addr+1] = br[addr+0] = true;
      return *reinterpret_cast<int*>(&this->bytes[addr]);
   }

   float readFloat(int addr){
      std::vector<bool>& br = this->bytesRead;
      br[addr+3] = br[addr+2] = br[addr+1] = br[addr+0] = true;
      return *reinterpret_cast<float*>(&this->bytes[addr]);
   }

   byte readByte(int addr){
      std::vector<bool>& br = this->bytesRead;
      br[addr] = true;
      return *reinterpret_cast<byte*>(&this->bytes[addr]);
   }
};

// loadFfxFile.cpp

struct Ffx{
   int version;

   int unk1; // Always 1
   int unk2; // Always 0

   // From Type133
   int id = -1;
};

struct Pond2{
   int address = -1;
   int type = -1;
   int totalSize = -1;
   int preDataCount = -1;
   int offsetToPreDataNumbers = -1;
   int offsetToPreDataSubtypes = -1;
   int offsetToSourceAST = -1;
   std::vector<byte> data;
};

void loadFfxFile(Ffx& ffx, std::wstring path, std::set<int>* allPond2Types = nullptr, std::vector<Pond2*>* allPond2s = nullptr);

// ffxToJson.cpp

void ffxToJson(const std::wstring& ffxPath, const std::wstring& jsonPath);

// entryPoint.cpp

extern FILE* currentGlobalFile;

void outputFfxAnalysis(std::wstring fileName);

void ffxReadError(const std::wstring& path, const std::wstring& text);
