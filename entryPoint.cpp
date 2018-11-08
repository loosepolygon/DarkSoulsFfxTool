#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <set>

typedef unsigned char byte;

void ffxReadError(const std::wstring& path, const std::wstring& text){
   std::wstring errorMessage = L"Error reading file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());
   throw;
}

void ffxAssumptionWrong(const std::wstring& path, const std::wstring& text){
   const wchar_t* filePath = &path[path.size() - 1];
   while(--filePath != path.data()){
      if(*filePath == L'/'){
         ++filePath;
         break;
      }
   }
   std::wstring errorMessage = L"FFX assumption proved wrong for file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());
   throw;
}

void ffxResearchError(const std::wstring& path, const std::wstring& text){
   const wchar_t* filePath = &path[path.size() - 1];
   while(--filePath != path.data()){
      if(*filePath == L'/'){
         ++filePath;
         break;
      }
   }
   std::wstring errorMessage = L"Error for file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());
   throw;
}

//struct RawFfxFile{
//   struct Header{
//      char signature[4];
//      int version;
//      int preDataStart;
//      int data2Start;
//      int data2Count;
//      int data3Count;
//      int unk1;
//      int unk2;
//
//      int unknownVersion;
//      int ffxId;
//
//      byte unknownSection[80];
//   } header;
//
//   std::vector<byte> data1;
//   std::vector<int> data2;
//   std::vector<int> data3;
//};

struct RawFfxFile{
   struct Header{
      char signature[4];
      int version;
      int preDataStart;
      int data2Start;
      int data2Count;
      int data3Count;
      int unk1;
      int unk2;

      int unknownVersion;
      int ffxId;
      int unk3;

      //byte unknownSection[80];
   };

   std::vector<byte> bytes;

   Header* header = 0;
   int* data2 = 0;
   int* data3 = 0;

   int data3Start;
};

void loadRawFfxFile(RawFfxFile* rawFfx, const std::wstring& path){
   *rawFfx = RawFfxFile();

   FILE* file = _wfopen(path.c_str(), L"rb");
   if (file == NULL) {
      ffxReadError(path, L"Cannot open file");
      return;
   }

   fseek(file, 0, SEEK_END);
   long fileSize = ftell(file);
   fseek(file, 0, SEEK_SET);

   //fread(&rawFfx->header, 1, sizeof(RawFfxFile::Header), file);
   //char signature[4] = "FXR";
   //if(memcmp(signature, rawFfx->header.signature, 4) != 0){
   //   ffxReadError(path, L"Not an FFX file");
   //}

   /*int data1Size = rawFfx->header.data2Start - sizeof(RawFfxFile::Header);
   rawFfx->data1.resize(data1Size);
   fread(rawFfx->data1.data(), 1, rawFfx->data1.size(), file);

   rawFfx->data2.resize(rawFfx->header.data2Count);
   fread(rawFfx->data2.data(), 4, rawFfx->data2.size(), file);

   rawFfx->data3.resize(rawFfx->header.data3Count);
   fread(rawFfx->data3.data(), 4, rawFfx->data3.size(), file);

   if(ftell(file) != fileSize){
      ffxReadError(path, L"Still more file to read");
   }*/

   rawFfx->bytes.resize(fileSize);
   fread(rawFfx->bytes.data(), 1, rawFfx->bytes.size(), file);

   rawFfx->header = reinterpret_cast<RawFfxFile::Header*>(&rawFfx->bytes[0]);
   rawFfx->data2 = reinterpret_cast<int*>(&rawFfx->bytes[rawFfx->header->data2Start]);
   rawFfx->data3 = rawFfx->data2 + rawFfx->header->data2Count;

   rawFfx->data3Start = rawFfx->header->data2Start + rawFfx->header->data2Count * 4;

   fclose(file);
}

void testEveryFfx(std::wstring dir){
   std::vector<std::wstring> fileList;
   {
      std::wstring fileListPath = dir + L"fileList.txt";
      FILE* file = _wfopen(fileListPath.c_str(), L"r");

      fseek(file, 0, SEEK_END);
      long fileSize = ftell(file);
      fseek(file, 0, SEEK_SET);

      wchar_t endianness;
      fread(&endianness, sizeof(wchar_t), 1, file);

      std::wstring fullText;
      fullText.resize(fileSize - sizeof(wchar_t));
      fread(&fullText[0], sizeof(fullText[0]), fullText.size(), file);
      fclose(file);

      std::wstringstream ss(fullText);
      std::wstring item;
      while (std::getline(ss, item, L'\n')) {
         fileList.push_back(item);
      }
   }

   RawFfxFile rawFfx;
   int index = 1;
   int minSecondData2Pointer = 9999999;
   int maxSecondData2Pointer = 0;
   std::wstring minSecondData2PointerPath;
   std::wstring maxSecondData2PointerPath;
   std::vector<std::pair<int, int>> preDataSizeCounts;
   std::vector<std::vector<std::pair<int, int>>> preDataValues;
   for(const std::wstring& path : fileList){
      //wprintf_s(L"File %d/%d\n", index, fileList.size());
      ++index;

      loadRawFfxFile(&rawFfx, dir + path);

      //int lastData2Pointer = rawFfx.data3[rawFfx.header->data3Count - 1];
      //int type = *reinterpret_cast<int*>(&rawFfx.bytes[lastData2Pointer]);
      //if(type != 136){
      //   ffxAssumptionWrong(path, L"data3 does not end with type 136");
      //}

      if(rawFfx.header->unknownVersion != 133 && rawFfx.header->unknownVersion != 134){
         ffxAssumptionWrong(path, L"unknownVersion unfamiliar");
      }

      if(rawFfx.header->version != 65536){
         ffxAssumptionWrong(path, L"version unfamiliar");
      }

      if(rawFfx.header->preDataStart != 32){
         ffxAssumptionWrong(path, L"preDataStart unfamiliar");
      }

      if(rawFfx.header->unk1 != 1){
         ffxAssumptionWrong(path, L"unk1 unfamiliar");
      }

      if(rawFfx.header->unk2 != 0){
         ffxAssumptionWrong(path, L"unk2 unfamiliar");
      }

      if(rawFfx.header->unk3 != 0){
         ffxAssumptionWrong(path, L"unk3 unfamiliar");
      }

      std::wstring ffxIdText = &path[1];
      ffxIdText.resize(ffxIdText.size() - 4);
      int ffxIdFromFilename = _wtoi(ffxIdText.c_str());
      if(ffxIdFromFilename != rawFfx.header->ffxId){
         ffxAssumptionWrong(path, L"ffx ID does not match filename");
      }

      if(rawFfx.data2[0] != 8){
         ffxAssumptionWrong(path, L"first data2 pointer not 8");
      }

      if(rawFfx.data3[0] != 32){
         ffxAssumptionWrong(path, L"first data3 pointer not 32");
      }

      int secondData2Pointer = rawFfx.data2[1];
      if(secondData2Pointer < minSecondData2Pointer){
         minSecondData2Pointer = secondData2Pointer;
         minSecondData2PointerPath = path;
      }
      if(secondData2Pointer > maxSecondData2Pointer){
         maxSecondData2Pointer = secondData2Pointer;
         maxSecondData2PointerPath = path;
      }
      if(secondData2Pointer < 44 || secondData2Pointer > 120){
         ffxAssumptionWrong(path, L"secondData2Pointer out of range");
      }

      //bool hasType = false;
      //for(int n = 0; n < rawFfx.header->data3Count; ++n){
      //   int pointer = rawFfx.data3[n];
      //   int type = *reinterpret_cast<int*>(&rawFfx.bytes[pointer]);
      //   if(type == 37){
      //      hasType = true;
      //   }
      //}
      //if(hasType == false){
      //   ffxAssumptionWrong(path, L"No data3 type 37");
      //}

      if(rawFfx.header->data2Count < 7){
         ffxAssumptionWrong(path, L"Not enough data2 pointers");
      }

      if(rawFfx.header->data3Count < 2){
         ffxAssumptionWrong(path, L"Not enough data3 pointers");
      }

      bool has133 = false;
      for(int n = 0; n < rawFfx.header->data3Count; ++n){
         int pointer = rawFfx.data3[n];
         int* p = reinterpret_cast<int*>(&rawFfx.bytes[pointer]);
         int type = *p;
         ++p;
         if(type == 7){
            float value = *reinterpret_cast<float*>(p);
            //printf("%f\n", value);
         }else if(type == 37){
            int refFfxId = *p;
            if(refFfxId == 0){
               ++p;
               int next1 = *p;
               ++p;
               int next2 = *p;
               if(next1 != 0 || next2 != 0){
                  ffxAssumptionWrong(path, L"Type 37");
               }
            }else{
               //printf("%d\n", refFfxId);
            }
         }else if(type == 70){
            float value = *reinterpret_cast<float*>(p);
            if(value < -0.017f){
               //printf("%f\n", value);
            }
         }else if(type == 81){
            float value1 = *reinterpret_cast<float*>(p);
            ++p;
            float value2 = *reinterpret_cast<float*>(p);
            if(value1 != 0.0f && value2 != 0.0f && value1 != 1.0f && value2 != 1.0f){
               //printf("%f, %f\n", value1, value2);
            }
         }else if(type == 133){
            has133 = true;
         }
      }
      if(has133 == false){
         ffxAssumptionWrong(path, L"Type 133 missing");
      }

      // Test the second var in the second data2 pointer
      if(0){
         int delta = rawFfx.data2[2] - rawFfx.data2[1];
         delta /= 4;
         if(delta != 2){
            //printf("%d, %d\n", rawFfx.header->unknownVersion, delta);
         }
      }

      int preDataSize = rawFfx.data2[1] - 0x28;

      // Test the length of pre-data
      if(0){
         auto findResult = std::find_if(
            preDataSizeCounts.begin(),
            preDataSizeCounts.end(),
            [&](auto& pair) -> bool{return pair.first == preDataSize;}
         );
         if(findResult == preDataSizeCounts.end()){
            preDataSizeCounts.emplace_back(preDataSize, 0);
            findResult = preDataSizeCounts.end() - 1;
         }
         ++findResult->second;
      }

      // Test all known pre-data lengths
      {
         int unknownVersion = rawFfx.header->unknownVersion;
         if(preDataSize == 4){
            //printf("unknownVersion = %d\n", unknownVersion);
            if(unknownVersion != 134){
               ffxAssumptionWrong(path, L"unknownVersion mismatch with preDataSize");
            }
         }else if(preDataSize == 32){
            //printf("unknownVersion = %d\n", unknownVersion);
            if(unknownVersion != 133){
               ffxAssumptionWrong(path, L"unknownVersion mismatch with preDataSize");
            }
         }else if(preDataSize == 56){
            //printf("unknownVersion = %d\n", unknownVersion);
            if(unknownVersion != 133){
               ffxAssumptionWrong(path, L"unknownVersion mismatch with preDataSize");
            }
         }else if(preDataSize == 80){
            //printf("unknownVersion = %d\n", unknownVersion);
            if(unknownVersion != 133){
               ffxAssumptionWrong(path, L"unknownVersion mismatch with preDataSize");
            }
         }else{
            ffxAssumptionWrong(path, L"preDataSize umfamiliar");
         }
      }

      // int between ffxId and first data
      if(*reinterpret_cast<int*>(&rawFfx.bytes[0x28]) != 0){
         ffxAssumptionWrong(path, L"int after ffxId value unfamiliar");
      }

      // Pre-data analysis
      {
         int targetPreDataSize = 76;

         if(preDataSize - 4 == targetPreDataSize){
            int numValues = targetPreDataSize / 4;
            if(preDataValues.size() == 0){
               preDataValues.resize(numValues);
            }
            int* p = reinterpret_cast<int*>(&rawFfx.bytes[0x2c]);
            for(int n = 0; n < numValues; ++n){
               auto& values = preDataValues[n];
               int value = *p;

               auto findResult = std::find_if(
                  values.begin(),
                  values.end(),
                  [&](auto& pair) -> bool{return pair.first == value;}
               );
               if(findResult == values.end()){
                  values.emplace_back(value, 0);
                  findResult = values.end() - 1;
               }
               ++findResult->second;

               ++p;
            }
         }
      }

      // Pre-data enforcement
      do{
         int* p = reinterpret_cast<int*>(&rawFfx.bytes[0x2c]);
         if (preDataSize == 4) break;

         if(p[0] != 0 || p[1] != 0 || p[2] != 0 || p[3] != 0 || p[4] != 0 || p[5] != 0 || !(p[6] == 8 || p[6] == 10)){
            ffxAssumptionWrong(path, L"Pre-data value unfamiliar");
         }
         //printf("%d, %d\n", p[7], p[8]);

         if (preDataSize == 32) break;

         if(p[7] != 0 || p[8] != 0 || p[9] != 0 || p[10] != 65536 || p[11] != 0 || p[12] != 0){
            ffxAssumptionWrong(path, L"Pre-data value unfamiliar");
         }

         if (preDataSize == 56) break;

         if(p[13] != 0 || p[14] != 0 || p[15] != 0 || p[16] != 65536 || p[17] != 0 || p[18] != 0){
            ffxAssumptionWrong(path, L"Pre-data value unfamiliar");
         }
      }while(false);
   }

   wprintf_s(L"minSecondData2Pointer = %d '%s'\n", minSecondData2Pointer, minSecondData2PointerPath.c_str());
   wprintf_s(L"maxSecondData2Pointer = %d '%s'\n", maxSecondData2Pointer, maxSecondData2PointerPath.c_str());
}

struct Data2Entry{
   int originalAddress = -1;
   int pointerAddress = -1;
};

struct Param{
   enum Type{
      Unknown,
      Int,
      Float
   };

   Param(int value) : value(value), type(Type::Int){}
   Param(float value) : value(*reinterpret_cast<int*>(&value)), type(Type::Float){}

   int intValue(){return this->value;}
   float floatValue(){return *reinterpret_cast<float*>(this->value);}

   int type;
private: int value;
};

struct Data3Entry{
   int originalAddress = -1;
   int pointerAddress = -1;

   int type = -1;
   std::vector<Param> params;
};

struct Line{
   int index = -1;
   int address = 0;

   bool isInt = false;
   bool isFloat = false;
   bool isString = false;

   int intValue = 0;
   float floatValue = 0.0f;
   char stringValue[4];

   std::vector<int> referencedBy;
   int referencedLineIndex = 0;
   bool isPartOfReference = false;

   bool isData3 = false;
};

struct DataNode{
   enum class Type{
      Unknown,
      Root,
      Int,
      Float,
      String,
      Pointer
   };

   union{
      char s[4];
      int i;
      float f;
   } u;
   Type type = Type::Unknown;
   Line* line = nullptr;
   DataNode* parent = nullptr;
   std::vector<DataNode*> children;

   DataNode(){
      this->type = Type::Root;
   }

   DataNode(int newValue, DataNode* newParent){
      this->type = Type::Int;
      this->u.i = newValue;
      this->setParent(newParent);
   }

   DataNode(float newValue, DataNode* newParent){
      this->type = Type::Float;
      this->u.f = newValue;
      this->setParent(newParent);
   }

   DataNode(char* newValue, DataNode* newParent){
      this->type = Type::String;
      memcpy(this->u.s, newValue, 4);
      this->setParent(newParent);
   }

   void setParent(DataNode* newParent){
      if(newParent == this){
         throw;
      }

      auto& pc = newParent->children;
      bool alreadyHasThisChild = std::find(pc.begin(), pc.end(), this) != pc.end();
      if(alreadyHasThisChild){
         printf("%d already has child %d\n", newParent->line->address, this->line->address);
         return;
         //throw;
      }

      if(this->parent){
         auto& oldPC = this->parent->children;
         oldPC.erase(std::find(oldPC.begin(), oldPC.end(), this));
      }
      this->parent = newParent;
      this->parent->children.push_back(this);
   }

   void setParentMostDepth(DataNode* possibleNewParent, int data2Start, int data2End){
      if(this->parent){
         // Choose the parent with the most depth
         int oldParentDepth = this->parent->getDepth();
         int newParentDepth = possibleNewParent->getDepth();
         if(newParentDepth == oldParentDepth){
            // If they're the same depth, choose the parent that's a data2
            //int addr = this->parent->line->address;
            //bool isOldParentData2 = addr >= data2Start && addr <= data2End;
            int addr = possibleNewParent->line->address;
            bool isNewParentData2 = addr >= data2Start && addr <= data2End;
            if(isNewParentData2 == false){
               return;
            }
         }else if(newParentDepth < oldParentDepth){
            return;
         }
      }

      this->setParent(possibleNewParent);
   }

   int getDepth(){
      DataNode* current = this->parent;
      int depth = -1;
      while(current){
         current = current->parent;
         ++depth;
      }
      return depth;
   }
};

void outputFfxAnalysis(std::wstring path){
   std::vector<Line> lines; // Each 4 bytes is a line
   std::vector<Data2Entry> data2Entries;
   std::vector<Data3Entry> data3Entries;
   char sBuffer[200];

   auto createLine = [&](int address) -> Line&{
      //sprintf(sBuffer, "0x%05x %6d ", address, address);
      //lines.emplace_back(sBuffer);
      //return lines.back();

      lines.emplace_back();
      lines.back().index = lines.size() - 1;
      lines.back().address = address;
      return lines.back();
   };

   RawFfxFile rawFfx;
   loadRawFfxFile(&rawFfx, path);

   // "FXR" line
   Line& sigLine = createLine(0);
   sigLine.isString = true;
   memcpy(sigLine.stringValue, reinterpret_cast<char*>(&rawFfx.bytes[0]), 4);

   // Populate data lines with ints or floats
   {
      int* ip = reinterpret_cast<int*>(&rawFfx.bytes[0]);
      float* fp = reinterpret_cast<float*>(ip);
      int lineCount = rawFfx.header->data2Start / 4 + rawFfx.header->data2Count + rawFfx.header->data3Count;
      for(int n = 1; n < lineCount; ++n){
         Line& line = createLine(n * 4);

         //float floatAbsValue = fabs(fp[n]);
         //if(floatAbsValue > 0.001 && floatAbsValue < 100000.0f){
         //   sprintf(sBuffer, "float %f", fp[n]);
         //}else{
         //   sprintf(sBuffer, "int %d", ip[n]);
         //}
         //line += sBuffer;

         float floatAbsValue = fabs(fp[n]);
         if(floatAbsValue > 0.001 && floatAbsValue < 100000.0f || ip[n] == -2147483647 - 1){
            line.isFloat = true;
            line.floatValue = fp[n];
         }else{
            line.isInt = true;
            line.intValue = ip[n];
         }
      }
   }

   // Analyze possible references
   for(Line& line : lines){
      if(line.isInt && line.intValue > 4){
         auto findResult = std::find_if(
            lines.begin(),
            lines.end(),
            [&](Line& l) -> bool{return l.address == line.intValue;}
         );
         if(findResult != lines.end()){
            findResult->referencedBy.push_back(line.address);
            line.referencedLineIndex = findResult->index;
         }
      }
   }

   // Create Data2 info
   {
      int baseAddress = rawFfx.header->data2Start;
      for(int n = 0; n < rawFfx.header->data2Count; ++n){
         int address = baseAddress + n * 4;
         int data2Pointer = *reinterpret_cast<int*>(&rawFfx.bytes[address]);
         Data2Entry entry;
         entry.originalAddress = address;
         entry.pointerAddress = data2Pointer;
         data2Entries.push_back(entry);
      }
   }

   // Create Data3 info
   {
      int baseAddress = rawFfx.data3Start;
      for(int n = 0; n < rawFfx.header->data3Count; ++n){
         int address = baseAddress + n * 4;
         int data3Pointer = *reinterpret_cast<int*>(&rawFfx.bytes[address]);
         int* ip = reinterpret_cast<int*>(&rawFfx.bytes[data3Pointer]);
         float* fp = reinterpret_cast<float*>(ip);

         data3Entries.emplace_back();
         Data3Entry& entry = data3Entries.back();
         entry.originalAddress = address;
         entry.pointerAddress = data3Pointer;
         entry.type = *ip;
         ++ip;
         ++fp;

         int endAddress = lines.back().address;
         for(const Line& line : lines){
            if(line.address > data3Pointer && line.address < endAddress && line.referencedBy.size() > 0){
               endAddress = line.address;
            }
         }
         for(const Data2Entry& data2Entry : data2Entries){
            int data2Pointer = data2Entry.pointerAddress;
            if(data2Pointer > data3Pointer && data2Pointer < endAddress){
               endAddress = data2Pointer;
            }
         }

         int paramCount = (endAddress - data3Pointer) / 4 - 1;
         auto paramCheck = [&](int assumedParamCount) -> void{
            if(paramCount != assumedParamCount){
               wchar_t wBuffer[80];
               swprintf(
                  wBuffer,
                  sizeof(wBuffer),
                  L"Data3 type %d param count issue (assumed %d, actually %d)", entry.type, assumedParamCount, paramCount
               );
               ffxAssumptionWrong(path, wBuffer);
            }
         };
         if(entry.type == 7){
            paramCheck(1);
            entry.params.emplace_back(fp[0]);
         }else if(entry.type == 37){
            if(ip[0] == 0 && paramCount != 3){
               ffxAssumptionWrong(path, L"Data3 type 37 has no refFfxId but no mysterious trailing params");
            }
            for(int a = 0; a < paramCount; ++a){
               entry.params.emplace_back(ip[a]);
            }
         }else if(entry.type == 70){
            paramCheck(1);
            entry.params.emplace_back(fp[0]);
         }else if(entry.type == 81){
            paramCheck(2);
            entry.params.emplace_back(fp[0]);
            entry.params.emplace_back(fp[1]);
         }else if(entry.type == 136){
            paramCheck(0);
         }else for(int a = 0; a < paramCount; ++a){
            entry.params.emplace_back(ip[a]);
            entry.params.back().type = Param::Type::Unknown;
         }

         // Set "isData3" flag for each line
         int lineIndexStart = 0;
         for(; lineIndexStart < (int)lines.size(); ++lineIndexStart){
            if(lines[lineIndexStart].address == data3Pointer){
               break;
            }
         }
         for(int l = lineIndexStart; l <= lineIndexStart + paramCount; ++l){
            lines[l].isData3 = true;
         }
      }
   }

   // Reanalyze possible references. Yeah, I know, whatever.
   for(Line& line : lines){
      line.referencedBy.resize(0);
      line.referencedLineIndex = 0;
   }
   for(const Data2Entry& data2Entry : data2Entries){
      auto findResult = std::find_if(
         lines.begin(),
         lines.end(),
         [&](Line& l) -> bool{return l.address == data2Entry.pointerAddress;}
      );
      if(findResult != lines.end()){
         findResult->referencedBy.push_back(data2Entry.originalAddress);
         int pointerIndex = findResult->index;

         findResult = std::find_if(
            lines.begin(),
            lines.end(),
            [&](Line& l) -> bool{return l.address == data2Entry.originalAddress;}
         );
         findResult->referencedLineIndex = pointerIndex;
      }
   }
   for(const Data3Entry& data3Entry : data3Entries){
      auto findResult = std::find_if(
         lines.begin(),
         lines.end(),
         [&](Line& l) -> bool{return l.address == data3Entry.pointerAddress;}
      );
      if(findResult != lines.end()){
         findResult->referencedBy.push_back(data3Entry.originalAddress);
         int pointerIndex = findResult->index;

         findResult = std::find_if(
            lines.begin(),
            lines.end(),
            [&](Line& l) -> bool{return l.address == data3Entry.originalAddress;}
         );
         findResult->referencedLineIndex = pointerIndex;
      }
   }
   for(Line& line : lines){
      if((line.address < data2Entries[1].pointerAddress && line.address != 8) || line.address >= rawFfx.header->data2Start){
         continue;
      }

      if(line.isInt && line.intValue > 4 && line.isData3 == false){
         auto findResult = std::find_if(
            lines.begin(),
            lines.end(),
            [&](Line& l) -> bool{return l.address == line.intValue;}
         );
         if(findResult != lines.end()){
            // Don't reference the early data between ~32 and the second data2
            if(findResult->address >= rawFfx.data2[1] || line.address <= rawFfx.data2[1]){
               findResult->referencedBy.push_back(line.address);
               line.referencedLineIndex = findResult->index;
            }
         }
      }
   }

   // Write the output to disk

   auto outputValue = [&](std::string& outputText, const Line& line){
      if(line.isString){
         outputText += line.stringValue;
      }else if(line.isFloat){
         sprintf(sBuffer, "float %f", line.floatValue);
         outputText += sBuffer;
      }else if(line.isInt){
         if(line.intValue == 0){
            sprintf(sBuffer, "0");
         }else{
            sprintf(sBuffer, "int %d", line.intValue);
         }
         outputText += sBuffer;
      }else{
         throw;
      }

      if(line.isData3){
         outputText += " (data3)";
      }

      if(line.referencedLineIndex != 0){
         outputText += "*";
      }

      if(line.address == 264){
         int bp=42;
      }

      for(int address : line.referencedBy){
         sprintf(sBuffer, "  (<-- %d)", address);
         outputText += sBuffer;
      }
   };

   FILE* file = fopen("output.txt", "wb");

   for(const Line& line : lines){
      std::string outputText;

      sprintf(sBuffer, "0x%05x %6d ", line.address, line.address);
      outputText += sBuffer;
      int addressTextSize = outputText.size();

      outputValue(outputText, line);

      // Output nested values
      std::vector<int> currentlyProcessing;
      int maxLoopCount = 10;
      std::function<void(const Line&, int)> processLine;
      processLine = [&](const Line& l, int loopCount){
         if(l.isInt == false || l.referencedLineIndex == 0){
            return;
         }

         if(loopCount >= maxLoopCount){
            ffxResearchError(path, L"Recursion too deep");
         }

         if(std::find(currentlyProcessing.begin(), currentlyProcessing.end(), l.address) != currentlyProcessing.end()){
            ffxResearchError(path, L"Reference loop >_>");
         }
         currentlyProcessing.push_back(l.address);

         int firstLineIndex = l.referencedLineIndex;
         int lastLineIndex = firstLineIndex;
         while(
            lastLineIndex + 1 < (int)lines.size() &&
            lines[lastLineIndex + 1].referencedBy.size() == 0 &&
            lines[lastLineIndex + 1].address != rawFfx.header->data2Start &&
            lines[lastLineIndex + 1].address != rawFfx.data3Start
         ){
            ++lastLineIndex;
         }
         for(int lineIndex = firstLineIndex; lineIndex <= lastLineIndex; ++lineIndex){
            outputText += "\n";
            for(int n = 0; n < addressTextSize + (loopCount + 1) * 3; ++n){outputText += " ";}
            outputValue(outputText, lines[lineIndex]);
            lines[lineIndex].isPartOfReference = true;
            processLine(lines[lineIndex], loopCount + 1);
         }

         currentlyProcessing.erase(std::find(
            currentlyProcessing.begin(),
            currentlyProcessing.end(),
            l.address
         ));
      };
      processLine(line, 0);

      outputText += "\n";

      if(line.address + 4 == rawFfx.header->data2Start){
         outputText += "\n";
         for(int a = 0; a < 3; ++a){
            for(int b = 0; b < 100; ++b){outputText += "#";}
            outputText += "\n";
         }
         outputText += "\nData2 pointers\n\n";
      }
      if(line.address + 4 == rawFfx.data3Start){
         outputText += "\n";
         for(int a = 0; a < 3; ++a){
            for(int b = 0; b < 100; ++b){outputText += "#";}
            outputText += "\n";
         }
         outputText += "\nData3 pointers\n\n";
      }

      //fwrite(outputText.data(), 1, outputText.size(), file);
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////
   // Generate DataNode tree

   std::vector<std::pair<int, DataNode*>> lineIndexToDataNode;
   DataNode* root = new DataNode;
   //std::set<DataNode*> dataNodesProcessed;

   auto getPair = [&](int lineIndex) -> std::pair<int, DataNode*>&{
      auto findResult = std::find_if(
         lineIndexToDataNode.begin(),
         lineIndexToDataNode.end(),
         [&](std::pair<int, DataNode*> p) -> bool{return p.first == lineIndex;}
      );
      if(findResult != lineIndexToDataNode.end()){
         return *findResult;
      }else{
         throw;
      }
   };

   // Populate lineIndexToDataNode
   for(Line& line : lines){
      // Skip data2 and data3 pointers
      //if(line.address >= rawFfx.header->data2Start + rawFfx.header->data2Count * 4){
      //if(line.address >= rawFfx.header->data2Start){
      //   continue;
      //}

      DataNode* newNode;
      if(line.isString){
         newNode = new DataNode(line.stringValue, root);
      }else if(line.isInt){
         newNode = new DataNode(line.intValue, root);
      }else if(line.isFloat){
         newNode = new DataNode(line.floatValue, root);
      }
      newNode->line = &line;

      lineIndexToDataNode.emplace_back(line.index, newNode);
   }

   // Set up parents and children for all nodes
   std::set<DataNode*> dataNodesProcessing;
   std::function<void(DataNode*)> processNode;
   processNode = [&](DataNode* node) -> void{
      if(dataNodesProcessing.count(node) != 0){
         ffxResearchError(path, L"Node is already being processed");
      }
      dataNodesProcessing.insert(node);

      int refIndex = node->line->referencedLineIndex;
      if(refIndex != 0){
         int firstIndex = refIndex;
         int lastIndex = firstIndex;
         while(
            lastIndex + 1 < (int)lines.size() &&
            lines[lastIndex + 1].referencedBy.size() == 0 &&
            lines[lastIndex + 1].address != rawFfx.header->data2Start &&
            lines[lastIndex + 1].address != rawFfx.data3Start
         ){
            ++lastIndex;
         }
         for(int i = firstIndex; i <= lastIndex; ++i){
            DataNode* newChild = getPair(i).second;
            newChild->setParentMostDepth(
               node,
               rawFfx.header->data2Start,
               rawFfx.data3Start
            );
            processNode(newChild);
         }
      }

      dataNodesProcessing.erase(node);
   };
   std::vector<DataNode*> childrenCopy = root->children;
   for(DataNode* child : childrenCopy){
      if(child->line->address == 300){
         printf("%d\n", child->getDepth());
      }
      processNode(child);
   }

   std::string outputText;

   outputText += "\n\n";
   outputText += "################################################################################\n";
   outputText += "DataNode approach\n";
   outputText += "\n";

   // Output dataNode structure

   int addressTextSize = sprintf(sBuffer, "0x%05x %6d ", 0, 0);

   std::function<void(DataNode*, int)> outputNode;
   outputNode = [&](DataNode* node, int depth) -> void{
      if(node->line->address == rawFfx.header->data2Start){
         outputText += "\n------------------------------\nData2\n\n";
      }
      if(node->line->address == rawFfx.data3Start){
         outputText += "\n------------------------------\nData3\n\n";
      }

      if(depth == 0){
         sprintf(sBuffer, "0x%05x %6d ", node->line->address, node->line->address);
         outputText += sBuffer;
      }else{
         for(int s = 0; s < addressTextSize; ++s) outputText += ' ';
         for(int d = 0; d < depth; ++d){
            outputText += "   ";
         }
      }

      if(node->type == DataNode::Type::String){
         outputText += node->u.s;
      }else if(node->type == DataNode::Type::Int){
         if(node->u.i == 0){
            sprintf(sBuffer, "0");
         }else if(node->line->referencedLineIndex != 0){
            sprintf(sBuffer, "int %d*", node->u.i);
         }else{
            sprintf(sBuffer, "int %d", node->u.i);
         }
         outputText += sBuffer;
      }else if(node->type == DataNode::Type::Float){
         sprintf(sBuffer, "float %f", node->u.f);
         outputText += sBuffer;
      }

      if(node->getDepth() == 0 && node->line->referencedLineIndex == 0 && node->line->address > 8){
         sprintf(sBuffer, "  (Orphan?)");
         outputText += sBuffer;
      }

      for(int refLineAddress : node->line->referencedBy){
         auto findResult = std::find_if(
            lines.begin(),
            lines.end(),
            [&](Line& l) -> bool{return l.address == refLineAddress;}
         );
         int refLineIndex = findResult->index;
         DataNode* refNode = getPair(refLineIndex).second;
         if(refNode->line->referencedLineIndex != 0){
            if(refNode->line->address >= rawFfx.data3Start){
               outputText += "  (<-- data3)";
            }else if(refNode->line->address >= rawFfx.header->data2Start){
               outputText += "  (<-- data2)";
            }
         }
      }

      if(node->line->referencedBy.size() > 1){
         for(int refLineAddress : node->line->referencedBy){
            auto findResult = std::find_if(
               lines.begin(),
               lines.end(),
               [&](Line& l) -> bool{return l.address == refLineAddress;}
            );
            int refLineIndex = findResult->index;
            DataNode* refNode = getPair(refLineIndex).second;
            if(refNode->children.empty() && refNode->line->referencedLineIndex != 0){
               if(refNode->line->address < rawFfx.data3Start){
                  sprintf(sBuffer, "  (<-- Middle ref from %d)", refNode->line->address);
                  outputText += sBuffer;
               }
            }
         }
      }

      if(node->children.empty() && node->line->referencedLineIndex != 0){
         DataNode* current = getPair(node->line->referencedLineIndex).second;
         while(current->parent != root){
            current = current->parent;
         }
         if(node->u.i > rawFfx.data2[1]){
            sprintf(sBuffer, "  (--> REFERENCE TO MIDDLE OF %d)", current->u.i);
            outputText += sBuffer;

            outputText += '\n';
            DataNode* ref = getPair(node->line->referencedLineIndex).second;
            outputNode(ref, depth + 1);
         }
      }else{
         outputText += "\n";
      }

      for(DataNode* child : node->children){
         outputNode(child, depth + 1);
      }
   };
   for(DataNode* child : root->children){
      outputNode(child, 0);
   }

   fwrite(outputText.data(), 1, outputText.size(), file);

   fclose(file);
}

int main(int argCount, char** args) {
   printf("Hello\n");

   //RawFfxFile rawFfx;
   //std::wstring path = L"C:/Projects/Dark Souls/FFX research/";
   //path += L"f0000482.ffx";
   //loadRawFfxFile(&rawFfx, path);

   //testEveryFfx(L"C:/Program Files (x86)/Steam/steamapps/common/Dark Souls Prepare to Die Edition/DATA-BR/sfx/Dark Souls (PC)/data/Sfx/OutputData/Main/Effect_win32/");

   std::wstring path = L"C:/Program Files (x86)/Steam/steamapps/common/Dark Souls Prepare to Die Edition/DATA-BR/sfx/Dark Souls (PC)/data/Sfx/OutputData/Main/Effect_win32/";
   path += L"f0013520.ffx";
   //path += L"f0000482.ffx";
   //path += L"f0000480.ffx";
   //path += L"f0002101.ffx";
   outputFfxAnalysis(path);
}
