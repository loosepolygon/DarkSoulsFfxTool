#include "header.hpp"

bool hasErrored = false;
FILE* currentGlobalFile = nullptr;

void ffxReadError(const std::wstring& path, const std::wstring& text){
   std::wstring errorMessage = L"Error reading file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());

   if(hasErrored == false){
      hasErrored = true;
      if(currentGlobalFile){
         fclose(currentGlobalFile);
      }
      printf("Outputting ffx analysis...\n");
      outputFfxAnalysis(path);
      printf("Done.\n");
   }

   throw;
}

void ffxAssumptionWrong(const std::wstring& path, const std::wstring& text){
   std::wstring errorMessage = L"FFX assumption proved wrong for file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());
   throw;
}

void ffxResearchError(const std::wstring& path, const std::wstring& text){
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

struct AxisThingStruct{
   int address = 0;
   int pond1 = 0;
   int moose1 = 0;
   int moose2 = 0;
   int axisThing = 0;
   int pond2 = 0;
   int pond3 = 0;
};

struct RawAST{
   int pond1 = 0;
   int moose1 = 0;
   int moose2 = 0;
   int axisThing = 0;
   int pond2 = 0;
   int pond3 = 0;
};

struct Type133{
   int type;
   int ffxId;
   int unk1;
   int unk2;
   int unk3;
   int unk4;
   int unk5;
   int unk6;
   int unk7;
   int always8Or10;
   
   int pond1_1;
   int moose1_1;
   int moose2_1;
   int axisThing_1;
   int pond2_1;
   int pond3_1;

   int pond1_2;
   int moose1_2;
   int moose2_2;
   int axisThing_2;
   int pond2_2;
   int pond3_2;

   int house;
   int ostrich;
};

struct Type134{
   int type;
   int ffxId;
   int unk1;
   int type133s; // Pointer to array of pointers to type 133s
   int type133Count;
};

void testEveryFfx(std::wstring dir){
   std::vector<std::wstring> fileList;
   std::set<int> ffxIdSet;
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

         const wchar_t* filePath = &item[item.size() - 1];
         while(--filePath != item.data()){
            if(*filePath == L'/'){
               ++filePath;
               break;
            }
         }
         std::wstring idText = filePath + 1;
         idText.resize(idText.size() - 4);
         int id = _wtoi(idText.c_str());
         ffxIdSet.insert(id);
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
   std::vector<std::pair<int, int>> mooseStats;
   for(const std::wstring& path : fileList){
      //wprintf_s(L"File %d/%d\n", index, fileList.size());
      ++index;

      auto getIP = [&](int address) -> int*{
         return reinterpret_cast<int*>(&rawFfx.bytes[address]);
      };

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


      // Axis number (65536 etc)

      int axisThingCount = 0;
      // If nothing references the axisThing struct, the value is still 0
      std::vector<std::vector<int>> refStuff;
      std::vector<AxisThingStruct> axisThings;

      int ffxIdsToOutput[] = {482, 511, 13520, 2023, 120034};
      bool isFfxSelected = includes(ffxIdsToOutput, rawFfx.header->ffxId);
      //isFfxSelected = true;

      for(size_t b = 40; b < rawFfx.bytes.size(); b += 4){
         int* ip = getIP(b);
         if(ip[0] == 0x00010000 || ip[0] == 0x00010100 || ip[0] == 0x00010101 || ip[0] == 0x00010001){
            //if(ip[0] > 65000 && ip[0] < 70000){
            //int unk1 = ip[-4];
            int pond1 = ip[-3];
            int moose1 = ip[-2];
            int moose2 = ip[-1];
            int axisThing = ip[0];
            int pond2 = ip[1];
            int pond3 = ip[2];

            if(moose1 != moose2){
               ffxAssumptionWrong(path, L"axis thing, meese not equal");
            }

            int findResult = findIndex(mooseStats, [&](auto& pair) -> bool{return pair.first == moose1;});
            if(findResult == -1){
               findResult = mooseStats.size();
               mooseStats.emplace_back(moose1, 0);
            }
            ++mooseStats[findResult].second;

            //if(moose1 == 64){
            //   printf("%d\n", rawFfx.header->ffxId);
            //}

            // moose seems to be a type
            if(moose1 == 1){
               if(!(pond1 && !pond2 && !pond3)) ffxAssumptionWrong(path, L"moose type error");
               if(axisThing != 65536) ffxAssumptionWrong(path, L"moose doesn't match axisThing");
            }else if(moose1 == 2){
               if(!(pond1 && !pond2 && !pond3)) ffxAssumptionWrong(path, L"moose type error");
               if(axisThing != 65536) ffxAssumptionWrong(path, L"moose doesn't match axisThing");
            }else if(moose1 == 3){
               if(!(pond1 && !pond2 && !pond3)) ffxAssumptionWrong(path, L"moose type error");
               if(axisThing != 65536) ffxAssumptionWrong(path, L"moose doesn't match axisThing");
            }else if(moose1 == 4){
               if(!(pond1 && !pond2 && !pond3)) ffxAssumptionWrong(path, L"moose type error");
               if(axisThing != 65536) ffxAssumptionWrong(path, L"moose doesn't match axisThing");
            }else if(moose1 == 5){
               if(!(pond1 && !pond2 && !pond3)) ffxAssumptionWrong(path, L"moose type error");
               if(axisThing != 65536) ffxAssumptionWrong(path, L"moose doesn't match axisThing");
            }else if(moose1 == 6){
               if(!(pond1 && !pond2)) ffxAssumptionWrong(path, L"moose type error");
               if(axisThing != 65536) ffxAssumptionWrong(path, L"moose doesn't match axisThing");
            }else if(moose1 == 7){
               if(!(pond1 && !pond2 && !pond3)) ffxAssumptionWrong(path, L"moose type error");
               if(axisThing != 65536) ffxAssumptionWrong(path, L"moose doesn't match axisThing");
            }else if(moose1 == 8){
               if(!(pond1 && !pond2)) ffxAssumptionWrong(path, L"moose type error");
            }else if(moose1 == 9){
               if(!(pond1 && !pond2)) ffxAssumptionWrong(path, L"moose type error");
            }else if(moose1 == 11){
               if(!(pond1 && !pond2 && !pond3)) ffxAssumptionWrong(path, L"moose type error");
               if(axisThing != 65536) ffxAssumptionWrong(path, L"moose doesn't match axisThing");
            }else if(moose1 == 12){
               if(!(pond1 && !pond2 && !pond3)) ffxAssumptionWrong(path, L"moose type error");
               if(axisThing != 65536) ffxAssumptionWrong(path, L"moose doesn't match axisThing");
            }else if(moose1 == 13){
               if(!(pond1 && !pond2)) ffxAssumptionWrong(path, L"moose type error");
            }else if(moose1 == 15){
               if(!(pond1 && !pond2 && !pond3)) ffxAssumptionWrong(path, L"moose type error");
            }else if(moose1 == 16){
               if(axisThing != 65536) ffxAssumptionWrong(path, L"moose doesn't match axisThing");
               if(!(pond1 && !pond2 && pond3)) ffxAssumptionWrong(path, L"moose type error");
            }else if(moose1 == 18){
               if(!(pond1 && !pond2 && pond3)) ffxAssumptionWrong(path, L"moose type error");
            }else if(moose1 == 64){
               if(axisThing != 65536) ffxAssumptionWrong(path, L"moose doesn't match axisThing");
               if(!(pond1 && !pond2 && pond3)) ffxAssumptionWrong(path, L"moose type error");
            }

            //if(axisThing != 65536){
            //   //printf("%6d  %6d, %3d, %3d, %5d, %5d \n", b, pond1, moose1, moose2, axisThing, pond2);
            //   printf("weird axis thing ---------------- %d\n", moose1);
            //}
            if(axisThing != 65536){
               //printf("%6d  %6d, %3d, %3d, %5d, %5d \n", b, pond1, moose1, moose2, axisThing, pond2);
               //printf("weird axis thing ---------------- %d\n", moose1);
            }

            //if(axisThing != 65536){
            //   byte* atB = (byte*)(&axisThing);
            //   //printf("%d %d %d %d\n", atB[0], atB[1], atB[2], atB[3]);
            //   int at2 = axisThing - 65536;
            //   if(at2 != 1 && at2 != 256 && at2 != 257){
            //      printf("%d\n", axisThing - 65536);
            //   }
            //   //printf("%d %d %d %d\n", atB[0], atB[1], atB[2], atB[3]);
            //}

            bool isPond1Zero = pond1 == 0;
            bool isMooseZero = moose1 == 0;
            if(isPond1Zero != isMooseZero){
               ffxAssumptionWrong(path, L"pond1 and moose");
            }

            if(pond2 != 0){
               if(pond1 != 0 || pond3 != 0){
                  ffxAssumptionWrong(path, L"pond2");
               }
            }

            if(axisThing != 65536){
               if(pond1 == 0 || moose1 == 0 || pond2 != 0){
                  ffxAssumptionWrong(path, L"weird axisThing");
               }
            }

            if(pond1 != 0){
               if(pond1 - b <= 0){
                  ffxAssumptionWrong(path, L"pond1 wrong direction");
               }
            }
            if(pond2 != 0){
               if(pond2 - b <= 0){
                  ffxAssumptionWrong(path, L"pond2 wrong direction");
               }
            }
            if(pond3 != 0){
               if(pond3 - b <= 0){
                  ffxAssumptionWrong(path, L"pond3 wrong direction");
               }
            }

            if(pond3 != 0){
               if(pond1 == 0){
                  ffxAssumptionWrong(path, L"pond3");
               }
            }


            if(pond3){
               //printf("%d\n", pond3 - b);
            }
            if(pond3 && pond3 - b < 0){
               //printf("%6d  %6d, %5d, %6d, %6d\n", pond1, moose1, axisThing, pond2, pond3);
            }

            if(isFfxSelected){
               // It seems to be that there's always something that references the struct (at pond1)
               // EXCEPT for the first ones AND the final one. Strange.

               // Returns the addresses that reference the input address
               auto getRefSources = [&](int address) -> std::vector<int>{
                  std::vector<int> output;

                  for(size_t b = 40; b < rawFfx.bytes.size(); b += 4){
                     int possiblePointer = getIP(b)[0];
                     if(possiblePointer == address){
                        output.push_back(b);
                     }
                  }

                  return output;
               };

               std::vector<int> refSources = getRefSources(b - 3 * 4);
               bool hasRefToThisStruct = !refSources.empty();

               ++axisThingCount;
               refStuff.push_back(refSources);

               AxisThingStruct ast;
               ast.address = b - 3 * 4;
               ast.pond1 = pond1;
               ast.moose1 = moose1;
               ast.moose2 = moose2;
               ast.axisThing = axisThing;
               ast.pond2 = pond2;
               ast.pond3 = pond3;
               axisThings.push_back(ast);
            }

            // pond2 analysis
            //if(pond2){
            //   ip = getIP(pond2);
            //   int someCount = ip[0];
            //   int relativeOffset = ip[1];
            //   int unk1 = ip[2];

            //   ip = getIP(pond2 + relativeOffset);

            //   printf("%6d, %6d, %6d\n", ip[0], ip[1], ip[2]);
            //}
         }
      }


      //////////////////////////////////////////////////////////////////////////////////////////////
      // Data3 analysis

      std::vector<std::wstring> ffxIdsToTryToLoad;

      bool has133 = false;
      bool has134 = false;
      std::set<int> axisThingsPointedTo;
      for(int n = 0; n < rawFfx.header->data3Count; ++n){
         int pointer = rawFfx.data3[n];
         int* p = getIP(rawFfx.data3[n]);
         int type = *p;
         ++p;
         if(type == 7){
            float value = *reinterpret_cast<float*>(p);
            //printf("%f\n", value);
         }else if(type == 37){
            int refFfxId = p[0];
            int axisThingPointer = p[1];
            int zero = p[2];

            if(refFfxId == 0){
               if(axisThingPointer != 0){
                  ffxAssumptionWrong(path, L"Type 37");
               }
            }else{
               if(axisThingPointer){
                  axisThingsPointedTo.insert(axisThingPointer);
                  //printf("%d:  %d\n", refFfxId, axisThingPointer);
               }

               if(zero != 0){
                  ffxAssumptionWrong(path, L"Type 37");
               }
            }
         }else if(type == 38){
            int someCount = p[0];
            int axisThingPointer = p[1];
            int zero = p[2];

            //printf("%7d %5d, %d\n", someCount, axisThingPointer, zero);

            if(axisThingPointer == 0){
               ffxAssumptionWrong(path, L"Type 38");
            }

            if(zero != 0){
               ffxAssumptionWrong(path, L"Type 38");
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

            --p;
            Type133 type133;
            memcpy(&type133, p, sizeof(Type133));

            //printf("%d\n", type133.ostrich);
            int ostrich = type133.ostrich;
            if(ostrich != 1 && ostrich != 2 && ostrich != 3 && ostrich != 4 && ostrich != 5 && ostrich != 6 && ostrich != 7){
               ffxAssumptionWrong(path, L"ostrich is unfamiliar");
            }

            if(type133.pond1_1 != 0){
               //printf("%d, %d\n", type133.pond1_1, type133.pond3_2);
               //printf("%d\n", type133.always8Or10);
            }

            if(type133.unk1 != 0 || type133.unk2 != 0 || type133.unk3 != 0 || type133.unk4 != 0 || type133.unk5 != 0 || type133.unk6 != 0 || type133.unk7 != 0){
               ffxAssumptionWrong(path, L"early part of type 133");
            }

            if(has134 == false && type133.ostrich == 2 && !type133.pond1_1 && !type133.pond1_2){
               ffxIdsToTryToLoad.push_back(path);
            }

            // Important stuff
            if(false && has134 == false && type133.ostrich == 2 && !type133.pond1_1 && !type133.pond1_2){
               int* ip = getIP(type133.house);
               int crepe1 = ip[0];
               int crepe2 = ip[1];
               int tulip1 = ip[2];
               int tulip2 = ip[3];
               int monkeyTail2 = ip[5];

               //if(monkeyTail2 + 4 != axisThings.back().address){
               //   int bp=42;
               //}

               {
                  ip = getIP(crepe2);

                  //if(crepe1 + 8 != crepe2){
                  //   printf("%d\n", crepe1 + 8 - crepe2);
                  //}
               }

               {
                  ip = getIP(crepe2);
                  int blossom = ip[0];
                  RawAST ast = *reinterpret_cast<RawAST*>(ip + 1);

                  //if(ast.pond1){
                  //   if(ast.moose1 != 1 && ast.moose1 != 8 && ast.moose1 != 16){
                  //      printf("(%5d, %5d, %5d), %5d, %5d, %5d\n", ast.pond1, ast.moose1, ast.moose2, ast.axisThing, ast.pond2, ast.pond3);
                  //   }

                  //   ip = getIP(ast.pond1);
                  //   int spider = ip[0];

                  //   ip = getIP(spider);

                  //   printf("%d, refFfx = %07d, %5d, %5d\n", ip[0], ip[1], ip[2], ip[3]);

                  //   if(ip[2] != 0 && ip[2] != 128){
                  //      int bp=42;
                  //   }

                  //}

                  if(ast.pond3){
                     ip = getIP(ast.pond3);
                     int pig1 = ip[0];
                     int pig2 = ip[1];


                     if(pig2 != 0 && pig2 != 1 && pig2 != 2 && pig2 != 3 && pig2 != 4 && pig2 != 5 && pig2 != 6 && pig2 != 7 && pig2 != 8 && pig2 != 9 && pig2 != 10 && pig2 != 11 && pig2 != 13){
                        printf("%d, %d\n", pig1, pig2);
                     }

                     //printf("%d, %d\n", ostrich, pig2);
                  }

                  if(ast.axisThing != 65536){
                     printf("(%5d, %5d, %5d), %5d, %5d, %5d\n", ast.pond1, ast.moose1, ast.moose2, ast.axisThing, ast.pond2, ast.pond3);
                  }

                  //if(monkeyTail2 + 4 != crepe2 + 8 * 4){
                  //   //printf("%d\n", ip[7]);
                  //   printf("ffx %d: %d\n", rawFfx.header->ffxId, monkeyTail2 + 4 - crepe2 + 8 * 4);
                  //}

                  

                  //int findResult = findIndex(axisThings, [&](auto& ast) -> bool{return ast.address == crepe2 + 4;});
                  //if(findResult != -1){
                  //   if(findResult != axisThings.size() - 2){
                  //      printf("ffx %d %d / %d,  addr %5d\n", rawFfx.header->ffxId, findResult + 1, axisThings.size(), axisThings[findResult].address);

                  //      ast = *reinterpret_cast<RawAST*>(getIP(monkeyTail2 + 4));
                  //      printf("from monkeyTail2:   %5d (%5d, %5d, %5d), %5d, %5d, %5d\n", monkeyTail2 + 4, ast.pond1, ast.moose1, ast.moose2, ast.axisThing, ast.pond2, ast.pond3);

                  //      AxisThingStruct aLast = axisThings[axisThings.size() - 2];
                  //      printf("2nd last axisThing: %5d (%5d, %5d, %5d), %5d, %5d, %5d\n", aLast.address, aLast.pond1, aLast.moose1, aLast.moose2, aLast.axisThing, aLast.pond2, aLast.pond3);
                  //      aLast = axisThings.back();
                  //      printf("last axisThing:     %5d (%5d, %5d, %5d), %5d, %5d, %5d\n\n", aLast.address, aLast.pond1, aLast.moose1, aLast.moose2, aLast.axisThing, aLast.pond2, aLast.pond3);

                  //      int bp=42;
                  //   }
                  //}else{
                  //   int bp=42;
                  //}
               }
            }
         }else if(type == 134){
            has134 = true;

            --p;
            Type134 type134;
            memcpy(&type134, p, sizeof(Type134));

            if(type134.type133Count == 0){
               ffxAssumptionWrong(path, L"type134 count is 0");
            }

            //printf("ostriches: [");
            int* type133Pointers = getIP(type134.type133s);
            for(int n = 0; n < type134.type133Count; ++n){
               Type133 type133 = *reinterpret_cast<Type133*>(getIP(type133Pointers[0]));
               //printf("%d\n", type133.ffxId);
               if(type133.ffxId != type134.ffxId){
                  ffxAssumptionWrong(path, L"type134 ffxId mismatch");
               }

               //printf("%d", type133.ostrich);
               //if(n != type134.type133Count - 1){
               //   printf(", ");
               //}

               // always 2
            }
            //printf("]\n");
         }
      }
      if(has133 == false){
         ffxAssumptionWrong(path, L"Type 133 missing");
      }

      //printf("axisThingCount = %4d, from type 37/38 = %4d\n", axisThingCount, axisThingsPointedTo.size());

      for(std::wstring ffxPath : ffxIdsToTryToLoad){
         Ffx ffx;
         loadFfxFile(ffx, ffxPath);
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
      }while(false); // wtf?


      //////////////////////////////////////////////////////////////////////////
      // Test similar data found near the start of 13520 and 482

      if(rawFfx.header->ffxId == 13520){
         int bp=42;
      }

      int* ip = getIP(rawFfx.data2[1]);
      int house = ip[0];
      int ostrich = ip[1];

      if(ostrich != 1 && ostrich != 2 && ostrich != 3 && ostrich != 4 && ostrich != 5 && ostrich != 7){
         ffxAssumptionWrong(path, L"ostrich is unfamiliar");
      }

      ip = getIP(house);
      int crepe1 = ip[0];
      int crepe2 = ip[1];
      int tulip1 = ip[2];
      int tulip2 = ip[3];

      if(crepe1 == 0){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);
         if(crepe2 < 16 || tulip1 != 0 || tulip2 != 1){
            ffxAssumptionWrong(path, L"?");
         }
      }else if(crepe1 < 16){
         ffxAssumptionWrong(path, L"?");
      }

      if(crepe2 == 0){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);
         if(
            crepe1 < 16 ||
            (tulip1 != 1 && tulip1 != 2) ||
            tulip2 != 0
         ){
            ffxAssumptionWrong(path, L"?");
         }
      }else if(crepe2 < 16){
         ffxAssumptionWrong(path, L"?");
      }

      if(tulip1 == 0){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);
         if(
            crepe1 != 0 ||
            crepe2 < 16 ||
            tulip2 != 1
            ){
            ffxAssumptionWrong(path, L"?");
         }
      }else if(tulip1 == 1){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);
         if(
            crepe1 < 16 ||
            (tulip2 != 0 && tulip2 != 1 && tulip2 != 2)
            ){
            ffxAssumptionWrong(path, L"?");
         }
      }else if(tulip1 == 2){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);
         if(tulip2 != 0 && tulip2 != 1){
            ffxAssumptionWrong(path, L"?");
         }
      }else if(tulip1 > 16){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);
         if(crepe1 < 16 || crepe2 < 16 || tulip2 < 16){
            ffxAssumptionWrong(path, L"?");
         }
      }

      if(tulip2 == 0){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);
         if(crepe1 < 16 || crepe2 != 0 || (tulip1 != 1 && tulip1 != 2)){
            ffxAssumptionWrong(path, L"?");
         }
      }else if(tulip2 == 1){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);
         if(
            crepe2 < 16 ||
            (tulip1 != 0 && tulip1 != 1 && tulip1 != 2 && tulip1 != 5) // Uhhhh
         ){
            ffxAssumptionWrong(path, L"?");
         }
      }else if(tulip2 == 2){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);
         if(
            crepe1 < 16 ||
            crepe2 < 16 ||
            tulip1 != 1
          ){
            ffxAssumptionWrong(path, L"?");
         }
      }else if(tulip2 > 16){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);
         if(
            crepe1 < 16 ||
            crepe2 < 16 ||
            tulip1 < 16
          ){
            ffxAssumptionWrong(path, L"?");
         }
      }


      if(ostrich == 1){
         bool hasSomeOffset = tulip1 != 0;
         if(hasSomeOffset){
            //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);
            if(tulip1 < 16 || tulip2 < 16){
               ffxAssumptionWrong(path, L"Ostrich 1, with offset");
            }
         }else{
            if(tulip2 != 1){
               ffxAssumptionWrong(path, L"Ostrich 1, no offset");
            }
         }

         if(crepe1 - house > 0){
            ffxAssumptionWrong(path, L"Ostrich 1");
         }
      }else if(ostrich == 2){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);

         if(tulip1 != 1 || (tulip2 != 1 && tulip2 != 2)){
            ffxAssumptionWrong(path, L"Ostrich 2");
         }

         if(crepe1 - house != 32){
            ffxAssumptionWrong(path, L"Ostrich 2");
         }
      }else if(ostrich == 3){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);

         // Usually very negative but two instances of 48 ????????
         //printf("crepe1 - house == %d\n", crepe1 - house);
         //printf("house = %d, crepe1 = %d\n", house, crepe1);

         //printf("crepe1 - house == %d, crepe1 = %d\n", crepe1 - house, crepe1);

      }else if(ostrich == 4){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);

         // Negative (once) or 64
         //printf("crepe1 - house == %d\n", crepe1 - house);
      }else if(ostrich == 5){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);

         // 80
         //printf("crepe1 - house == %d\n", crepe1 - house);
      }else if(ostrich == 7){
         //printf("(%d, %d), (%d, %d)\n", crepe1, crepe2, tulip1, tulip2);
         if(tulip1 != 5){
            ffxAssumptionWrong(path, L"Ostrich 2");
         }

         // 112
         //printf("crepe1 - house == %d\n", crepe1 - house);
      }


      if(crepe1 != 0){
         ip = getIP(crepe1);
         int monkey = ip[0];

         bool isMonkeyValidAddress = monkey > 16 && monkey % 4 == 0;
         
         if(ostrich == 1){
            if(monkey != 1 && monkey != 37){
               ffxAssumptionWrong(path, L"crepe1 data");
            }
         }else if(ostrich == 2){
            // Offset
            //printf("%d\n", monkey);
            if(isMonkeyValidAddress == false){
               ffxAssumptionWrong(path, L"crepe1 data");
            }
         }else if(ostrich == 3){
            // 1, 352, 328, but mostly 133 (??????)
         }else if(ostrich == 4){
            // 1 or an offset
         }else if(ostrich == 5){
            // Offset
         }else if(ostrich == 7){
            // Offset (same as 5)
         }

         if(isMonkeyValidAddress){
            if(tulip1 > 16 || tulip2 > 16){
               ffxAssumptionWrong(path, L"monkey");
            }
         }else{
            //printf("%d  -  %d, %d\n", monkey, tulip1, tulip2);
            if(tulip1 < 16 || tulip2 < 16){
               ffxAssumptionWrong(path, L"monkey");
            }
         }
      }


      // ostrich 2 specific testing

      if(ostrich == 2){
         // Always has crepe1 and crepe2 offsets
         // tulip1 is 1 and tulip2 is 1, sometimes 2
         // crepe1 is always 32 bytes after house
         // monkeyTail is always 16 bytes before monkey

         if(tulip1 == 1 && tulip2 == 1){
            // crepe1, monkey

            ip = getIP(crepe1);
            int monkey = ip[0];
            int turtle = ip[1];


            if(monkey - crepe1 != -16){
               ffxAssumptionWrong(path, L"monkey - crepe1");
            }

            ip = getIP(monkey);
            int monkeyTail1 = ip[0];
            int monkeyTail2 = ip[1];
            int monkeyTail3 = ip[2];
            int monkeyTail4 = ip[3];

            if(monkeyTail1 != 0){
               ffxAssumptionWrong(path, L"monkeyTail1");
            }
            if(monkeyTail2 < 16){
               ffxAssumptionWrong(path, L"monkeyTail2");
            }
            if(monkeyTail2 - monkey != 52){
               ffxAssumptionWrong(path, L"monkeyTail2 - monkey");
            }
            if(monkeyTail3 != 0){
               ffxAssumptionWrong(path, L"monkeyTail3");
            }
            if(monkeyTail4 != 1){
               ffxAssumptionWrong(path, L"monkeyTail4");
            }

            ip = getIP(monkeyTail2);
            int hair1 = ip[0];

            if(
               hair1 != 5 ||
               ip[1] != 0 ||
               ip[2] != 0 ||
               ip[3] != 0 ||
               ip[4] != 65536 ||
               ip[5] != 0
            ){
               ffxAssumptionWrong(path, L"hair");
            }

            // turtle

            if(turtle < 16){
               ffxAssumptionWrong(path, L"turtle");
            }

            ip = getIP(turtle);
            int turtleEgg = ip[0];

            if(turtleEgg != 136){
               ffxAssumptionWrong(path, L"turtleEgg");
            }

            // crepe2, blossom

            ip = getIP(crepe2);
            int blossom1 = ip[0];
            int blossom2 = ip[1];
            int moose1 = ip[2];
            int moose2 = ip[3];
            int dirt1 = ip[4];
            int dirt2 = ip[5];
            int ghast = ip[6];

            if(blossom1 != 14 && blossom1 != 17 && blossom1 != 79 && blossom1 != 87){
               ffxAssumptionWrong(path, L"blossom1");
            }
            if(blossom2 != 0 && blossom2 < 16){
               ffxAssumptionWrong(path, L"blossom2");
            }
            if(blossom2 != 0){
               ip = getIP(blossom2);

               ip = getIP(ip[0]);
               int type = ip[0];
               int sfxId = ip[1];
               if(type != 37 || (sfxId != 0 && ffxIdSet.count(sfxId) == 0)){
                  //ffxAssumptionWrong(path, L"type 37");
                  //printf("%d not found\n", sfxId);
               }
            }


            if(moose1 != moose2){
               ffxAssumptionWrong(path, L"meese differ");
            }
            if(moose1 != 0 && moose1 != 1 && moose1 != 8 && moose1 != 16){
               ffxAssumptionWrong(path, L"meese unfamiliar");
            }

            if(dirt1 != 65536 || dirt2 != 0){
               ffxAssumptionWrong(path, L"dirts unfamiliar");
            }

            if(ghast != 0 && (ghast < 16 || ghast % 4 != 0)){
               ffxAssumptionWrong(path, L"ghast");
            }
            if(ghast != 0){
               ip = getIP(ghast);
               int gas1 = ip[0];
               int gas2 = ip[1];

               if(gas1 != 0){
                  ffxAssumptionWrong(path, L"gas1");
               }
               if(gas2 < 0 || gas2 > 13){
                  ffxAssumptionWrong(path, L"gas2");
               }
               if(gas2 != 0 && gas2 != 1 && gas2 != 2 && gas2 != 3 && gas2 != 4 && gas2 != 5 && gas2 != 6 && gas2 != 7 && gas2 != 8 && gas2 != 9 && gas2 != 10 && gas2 != 11 && gas2 != 13){
                  ffxAssumptionWrong(path, L"gas2");
               }
            }


         }
      }

      // Start address vs ostrich testing

      int secondData2 = rawFfx.data2[1];
      if(ostrich == 1 && secondData2 != 96){
         //ffxAssumptionWrong(path, L"secondData2 ostrich 1");
         //printf("%d, %d, %d\n", ostrich, secondData2, rawFfx.header->ffxId);
      }else if(ostrich == 2 && secondData2 != 120){
         ffxAssumptionWrong(path, L"secondData2 ostrich 2");
      }else{
         //printf("%d, %d\n", ostrich, secondData2);
      }


      // Address distances are interesting
      if(isFfxSelected && false){
         printf("ffx %7d\n", rawFfx.header->ffxId);
         for(int r = 0; r < axisThingCount - 1; ++r){
            AxisThingStruct currAST = axisThings[r];
            AxisThingStruct nextAST = axisThings[r + 1];

            int distance = nextAST.address - currAST.address;
            printf("  %5d  moose %2d dist = %d\n", currAST.address, currAST.moose1, distance);
         }
         printf("  %5d  moose %2d\n", axisThings.back().address, axisThings.back().moose1);

         printf("\n");
      }

      if(isFfxSelected && false){
         printf("refStuff for ffx %d: [\n", rawFfx.header->ffxId);
         int lastRef = 0;
         for(int r = 0; r < axisThingCount; ++r){
            if(r > 0) printf("\n");
            std::vector<int>& currentRefs = refStuff[r];
            AxisThingStruct ast = axisThings[r];

            // Choose from currentRefs the one that fits best
            int currentRef = 0;
            for(int ref : currentRefs){
               if((ref > lastRef && ref < currentRef) || currentRef == 0){
                  currentRef = ref;
               }
            }

            printf(" %5d -> %5d (%2d %5d, %5d, %5d)", currentRef, ast.address, ast.moose1, ast.pond1, ast.pond2, ast.pond3);

            if(currentRef != 0 && currentRef < lastRef){
               //ffxAssumptionWrong(path, L"axisThing ref doesn't make sense");
            }

            if(currentRef != 0){
               lastRef = currentRef;
            }
         }
         printf("\n]\n\n");
      }

      
      // Test "pre-data", but more experienced this time

      int* firstData3 = getIP(rawFfx.data3[0]);
      int* secondData3 = getIP(rawFfx.data3[1]);
      int* commonStart = nullptr;
      if(firstData3[0] == 133){
         if(rawFfx.data2[1] == 120){
         }else if(rawFfx.data2[2] == 120){
         }else{
            ffxAssumptionWrong(path, L"pre-data stuff");
         }

         commonStart = getIP(120);
      }else if(firstData3[0] == 134){
         if(secondData3[0] != 133){
            ffxAssumptionWrong(path, L"data3 after 134 is not 133");
         }

         if(rawFfx.data2[2] == 140){
         }else if(rawFfx.data2[3] == 140){
         }else{
            ffxAssumptionWrong(path, L"pre-data stuff");
         }

         commonStart = getIP(140);
      }else{
         ffxAssumptionWrong(path, L"first data3 unfamiliar");
      }

      // Then try to read more stuff based on more experience

      house = commonStart[0];
      ostrich = commonStart[1];

      if(ostrich != 1 && ostrich != 2 && ostrich != 3 && ostrich != 4 && ostrich != 5 && ostrich != 6 && ostrich != 7){
         ffxAssumptionWrong(path, L"ostrich is unfamiliar");
      }
   }

   //wprintf_s(L"minSecondData2Pointer = %d '%s'\n", minSecondData2Pointer, minSecondData2PointerPath.c_str());
   //wprintf_s(L"maxSecondData2Pointer = %d '%s'\n", maxSecondData2Pointer, maxSecondData2PointerPath.c_str());

   //printf("\n-- mooseStats\n");
   //std::sort(mooseStats.begin(), mooseStats.end(), [](auto a, auto b) -> bool{return a.first < b.first;});
   //for(auto pair : mooseStats){
   //   printf("moose %3d, count = %d\n", pair.first, pair.second);
   //}
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

// Struct analysis

struct SAValue{
   enum class Type{
      Unknown,
      Int,
      Float,
      String,
      Pointer
   };

   //union{
   //   char s[4];
   //   int i;
   //   float f;
   //} u;
   Type type = Type::Unknown;
   Line* line;

   SAValue(Line* line) : line(line){
      if(line->isString) this->type = Type::String;
      if(line->isFloat) this->type = Type::Float;

      if(line->isInt){
         if(line->referencedLineIndex != 0){
            this->type = Type::Pointer;
         }else{
            this->type = Type::Int;
         }
      }
   }
};

struct SAStruct{
   std::vector<SAValue> values;

   bool compare(SAStruct* other){
      if(this->values.size() != other->values.size()) return false;

      for(size_t n = 0; n < this->values.size(); ++n){
         const SAValue& v1 = this->values[n];
         const SAValue& v2 = other->values[n];

         if(v1.type != v2.type) return false;
      }

      return true;
   }
};

void outputFfxAnalysis(std::wstring fileName){
   std::wstring path;
   if(fileName[1] == L':'){
      path = fileName;
   }else{
      path += L"C:/Program Files (x86)/Steam/steamapps/common/Dark Souls Prepare to Die Edition/DATA-BR/sfx/Dark Souls (PC)/data/Sfx/OutputData/Main/Effect_win32/";
      path += fileName;
   }

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
            //if(paramCount != assumedParamCount){
            //   wchar_t wBuffer[80];
            //   swprintf(
            //      wBuffer,
            //      sizeof(wBuffer),
            //      L"Data3 type %d param count issue (assumed %d, actually %d)", entry.type, assumedParamCount, paramCount
            //   );
            //   ffxAssumptionWrong(path, wBuffer);
            //}
         };
         if(entry.type == 7){
            paramCheck(1);
            entry.params.emplace_back(fp[0]);
         }else if(entry.type == 37){
            //if(ip[0] == 0 && paramCount != 3){
            //   ffxAssumptionWrong(path, L"Data3 type 37 has no refFfxId but no mysterious trailing params");
            //}
            //for(int a = 0; a < paramCount; ++a){
            //   entry.params.emplace_back(ip[a]);
            //}

            entry.params.emplace_back(ip[0]);
            entry.params.emplace_back(ip[1]);
            entry.params.emplace_back(ip[2]);
            entry.params.emplace_back(ip[3]);
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
   //for(Line& line : lines){
   //   if((line.address < data2Entries[1].pointerAddress && line.address != 8) || line.address >= rawFfx.header->data2Start){
   //      continue;
   //   }

   //   if(line.isInt && line.intValue > 4 && line.isData3 == false){
   //      auto findResult = std::find_if(
   //         lines.begin(),
   //         lines.end(),
   //         [&](Line& l) -> bool{return l.address == line.intValue;}
   //      );
   //      if(findResult != lines.end()){
   //         // Don't reference the early data between ~32 and the second data2
   //         if(findResult->address >= rawFfx.data2[1] || line.address <= rawFfx.data2[1]){
   //            findResult->referencedBy.push_back(line.address);
   //            line.referencedLineIndex = findResult->index;
   //         }
   //      }
   //   }
   //}
   for(int n = 0; n < rawFfx.header->data2Count; ++n){
      int findResult = findIndex(lines, [&](auto& l) -> bool{return l.address == rawFfx.data2[n];});
      if(findResult == -1) ffxAssumptionWrong(path, L"data2 as all pointers");
      Line& pointerLine = lines[findResult];
      if(!pointerLine.isInt || pointerLine.intValue == 0) ffxAssumptionWrong(path, L"data2 as all pointers");


      findResult = findIndex(lines, [&](auto& l) -> bool{return l.address == pointerLine.intValue;});
      if(findResult == -1) ffxAssumptionWrong(path, L"data2 as all pointers");
      Line& destLine = lines[findResult];

      pointerLine.referencedLineIndex = destLine.index;
      destLine.referencedBy.push_back(pointerLine.address);
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

      for(int address : line.referencedBy){
         if(address >= rawFfx.data3Start){
            int data3Index = (address - rawFfx.data3Start) / 4;
            sprintf(sBuffer, "  (<-- data3[%d])", data3Index);
            outputText += sBuffer;
         }else if(address < rawFfx.header->data2Start){
            sprintf(sBuffer, "  (<-- %d)", address);
            outputText += sBuffer;
         }
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
            //ffxResearchError(path, L"Reference loop >_>");
            printf("Reference loop >_>\n");
            outputText += "Reference loop >_>";
            return;
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
         for(int a = 0; a < 6; ++a){
            for(int b = 0; b < 120; ++b){outputText += "#";}
            outputText += "\n";
         }
         outputText += "\nData2 pointers\n\n";
      }
      if(line.address + 4 == rawFfx.data3Start){
         outputText += "\n";
         for(int a = 0; a < 6; ++a){
            for(int b = 0; b < 120; ++b){outputText += "#";}
            outputText += "\n";
         }
         outputText += "\nData3 pointers\n\n";
      }

      fwrite(outputText.data(), 1, outputText.size(), file);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////
   //// Struct analysis

   //{
   //   // Struct, count
   //   std::vector<std::pair<SAStruct, int>> structs;
   //   SAStruct currentStruct;
   //   for(Line& line : lines){
   //      if(line.referencedBy.empty() || currentStruct.values.empty()){
   //         currentStruct.values.emplace_back(&line);
   //      }else{
   //         if(currentStruct.values.size() > 0){
   //            bool alreadyExists = false;
   //            for(auto& pair : structs){
   //               if(currentStruct.compare(&pair.first)){
   //                  alreadyExists = true;

   //                  ++pair.second;

   //                  break;
   //               }
   //            }
   //            if(alreadyExists == false){
   //               structs.emplace_back(currentStruct, 1);
   //            }

   //            currentStruct = SAStruct();
   //            currentStruct.values.emplace_back(&line);
   //         }else{
   //            printf("Empty struct (address %d)\n", line.address);
   //         }
   //      }
   //   }

   //   std::string outputText;

   //   outputText += "\n\n\n\n\n\n\n\n\n\n\n";
   //   outputText += "################################################################################\n";
   //   outputText += "Struct analysis approach, straight\n";
   //   outputText += "\n";

   //   sprintf(sBuffer, "%d unique structs found\n\n", structs.size());
   //   outputText += sBuffer;

   //   // Sort by value count?

   //   int index = 0;
   //   for(auto& pair : structs){
   //      sprintf(sBuffer, "Struct #%d, count %d  (first address = %d)\n", index++, pair.second, pair.first.values[0].line->address);
   //      outputText += sBuffer;

   //      for(SAValue& value : pair.first.values){
   //         outputText += "   ";

   //         if(value.type == SAValue::Type::String){
   //            outputText += "string";
   //         }else if(value.type == SAValue::Type::Int){
   //            outputText += "int";
   //         }else if(value.type == SAValue::Type::Float){
   //            outputText += "float";
   //         }else if(value.type == SAValue::Type::Pointer){
   //            outputText += "Pointer";
   //         }

   //         outputText += "\n";
   //      }
   //   }

   //   fwrite(outputText.data(), 1, outputText.size(), file);
   //}


   ///////////////////////////////////////////////////////////////////////////////////////////////////
   //// Generate DataNode tree

   //std::vector<std::pair<int, DataNode*>> lineIndexToDataNode;
   //DataNode* root = new DataNode;
   ////std::set<DataNode*> dataNodesProcessed;

   //auto getPair = [&](int lineIndex) -> std::pair<int, DataNode*>&{
   //   auto findResult = std::find_if(
   //      lineIndexToDataNode.begin(),
   //      lineIndexToDataNode.end(),
   //      [&](std::pair<int, DataNode*> p) -> bool{return p.first == lineIndex;}
   //   );
   //   if(findResult != lineIndexToDataNode.end()){
   //      return *findResult;
   //   }else{
   //      throw;
   //   }
   //};

   //// Populate lineIndexToDataNode
   //for(Line& line : lines){
   //   // Skip data2 and data3 pointers
   //   //if(line.address >= rawFfx.header->data2Start + rawFfx.header->data2Count * 4){
   //   //if(line.address >= rawFfx.header->data2Start){
   //   //   continue;
   //   //}

   //   DataNode* newNode;
   //   if(line.isString){
   //      newNode = new DataNode(line.stringValue, root);
   //   }else if(line.isInt){
   //      newNode = new DataNode(line.intValue, root);
   //   }else if(line.isFloat){
   //      newNode = new DataNode(line.floatValue, root);
   //   }
   //   newNode->line = &line;

   //   lineIndexToDataNode.emplace_back(line.index, newNode);
   //}

   //// Set up parents and children for all nodes
   //std::set<DataNode*> dataNodesProcessing;
   //std::function<void(DataNode*)> processNode;
   //processNode = [&](DataNode* node) -> void{
   //   if(dataNodesProcessing.count(node) != 0){
   //      ffxResearchError(path, L"Node is already being processed");
   //   }
   //   dataNodesProcessing.insert(node);

   //   int refIndex = node->line->referencedLineIndex;
   //   if(refIndex != 0){
   //      int firstIndex = refIndex;
   //      int lastIndex = firstIndex;
   //      while(
   //         lastIndex + 1 < (int)lines.size() &&
   //         lines[lastIndex + 1].referencedBy.size() == 0 &&
   //         lines[lastIndex + 1].address != rawFfx.header->data2Start &&
   //         lines[lastIndex + 1].address != rawFfx.data3Start
   //      ){
   //         ++lastIndex;
   //      }
   //      for(int i = firstIndex; i <= lastIndex; ++i){
   //         DataNode* newChild = getPair(i).second;
   //         newChild->setParentMostDepth(
   //            node,
   //            rawFfx.header->data2Start,
   //            rawFfx.data3Start
   //         );
   //         processNode(newChild);
   //      }
   //   }

   //   dataNodesProcessing.erase(node);
   //};
   //std::vector<DataNode*> childrenCopy = root->children;
   //for(DataNode* child : childrenCopy){
   //   if(child->line->address == 300){
   //      printf("%d\n", child->getDepth());
   //   }
   //   processNode(child);
   //}

   //std::string outputText;

   //outputText += "\n\n\n\n\n\n\n\n\n\n\n";
   //outputText += "################################################################################\n";
   //outputText += "DataNode approach\n";
   //outputText += "\n";

   //// Output dataNode structure

   //int addressTextSize = sprintf(sBuffer, "0x%05x %6d ", 0, 0);

   //std::function<void(DataNode*, int)> outputNode;
   //outputNode = [&](DataNode* node, int depth) -> void{
   //   if(node->line->referencedLineIndex == 2188 / 4){
   //      int bp=42;
   //   }

   //   if(node->line->address == rawFfx.header->data2Start){
   //      outputText += "\n------------------------------\nData2\n\n";
   //   }
   //   if(node->line->address == rawFfx.data3Start){
   //      outputText += "\n------------------------------\nData3\n\n";
   //   }

   //   if(depth == 0){
   //      sprintf(sBuffer, "0x%05x %6d ", node->line->address, node->line->address);
   //      outputText += sBuffer;
   //   }else{
   //      for(int s = 0; s < addressTextSize; ++s) outputText += ' ';
   //      for(int d = 0; d < depth; ++d){
   //         outputText += "   ";
   //      }
   //   }

   //   if(node->type == DataNode::Type::String){
   //      outputText += node->u.s;
   //   }else if(node->type == DataNode::Type::Int){
   //      if(node->u.i == 0){
   //         sprintf(sBuffer, "0");
   //      }else if(node->line->referencedLineIndex != 0){
   //         sprintf(sBuffer, "int %d*", node->u.i);
   //      }else{
   //         sprintf(sBuffer, "int %d", node->u.i);
   //      }
   //      outputText += sBuffer;
   //   }else if(node->type == DataNode::Type::Float){
   //      sprintf(sBuffer, "float %f", node->u.f);
   //      outputText += sBuffer;
   //   }

   //   if(node->getDepth() == 0 && node->line->referencedLineIndex == 0 && node->line->address > 8){
   //      sprintf(sBuffer, "  (Orphan?)");
   //      outputText += sBuffer;
   //   }

   //   for(int refLineAddress : node->line->referencedBy){
   //      auto findResult = std::find_if(
   //         lines.begin(),
   //         lines.end(),
   //         [&](Line& l) -> bool{return l.address == refLineAddress;}
   //      );
   //      int refLineIndex = findResult->index;
   //      DataNode* refNode = getPair(refLineIndex).second;
   //      if(refNode->line->referencedLineIndex != 0){
   //         if(refNode->line->address >= rawFfx.data3Start){
   //            outputText += "  (<-- data3)";
   //         }else if(refNode->line->address >= rawFfx.header->data2Start){
   //            outputText += "  (<-- data2)";
   //         }
   //      }
   //   }

   //   if(node->line->referencedBy.size() > 1){
   //      for(int refLineAddress : node->line->referencedBy){
   //         auto findResult = std::find_if(
   //            lines.begin(),
   //            lines.end(),
   //            [&](Line& l) -> bool{return l.address == refLineAddress;}
   //         );
   //         int refLineIndex = findResult->index;
   //         DataNode* refNode = getPair(refLineIndex).second;
   //         if(refNode->children.empty() && refNode->line->referencedLineIndex != 0){
   //            if(refNode->line->address < rawFfx.data3Start){
   //               sprintf(sBuffer, "  (<-- Middle ref from %d)", refNode->line->address);
   //               outputText += sBuffer;
   //            }
   //         }
   //      }
   //   }

   //   if(node->children.empty() && node->line->referencedLineIndex != 0){
   //      DataNode* current = getPair(node->line->referencedLineIndex).second;
   //      while(current->parent != root){
   //         current = current->parent;
   //      }
   //      if(node->u.i > rawFfx.data2[1]){
   //         DataNode* ref = getPair(node->line->referencedLineIndex).second;

   //         sprintf(sBuffer, "  (--> REFERENCE TO MIDDLE OF %d)", current->u.i);
   //         outputText += sBuffer;

   //         outputText += '\n';
   //         for(DataNode* child : ref->parent->children){
   //            outputNode(child, depth + 1);
   //         }
   //      }else{
   //         outputText += '\n';
   //      }
   //   }else{
   //      outputText += '\n';
   //   }

   //   for(DataNode* child : node->children){
   //      outputNode(child, depth + 1);
   //   }
   //};
   //for(DataNode* child : root->children){
   //   outputNode(child, 0);
   //}

   //fwrite(outputText.data(), 1, outputText.size(), file);

   fclose(file);

   sprintf(sBuffer, "cp output.txt %d.txt", rawFfx.header->ffxId);
   system(sBuffer);
}

void loadEveryFfx(std::wstring dir){
   std::vector<std::wstring> fileList;
   {
      std::wstring fileListPath = dir + L"fileListFull.txt";
      //std::wstring fileListPath = dir + L"fileListLast.txt";
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

   std::set<int> allPond2Types;
   std::vector<Pond2*> allPond2s;

   for(const std::wstring& path : fileList){
      Ffx ffx;
      loadFfxFile(ffx, dir + path, &allPond2Types, &allPond2s);
   }

   return;

   // Output pond2 types analysis
   //std::vector<int> allPond2TypesArray;
   //for(int type : allPond2Types){
   //   allPond2TypesArray.push_back(type);
   //}
   //std::sort(allPond2TypesArray.begin(), allPond2TypesArray.end());
   //printf("All Pond2 types:\n");
   //for(int type : allPond2TypesArray){
   //   printf("%d\n", type);
   //}

   // Output pond2 specific type analysis
   int researchingType = 107;
   char sBuffer[200];
   std::string outputText;
   for(Pond2* pond2 : allPond2s){
      if(pond2->type == researchingType && pond2->preDataCount == 0){
         sprintf(sBuffer, "\n---------------- pond2 type %d ----------------\n", pond2->type);
         outputText += sBuffer;

         int* ip = (int*)pond2->data.data();
         float* fp = (float*)ip;
         int address = pond2->address + 6 * 4;
         for(size_t n = 0; n < pond2->data.size(); ++n){
            float floatAbsValue = fabs(fp[n]);
            if(ip[n] == 0){
               sprintf(sBuffer, "%6d 0\n", address);
            }else if(floatAbsValue > 0.001 && floatAbsValue < 100000.0f || ip[n] == -2147483647 - 1){
               sprintf(sBuffer, "%6d float %f\n", address, fp[n]);
            }else{
               sprintf(sBuffer, "%6d int %d\n", address, ip[n]);
            }
            outputText += sBuffer;

            address += 4;
         }

         outputText += "\n\n";
      }
   }

   char outputPath[80];
   sprintf(outputPath, "P2T%03d.txt", researchingType);
   FILE* file = fopen(outputPath, "w");
   fwrite(outputText.data(), 1, outputText.size(), file);
   fclose(file);

   int bp=42;
}

void importEveryFfx(std::wstring dir){
   std::vector<std::wstring> fileList;
   {
      std::wstring fileListPath = dir + L"fileListFull.txt";
      //std::wstring fileListPath = dir + L"fileListLast.txt";
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
         if(item[0] != '\0'){
            fileList.push_back(item);
         }
      }
   }

   system("mkdir json");
   for(const std::wstring& fileName : fileList){
      ffxToJson(dir + fileName, L"json/" + fileName + L".json");
   }
}

int main(int argCount, char** args) {
   printf("Hello\n");

   //RawFfxFile rawFfx;
   //std::wstring path = L"C:/Projects/Dark Souls/FFX research/";
   //path += L"f0000482.ffx";
   //loadRawFfxFile(&rawFfx, path);
   
   //testEveryFfx(L"C:/Program Files (x86)/Steam/steamapps/common/Dark Souls Prepare to Die Edition/DATA-BR/sfx/Dark Souls (PC)/data/Sfx/OutputData/Main/Effect_win32/");


   //outputFfxAnalysis(L"f0002125.ffx");
   //outputFfxAnalysis(L"f0000482.ffx");
   //outputFfxAnalysis(L"f0000511.ffx");
   //outputFfxAnalysis(L"f0002023.ffx");
   //outputFfxAnalysis(L"f0013520.ffx");
   //outputFfxAnalysis(L"f0120034.ffx");

   Ffx ffx;
   std::wstring dir = L"C:/Program Files (x86)/Steam/steamapps/common/Dark Souls Prepare to Die Edition/DATA-BR/sfx/Dark Souls (PC)/data/Sfx/OutputData/Main/Effect_win32/";
   //for(int ffxId : {459, 482, 511, 13520, 2023, 120034}){
   //   wchar_t wBuffer[250];
   //   swprintf(wBuffer, 250, L"%sf%07d.ffx", dir.c_str(), ffxId);
   //   loadFfxFile(ffx, wBuffer);
   //}

   //loadFfxFile(ffx, dir + L"f0000511.ffx");

   //loadEveryFfx(dir);

   //loadFfxFile(ffx, L"DSR_f0000881.ffx");


   //for(int ffxId : {13520}){
   //   wchar_t wBuffer[250];
   //   swprintf(wBuffer, sizeof(wBuffer), L"%sf%07d.ffx", dir.c_str(), ffxId);
   //   std::wstring ffxPath = wBuffer;
   //   swprintf(wBuffer, sizeof(wBuffer), L"f%07d.ffx.json", ffxId);
   //   std::wstring jsonPath = wBuffer;
   //   ffxToJson(ffxPath, jsonPath);
   //}

   importEveryFfx(dir);


   system("pause");
}
