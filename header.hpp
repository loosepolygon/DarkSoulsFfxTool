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

class DataWriter{
   struct OffsetToFix{
      int writeOffsetHere;
      DataWriter* dwSource;
      int readThisOffset;

      OffsetToFix(int b, DataWriter& c, int d) : 
         writeOffsetHere(b),
         dwSource(&c),
         readThisOffset(d)
      {}
   };

   std::vector<OffsetToFix> offsetsToFix;

public:

   std::vector<byte> bytes;
   int finalOffset = 0;
   int padToMultiple = 0;

   template<typename T>
   void write(T t){
      this->bytes.resize(this->bytes.size() + sizeof(T));
      byte* dest = this->bytes.data() + (this->bytes.size() - sizeof(T));
      *reinterpret_cast<T*>(dest) = t;
   }

   template<typename T>
   void writeAt(int offset, T t){
      if(this->bytes.size() < offset + sizeof(T)){
         this->bytes.resize(offset + sizeof(T));
      }

      byte* dest = this->bytes.data() + offset;
      *reinterpret_cast<T*>(dest) = t;
   }

   template<typename T>
   T read(int offset){
      return *reinterpret_cast<T*>(&this->bytes[offset]);
   }

   void addOffsetToFixAt(int writeOffsetHere, DataWriter& otherDW, int offsetToWrite){
      this->offsetsToFix.emplace_back(writeOffsetHere, otherDW, offsetToWrite);
   }

   void writeOffsetToFix(DataWriter& otherDW, int offsetToWrite){
      this->addOffsetToFixAt(this->bytes.size(), otherDW, offsetToWrite);
      this->write<int>(-1);
   }

   void replaceOffsetToFix(int writeOffsetHere, DataWriter& otherDW, int offsetToWrite){
      for(auto& offsetToFix : this->offsetsToFix){
         if(offsetToFix.writeOffsetHere == writeOffsetHere){
            offsetToFix.dwSource = &otherDW;
            offsetToFix.readThisOffset = offsetToWrite;

            return;
         }
      }

      throw;
   }

   void merge(std::vector<DataWriter*> dataWriters, std::vector<int>& offsetList){
      // Concat bytes
      for(DataWriter* dw : dataWriters){
         dw->finalOffset = this->bytes.size() + this->finalOffset;

         this->bytes.insert(this->bytes.end(), dw->bytes.begin(), dw->bytes.end());

         if(dw->padToMultiple != 0){
            while(this->bytes.size() % dw->padToMultiple != 0){
               this->write<byte>(0);
            }
         }
      }

      // Fix offsets
      dataWriters.push_back(this);
      for(DataWriter* dw : dataWriters){
         for(OffsetToFix& offsetToFix : dw->offsetsToFix){
            int writeOffsetHere = offsetToFix.writeOffsetHere + dw->finalOffset;
            int readThisOffset = offsetToFix.readThisOffset + offsetToFix.dwSource->finalOffset;
            this->writeAt<int>(writeOffsetHere, readThisOffset);

            offsetList.push_back(writeOffsetHere);
         }

         dw->offsetsToFix.clear();
      }
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

struct FlexibleData{
   int type = -1;

   template<typename T>
   T* get(){
      return reinterpret_cast<T*>(this->data.data());
   }

   template<typename T>
   void set(T& t){
      this->data.resize(sizeof(T));
      *reinterpret_cast<T*>(this->data.data()) = t;
   }

private:
   std::vector<int> data;
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

struct Pond3{
   int address;
   FlexibleData data;
};

void loadFfxFile(Ffx& ffx, std::wstring path, std::set<int>* allPond2Types = nullptr, std::vector<Pond2*>* allPond2s = nullptr, std::vector<Pond3*>* = nullptr);

// ffxToJson.cpp

void ffxToJson(const std::wstring& ffxPath, const std::wstring& jsonPath);

// jsonToFfx.cpp

void jsonToFfx(const std::wstring& jsonPath, const std::wstring& ffxPath);

// entryPoint.cpp

extern FILE* currentGlobalFile;
extern int jsonVersion; // :/

void outputFfxAnalysis(std::wstring fileName, bool rawPath = false);

void ffxReadError(const std::wstring& path, const std::wstring& text);
void ffxWriteError(const std::wstring& path, const std::wstring& text);
void jsonReadError(const std::wstring& path, const std::wstring& text);
void jsonWriteError(const std::wstring& path, const std::wstring& text);
