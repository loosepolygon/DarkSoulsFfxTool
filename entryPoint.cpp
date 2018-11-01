#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

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
};

void loadRawFfxFile(const std::wstring& path, RawFfxFile* rawFfx){
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

      loadRawFfxFile(dir + path, &rawFfx);

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
         }
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

int main(int argCount, char** args) {
   printf("Hello\n");

   //RawFfxFile rawFfx;
   //std::wstring path = L"C:/Projects/Dark Souls/FFX research/";
   //path += L"f0000482.ffx";
   //loadRawFfxFile(path, &rawFfx);

   testEveryFfx(L"C:/Program Files (x86)/Steam/steamapps/common/Dark Souls Prepare to Die Edition/DATA-BR/sfx/Dark Souls (PC)/data/Sfx/OutputData/Main/Effect_win32/");
}
