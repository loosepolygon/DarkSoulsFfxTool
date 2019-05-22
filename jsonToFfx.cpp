#include "header.hpp"

void jsonToFfx(const std::wstring& jsonPath, const std::wstring& ffxPath){
   std::string jsonText;
   {
      FILE* file = _wfopen(jsonPath.c_str(), L"r");
      if(file == NULL) {
         jsonReadError(jsonPath, L"Cannot open file");
      }

      fseek(file, 0, SEEK_END);
      long fileSize = ftell(file);
      fseek(file, 0, SEEK_SET);

      jsonText.resize(fileSize);
      fread(&jsonText[0], sizeof(jsonText[0]), jsonText.size(), file);
      fclose(file);
   }

   json::JSON root = json::JSON::Load(jsonText);
   if(root["jsonVersion"].ToInt() != ::jsonVersion){
      jsonReadError(jsonPath, L"jsonVersion not compatible with this reader");
   }

   DataWriter dwMain;
   DataWriter dwMainASTs;
   DataWriter dwHouses;
   DataWriter dwLinks;
   DataWriter dwBlossoms;
   DataWriter dwSubDataAndPond3s;
   DataWriter dwData3s;
   DataWriter dwLinkData3s;
   std::vector<DataWriter*> dwPond1Arrays;
   DataWriter dwType1Pond3AndSomeSubdata;
   std::vector<DataWriter*> dwPond2Stuff;

   std::vector<int> offsetList;
   std::vector<int> firstData3Offsets; // Types 133 and 134
   std::vector<int> data3Offsets;
   std::vector<int> linkData3Offsets;

   wchar_t wBuffer[200];

   std::function<void(DataWriter&, DataWriter*, json::JSON&)> writeData3;
   std::function<void(json::JSON&)> writeMainAST;

   writeData3 = [&](DataWriter& dw, DataWriter* dwPond1Array, json::JSON& data3) -> void{
      if(&dw == &dwLinkData3s){
         linkData3Offsets.push_back(dwLinkData3s.bytes.size());
      }else{
         data3Offsets.push_back(dw.bytes.size());
      }

      int type = data3["data3Type"].ToInt();

      if(dwPond1Array){
         dwPond1Array->writeOffsetToFix(dw, dw.bytes.size());
      }

      dw.write<int>(type);

      if(type == 112 || type == 113 || type == 129 || type == 130 || type == 131 || type == 132 || type == 136){
         // empty
      }else if(type == 1 || type == 59 || type == 66 || type == 68 || type == 111 || type == 138 || type == 139){
         dw.write<int>(data3["unk1"].ToInt());
      }else if(type == 7 || type == 70){
         dw.write<float>(data3["unk1"].ToFloat());
      }else if(type == 79 || type == 140){
         dw.write<int>(data3["unk1"].ToInt());
         dw.write<int>(data3["unk2"].ToInt());
      }else if(type == 81 || type == 85){
         dw.write<float>(data3["unk1"].ToFloat());
         dw.write<float>(data3["unk2"].ToFloat());
      }else if(type == 137){
         dw.write<int>(data3["unk1"].ToInt());
         dw.write<int>(data3["unk2"].ToInt());
         dw.write<int>(data3["unk3"].ToInt());
      }else if(type == 2){
         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());

         int count = data3["bunchaInts"].size();
         dw.write<int>(count);

         for(json::JSON& value : data3["bunchaInts"].ArrayRange()){
            dwSubDataAndPond3s.write<int>(value.ToInt());
         }
      }else if(type == 3 || type == 5 || type == 6 || type == 9 || type == 89){
         int floatCount = data3["floats"].size();
         int intCount = data3["ints"].size();
         if(floatCount != intCount){
            swprintf(wBuffer, sizeof(wBuffer), L"Data3 type %d count mismatch", type);
            jsonReadError(ffxPath, wBuffer);
         }

         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());
         for(json::JSON& value : data3["floats"].ArrayRange()){
            dwSubDataAndPond3s.write<float>(value.ToFloat());
         }

         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());
         for(json::JSON& value : data3["ints"].ArrayRange()){
            dwSubDataAndPond3s.write<int>(value.ToInt());
         }
         
         dw.write<int>(floatCount);

         if(type == 89){
            dw.write<int>(data3["unk1"].ToInt());
            dw.write<int>(data3["unk2"].ToInt());
         }
      }else if(type == 11 || type == 12 || type == 91 || type == 95){
         int floatCountA = data3["floatsA"].size();
         int floatCountB = data3["floatsB"].size();
         if(floatCountA != floatCountB){
            swprintf(wBuffer, sizeof(wBuffer), L"Data3 type %d count mismatch", type);
            jsonReadError(ffxPath, wBuffer);
         }

         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());
         for(json::JSON& value : data3["floatsA"].ArrayRange()){
            dwSubDataAndPond3s.write<float>(value.ToFloat());
         }

         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());
         for(json::JSON& value : data3["floatsB"].ArrayRange()){
            dwSubDataAndPond3s.write<float>(value.ToFloat());
         }

         dw.write<int>(floatCountA);

         if(type == 91 || type == 95){
            dw.write<int>(data3["unk1"].ToInt());

            dw.writeOffsetToFix(dw, dw.bytes.size() + 4);
            writeData3(dw, nullptr, data3["data3"]);
         }
      }else if(type == 13 || type == 14){
         int floatCountA = data3["floatsA"].size();
         int floatCountB = data3["floatsB"].size();
         if(floatCountA * 3 != floatCountB){
            swprintf(wBuffer, sizeof(wBuffer), L"Data3 type %d count mismatch", type);
            jsonReadError(ffxPath, wBuffer);
         }

         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());
         for(json::JSON& value : data3["floatsA"].ArrayRange()){
            dwSubDataAndPond3s.write<float>(value.ToFloat());
         }

         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());
         for(json::JSON& value : data3["floatsB"].ArrayRange()){
            dwSubDataAndPond3s.write<float>(value.ToFloat());
         }

         dw.write<int>(floatCountA);
      }else if(type == 19 || type == 20){
         int floatCountA = data3["floatsA"].size();
         int floatCountB = data3["floatsB"].size();
         if(floatCountA * 4 != floatCountB){
            swprintf(wBuffer, sizeof(wBuffer), L"Data3 type %d count mismatch", type);
            jsonReadError(ffxPath, wBuffer);
         }

         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());
         for(json::JSON& value : data3["floatsA"].ArrayRange()){
            dwSubDataAndPond3s.write<float>(value.ToFloat());
         }

         dw.writeOffsetToFix(dwType1Pond3AndSomeSubdata, dwType1Pond3AndSomeSubdata.bytes.size());
         for(json::JSON& value : data3["floatsB"].ArrayRange()){
            dwType1Pond3AndSomeSubdata.write<float>(value.ToFloat());
         }

         dw.write<int>(floatCountA);
      }else if(type == 37){
         dw.write<int>(data3["templateFfxId"].ToInt());

         int templateASTIndex = data3["templateASTIndex"].ToInt();
         if(templateASTIndex == -1){
            dw.write<int>(0);
            dw.write<int>(0);
         }else{
            dw.writeOffsetToFix(dwMainASTs, dwMainASTs.bytes.size());
            dw.write<int>(0);

            writeMainAST(root["templateASTs"][templateASTIndex]);
         }
      }else if(type == 38){
         dw.write<int>(data3["t38Subtype"].ToInt());

         json::JSON& ast = data3["ast"];
         if(ast.size() == 0){
            dw.write<int>(0);
            dw.write<int>(0);
         }else{
            dw.writeOffsetToFix(dwMainASTs, dwMainASTs.bytes.size());
            dw.write<int>(0);

            writeMainAST(ast);
         }
      }else if(type == 44 || type == 45 || type == 46 || type == 47 || type == 60 || type == 71 || type == 87 || type == 114 || type == 115){
         dw.write<short>(data3["unk1"].ToInt());
         dw.write<short>(data3["unk2"].ToInt());
      }else if(type == 128){
         dw.writeOffsetToFix(dw, dw.bytes.size() + 4);
         writeData3(dw, nullptr, data3["data3"]);
      }else if(type == 120 || type == 121 || type == 122 || type == 123 || type == 124 || type == 126 || type == 127){
         dw.writeOffsetToFix(dw, dw.bytes.size() + 8);
         int offsetB = dw.bytes.size();
         dw.write<int>(-1);
         writeData3(dw, nullptr, data3["data3A"]);

         dw.addOffsetToFixAt(offsetB, dw, dw.bytes.size());
         writeData3(dw, nullptr, data3["data3B"]);
      }else{
         swprintf(wBuffer, sizeof(wBuffer), L"Data3 type %d unfamiliar", type);
         jsonReadError(ffxPath, wBuffer);
      }
   };

   auto writeASTAt = [&](DataWriter& dw, int astOffset, json::JSON& ast) -> void{
      int flags = 0;
      if(ast["flag1"].ToBool()) flags |= 0x1;
      if(ast["flag2"].ToBool()) flags |= 0x100;
      if(ast["flag3"].ToBool()) flags |= 0x10000;

      dw.writeAt<int>(astOffset + 0,  0);
      dw.writeAt<int>(astOffset + 4,  0);
      dw.writeAt<int>(astOffset + 8,  0);
      dw.writeAt<int>(astOffset + 12, flags);
      dw.writeAt<int>(astOffset + 16, 0);
      dw.writeAt<int>(astOffset + 20, 0);

      int astType = ast["astType"].ToInt();
      if(astType == 0){
         // Empty
      }else if(astType == 1){
         int data3Count = ast["pond1Data3s"].size();
         dw.writeAt<int>(astOffset + 4, data3Count);
         dw.writeAt<int>(astOffset + 8, data3Count); // Fromsoft

         dwPond1Arrays.push_back(new DataWriter);
         DataWriter* dwPond1Array = dwPond1Arrays.back();
         dw.addOffsetToFixAt(astOffset + 0, *dwPond1Array, 0);

         for(int d = 0; d < data3Count; ++d){
            json::JSON& data3 = ast["pond1Data3s"][d];
            writeData3(dwData3s, dwPond1Array, data3);
         }

         json::JSON& pond3 = ast["pond3"];
         bool hasPond3 = pond3.size() > 0;
         if(hasPond3){
            int type = pond3["pond3Type"].ToInt();

            if(type == 1){
               dw.addOffsetToFixAt(astOffset + 20, dwType1Pond3AndSomeSubdata, dwType1Pond3AndSomeSubdata.bytes.size());
               dwType1Pond3AndSomeSubdata.write<int>(type);
            }else{
               dw.addOffsetToFixAt(astOffset + 20, dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());
               dwSubDataAndPond3s.write<int>(type);
            }

            if(type == 0){
               dwSubDataAndPond3s.write<int>(pond3["astCountMaybe"].ToInt());
            }else if(type == 1){
               dwType1Pond3AndSomeSubdata.write<int>(pond3["unk1"].ToInt());
               dwType1Pond3AndSomeSubdata.write<int>(pond3["unk2"].ToInt());
               dwType1Pond3AndSomeSubdata.write<int>(pond3["unk3"].ToInt());
               dwType1Pond3AndSomeSubdata.write<float>(pond3["offsetX"].ToFloat());
               dwType1Pond3AndSomeSubdata.write<float>(pond3["offsetY"].ToFloat());
               dwType1Pond3AndSomeSubdata.write<int>(pond3["unk6"].ToInt());
               dwType1Pond3AndSomeSubdata.write<int>(pond3["unk7"].ToInt());
               dwType1Pond3AndSomeSubdata.write<int>(pond3["unk8"].ToInt());
               dwType1Pond3AndSomeSubdata.write<float>(pond3["unk9"].ToFloat());
               dwType1Pond3AndSomeSubdata.write<float>(pond3["unk10"].ToFloat());
               dwType1Pond3AndSomeSubdata.write<float>(pond3["unk11"].ToFloat());
            }else if(type == 2){
               dwSubDataAndPond3s.write<float>(pond3["lifetime"].ToFloat());
               dwSubDataAndPond3s.write<float>(pond3["unk2"].ToFloat());
               dwSubDataAndPond3s.write<int>(pond3["unk3"].ToInt());
               dwSubDataAndPond3s.write<float>(pond3["unk4"].ToFloat());
               dwSubDataAndPond3s.write<byte>(pond3["unk5"].ToInt());
               dwSubDataAndPond3s.write<byte>(pond3["unk6"].ToInt());
               dwSubDataAndPond3s.write<short>(0);
            }else if(type == 3){
               dwSubDataAndPond3s.write<float>(pond3["unk1"].ToFloat());
               dwSubDataAndPond3s.write<int>(pond3["unk2"].ToInt());
               dwSubDataAndPond3s.write<float>(pond3["unk3"].ToFloat());
               dwSubDataAndPond3s.write<int>(pond3["unk4"].ToInt());
            }else if(type == 4){
               dwSubDataAndPond3s.write<int>(pond3["unk1"].ToInt());
            }else if(type == 5){
               dwSubDataAndPond3s.write<int>(pond3["unk1"].ToInt());
            }else if(type == 6){
               dwSubDataAndPond3s.write<float>(pond3["unk1"].ToFloat());
               dwSubDataAndPond3s.write<int>(pond3["unk2"].ToInt());
            }else if(type == 7){
               dwSubDataAndPond3s.write<int>(pond3["unk1"].ToInt());
            }else{
               swprintf(wBuffer, sizeof(wBuffer), L"Pond3 type %d umfamiliar", type);
               jsonReadError(ffxPath, wBuffer);
            }
         }
      }else if(astType == 2){
         DataWriter* dwPond2 = new DataWriter;
         DataWriter* dwPreDataNumbers = new DataWriter;
         DataWriter* dwPreDataSubtypes = new DataWriter;
         DataWriter* dwSubtypeData = new DataWriter;
         dwPond2Stuff.push_back(dwPond2);
         dwPond2Stuff.push_back(dwPreDataNumbers);
         dwPond2Stuff.push_back(dwPreDataSubtypes);
         dwPond2Stuff.push_back(dwSubtypeData);

         dwSubtypeData->padToMultiple = 16;

         dw.addOffsetToFixAt(astOffset + 16, *dwPond2, 0);

         json::JSON& pond2 = ast["pond2"];
         int pond2Type = pond2["pond2Type"].ToInt();
         dwPond2->write<int>(pond2Type);
         dwPond2->write<int>(-1); // total size
         dwPond2->write<int>(pond2["preData"].size());
         dwPond2->writeOffsetToFix(*dwPreDataNumbers, 0);
         dwPond2->writeOffsetToFix(*dwPreDataSubtypes, 0);
         dwPond2->writeOffsetToFix(dw, astOffset);

         auto writeSubtype = [&](DataWriter& dw, json::JSON& obj) -> void{
            int subtype = obj["subtypeType"].ToInt();
            dw.write<int>(subtype);

            if(subtype == 28){
               dw.write<int>(0);
            }else{
               dw.writeOffsetToFix(*dwSubtypeData, dwSubtypeData->bytes.size());
            }

            bool isCurve = (
               subtype == 0 || subtype == 4 || subtype == 5 || subtype == 6 || subtype == 7 ||
               subtype == 8 || subtype == 9 || subtype == 12 || subtype == 16 || subtype == 17
            );
            if(isCurve){
               bool hasRange = subtype == 5 || subtype == 7 || subtype == 9 || subtype == 17;
               bool hasPreDataIndex = subtype == 6 || subtype == 7;
               int numArrays = subtype == 8 || subtype == 9 ? 4 : 2;

               dwSubtypeData->write<int>(obj["times"].size());

               for(json::JSON& t : obj["times"].ArrayRange()){
                  dwSubtypeData->write<float>(t.ToFloat());
               }

               if(numArrays == 2){
                  for(json::JSON& v : obj["values"].ArrayRange()){
                     dwSubtypeData->write<float>(v.ToFloat());
                  }
               }else{
                  for(const json::JSON& values : {obj["values1"], obj["values2"], obj["values3"]}){
                     for(const json::JSON& v : values.ArrayRange()){
                        dwSubtypeData->write<float>(v.ToFloat());
                     }
                  }
               }

               if(hasRange){
                  dwSubtypeData->write<float>(obj["range"][0].ToFloat());
                  dwSubtypeData->write<float>(obj["range"][1].ToFloat());
               }

               if(hasPreDataIndex){
                  dwSubtypeData->write<int>(obj["preDataIndex"].ToInt());
               }
            }else if(subtype == 24){
               dwSubtypeData->write<float>(obj["constant"].ToFloat());
            }else if(subtype == 25){
               dwSubtypeData->write<float>(obj["base"].ToFloat());
               dwSubtypeData->write<float>(obj["unknownA"].ToFloat());
               dwSubtypeData->write<float>(obj["unknownB"].ToFloat());
            }else if(subtype == 26){
               dwSubtypeData->write<float>(obj["unk1"].ToFloat());
               dwSubtypeData->write<int>(obj["unk2"].ToInt());
            }else if(subtype == 27){
               dwSubtypeData->write<float>(obj["unk1"].ToFloat());
               dwSubtypeData->write<float>(obj["unk2"].ToFloat());
               dwSubtypeData->write<float>(obj["unk3"].ToFloat());
               dwSubtypeData->write<int>(obj["preDataIndex"].ToInt());
            }else if(subtype == 28){
               // Empty
            }else{
               swprintf(wBuffer, sizeof(wBuffer), L"Pond2 subtype %d unfamiliar", subtype);
               jsonReadError(ffxPath, wBuffer);
            }
         };

         for(json::JSON& preData : pond2["preData"].ArrayRange()){
            dwPreDataNumbers->write<int>(preData["number"].ToInt());
            writeSubtype(*dwPreDataSubtypes, preData["subtype"]);
         }

         std::vector<json::JSON> vars;
         int varIndex = 0;
         for(std::pair<std::string, json::JSON>& pair : pond2.ObjectRange()){
            if(varIndex++ >= 3){
               vars.push_back(pair.second);
            }
         }
         varIndex = 0;

         auto writeZero = [&]() -> void{
            dwPond2->write<int>(0);
         };
         auto writeInt = [&]() -> void{
            dwPond2->write<int>(vars[varIndex++].ToInt());
         };
         auto writeFloat = [&]() -> void{
            dwPond2->write<float>(vars[varIndex++].ToFloat());
         };
         auto writeSubtypes = [&](int subtypeCount) -> void{
            for(int n = 0; n < subtypeCount; ++n){
               writeSubtype(*dwPond2, vars[varIndex++]);
            }
         };
         auto writeSubtypesWithDataOrder = [&](std::vector<int> subtypeOrder) -> void{
            // subtypeOrder index: subtype *data* order
            // subtypeOrder value: subtype index

            int pond2OldSize = dwPond2->bytes.size();
            int subtypeDataOldSize = dwSubtypeData->bytes.size();

            std::vector<std::pair<int, int>> subtypeDataSlices;
            for(size_t n = 0; n < subtypeOrder.size(); ++n){
               int oldSize = dwSubtypeData->bytes.size();
               writeSubtype(*dwPond2, vars[varIndex++]);
               subtypeDataSlices.emplace_back(
                  oldSize - subtypeDataOldSize,
                  dwSubtypeData->bytes.size() - subtypeDataOldSize
               );
            }

            std::vector<byte> subtypeDataCopy(
               dwSubtypeData->bytes.begin() + subtypeDataOldSize,
               dwSubtypeData->bytes.end()
            );
            dwSubtypeData->bytes.resize(subtypeDataOldSize);
            
            for(size_t subtypeDataIndex = 0; subtypeDataIndex < subtypeOrder.size(); ++subtypeDataIndex){
               int subtypeIndex = subtypeOrder[subtypeDataIndex];

               int subtypeOffset = pond2OldSize + subtypeIndex * sizeof(int) * 2;
               int type = dwPond2->read<int>(subtypeOffset + 0);

               if(type != 28){
                  dwPond2->replaceOffsetToFix(subtypeOffset + 4, *dwSubtypeData, dwSubtypeData->bytes.size());
                  dwSubtypeData->bytes.insert(
                     dwSubtypeData->bytes.end(),
                     subtypeDataCopy.begin() + subtypeDataSlices[subtypeIndex].first,
                     subtypeDataCopy.begin() + subtypeDataSlices[subtypeIndex].second
                  );
               }
            }
         };

         if(pond2Type == 27){
            writeFloat();
            writeFloat();
            writeFloat();
            writeZero();
            writeZero();
            writeInt();
            writeInt();
            writeZero();
            writeSubtypes(10);
            writeInt();
            writeInt();
            writeInt();
            writeFloat();
         }else if(pond2Type == 28){
            writeSubtypes(3);
            writeInt();
         }else if(pond2Type == 29){
            writeSubtypes(5);
            writeInt();
         }else if(pond2Type == 30){
            writeSubtypes(4);
            writeFloat();
            writeInt();
            writeInt();
         }else if(pond2Type == 31){
            writeSubtypes(4);
            writeInt();
            writeInt();
         }else if(pond2Type == 32){
            writeSubtypes(6);
            writeInt();
            writeInt();
         }else if(pond2Type == 40){
            writeZero();
            writeFloat();
            writeInt();
            writeZero();
            writeInt();
            writeInt();
            writeInt();
            writeSubtypes(4);
            writeFloat();
            writeFloat();
            writeInt();
            writeInt();
            writeZero();
            writeSubtypes(4);
            writeInt();
            writeInt();
            writeZero();
            writeSubtypes(1);
            writeInt();
            writeFloat();
            writeSubtypes(2);
            writeInt();
         }else if(pond2Type == 43){
            writeFloat();
            writeZero();
            writeZero();
            writeInt();
            writeInt();
            writeInt();
            writeInt();
            writeInt();
            writeInt();
            writeSubtypesWithDataOrder({0, 1, 2, 3, 6, 9, 10, 11, 12, 7, 8, 4, 5});
            writeInt();
         }else if(pond2Type == 55){
            writeSubtypes(3);
            writeZero();
            writeFloat();
         }else if(pond2Type == 59){
            writeZero();
            writeZero();
            writeInt();
            writeZero();
            writeInt();
            writeInt();
            writeSubtypes(5);
            writeInt();
            writeInt();
            writeSubtypes(8);
            writeInt();
            writeInt();
            writeZero();
            writeInt();
            writeFloat();
            writeZero();
         }else if(pond2Type == 61){
            writeZero();
            writeZero();
            writeInt();
            writeInt();
            writeInt();
            writeInt();
            writeZero();
            writeSubtypes(3);
            writeZero();
            writeZero();
            writeInt();
            writeFloat();
            writeZero();
            writeSubtypes(1);
            writeInt();
            writeInt();
            writeSubtypes(10);
            writeInt();
            writeInt();
            writeZero();
            writeZero();
         }else if(pond2Type == 66){
            writeFloat();
            writeFloat();
            writeZero();
            writeInt();
            writeFloat();
            writeInt();
            writeSubtypes(26);
            writeZero();
         }else if(pond2Type == 70){
            writeFloat();
            writeFloat();
            writeFloat();
            writeInt();
            writeFloat();
            writeInt();
            writeInt();
            writeInt();
            writeInt();
            writeInt();
            writeInt();
            writeSubtypes(10);
            writeSubtypesWithDataOrder({
               12, 13, 14, 15, 16, 17, 18, 19,
               0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
            });
            writeInt();
            writeZero();
            writeZero();
            writeZero();
            writeZero();
            writeInt();
            writeInt();
            writeInt();
            writeFloat();
            writeInt();
            writeFloat();
            writeInt();
         }else if(pond2Type == 71){
            writeFloat();
            writeFloat();
            writeFloat();
            writeInt();
            writeFloat();
            writeInt();
            writeZero();
            writeInt();
            writeInt();
            writeInt();
            writeSubtypes(10);
            writeInt();
            writeInt();
            writeSubtypesWithDataOrder({2, 3, 4, 5, 6, 7, 8, 9, 0, 1});
            writeInt();
            writeZero();
            writeZero();
            writeZero();
            writeZero();
            writeInt();
            writeInt();
            writeInt();
            writeFloat();
            writeInt();
            writeFloat();
            writeInt();
         }else if(pond2Type == 84){
            writeSubtypes(3);
            writeZero();
            writeFloat();
            writeSubtypes(1);
            writeInt();
         }else if(pond2Type == 105){
            writeSubtypes(3);
            writeZero();
            writeFloat();
            writeSubtypes(1);
            writeInt();
            writeSubtypes(1);
         }else if(pond2Type == 107){
            writeZero();
            writeZero();
            writeInt();
            writeInt();
            writeSubtypes(7);
         }else if(pond2Type == 108){
            writeFloat();
            writeFloat();
            writeFloat();
            writeInt();
            writeFloat();
            writeInt();
            writeInt();
            writeInt();
            writeInt();
            writeZero();
            writeSubtypes(12);
            writeInt();
            writeInt();
            writeSubtypes(14);
            writeInt();
            writeInt();
            writeInt();
            writeFloat();
            writeInt();
         }else{
            jsonReadError(ffxPath, L"pond2 unknown pond2Type");
         }

         int pond2Size = (
            dwPond2->bytes.size() +
            dwPreDataNumbers->bytes.size() +
            dwPreDataSubtypes->bytes.size() +
            dwSubtypeData->bytes.size()
         );
         dwPond2->writeAt<int>(4, pond2Size);
      }else{
         ffxWriteError(ffxPath, L"Unfamiliar AST type");
      }
   };

   auto writeAST = [&](DataWriter& dw, json::JSON& ast) -> void{
      writeASTAt(dw, dw.bytes.size(), ast);
   };

   writeMainAST = [&](json::JSON& ast) -> void{
      writeASTAt(dwMainASTs, dwMainASTs.bytes.size(), ast);
   };


   dwMain.write('F');
   dwMain.write('X');
   dwMain.write('R');
   dwMain.write('\0');

   dwMain.write<short>(0);
   dwMain.write<short>(root["ffxVersion"].ToInt());
   offsetList.push_back(dwMain.bytes.size());
   dwMain.write<int>(32); // dataStartAfterHeader
   dwMain.write<int>(-1); // data2Start
   dwMain.write<int>(-1); // data2Count
   dwMain.write<int>(-1); // data3Count
   dwMain.write<int>(root["unk1"].ToInt());
   dwMain.write<int>(root["unk2"].ToInt());

   int t133Count = root["type133s"].size();
   bool needsType134 = t133Count > 1;
   if(needsType134){
      firstData3Offsets.push_back(dwMain.bytes.size());

      dwMain.write<int>(134);
      dwMain.write<int>(root["ffxId"].ToInt());
      dwMain.write<int>(0);

      DataWriter* dwType133OffsetArray = new DataWriter;
      dwPond1Arrays.emplace_back(dwType133OffsetArray);

      dwMain.writeOffsetToFix(*dwType133OffsetArray, 0);
      dwMain.write<int>(t133Count);

      for(int n = 0; n < t133Count; ++n){
         int t133Offset = dwMain.bytes.size() + 96 * n;
         dwType133OffsetArray->writeOffsetToFix(dwMain, t133Offset);
      }
   }
   
   for(json::JSON& type133 : root["type133s"].ArrayRange()){
      firstData3Offsets.push_back(dwMain.bytes.size());

      dwMain.write<int>(133);
      dwMain.write<int>(root["ffxId"].ToInt());
      for(int n = 0; n < 7; ++n){
         dwMain.write<int>(0);
      }
      dwMain.write<int>(type133["always8Or10"].ToInt());
      json::JSON& preAST1 = type133["preAST1"];
      json::JSON& preAST2 = type133["preAST2"];
      writeAST(dwMain, preAST1);
      writeAST(dwMain, preAST2);
      dwMain.writeOffsetToFix(dwHouses, dwHouses.bytes.size());
      dwMain.write<int>(type133["houses"].size());

      json::JSON& houses = type133["houses"];

      for(int h = 0; h < houses.size(); ++h){
         json::JSON& house = houses[h];
         int linkCount = house["links"].size();
         int blossomCount = house["blossoms"].size();

         house["_offset"] = dwHouses.bytes.size();
         if(linkCount > 0){
            dwHouses.write<int>(-1);
         }else{
            dwHouses.write<int>(0);
         }

         if(blossomCount > 0){
            dwHouses.writeOffsetToFix(dwBlossoms, dwBlossoms.bytes.size());
         }else{
            dwHouses.write<int>(0);
         }

         dwHouses.write<int>(linkCount);
         dwHouses.write<int>(blossomCount);

         for(json::JSON& blossom : house["blossoms"].ArrayRange()){
            dwBlossoms.write<int>(blossom["blossomType"].ToInt());
            writeAST(dwBlossoms, blossom["blossomAST"]);
         }
      }

      // Another houses loop to write links (especially since the links' data3 comes last)
      for(int h = 0; h < houses.size(); ++h){
         json::JSON& house = houses[h];
         int currentHouseOffset = house["_offset"].ToInt();

         // Fix the house's "links" offset
         if(house["links"].size() > 0){
            dwHouses.addOffsetToFixAt(currentHouseOffset, dwLinks, dwLinks.bytes.size());
         }

         for(json::JSON& link : house["links"].ArrayRange()){
            int linkedHouseIndex = link["linkedHouseIndex"].ToInt();
            int linkedHouseOffset = houses[linkedHouseIndex]["_offset"].ToInt();

            // Write link's house offset
            dwLinks.writeOffsetToFix(dwHouses, linkedHouseOffset);

            // Write link's data3
            dwLinks.writeOffsetToFix(dwLinkData3s, dwLinkData3s.bytes.size());
            writeData3(dwLinkData3s, nullptr, link["data3"]);
         }
      }
   }

   // Merge DataWriters
   if(!dwPond1Arrays.empty()){
      dwPond1Arrays.back()->padToMultiple = 16;
   }
   std::vector<DataWriter*> dataWriters = {
      &dwMainASTs,
      &dwHouses,
      &dwLinks,
      &dwBlossoms,
      &dwSubDataAndPond3s,
      &dwData3s,
      &dwLinkData3s,
      &dwType1Pond3AndSomeSubdata
   };
   dataWriters.insert(dataWriters.begin() + 7, dwPond1Arrays.begin(), dwPond1Arrays.end());
   dataWriters.insert(dataWriters.end() - 0, dwPond2Stuff.begin(), dwPond2Stuff.end());
   dataWriters.back()->padToMultiple = 16;
   dwMain.merge(dataWriters, offsetList);

   // Write offsetList
   dwMain.writeAt<int>(12, dwMain.bytes.size());
   std::sort(offsetList.begin(), offsetList.end());
   // Make unique
   for(int n = offsetList.size(); n --> 1;){
      int offsetA = offsetList[n - 1];
      int offsetB = offsetList[n - 0];
      if(offsetA == offsetB){
         offsetList.erase(offsetList.begin() + n);
      }
   }
   for(int offset : offsetList){
      dwMain.write<int>(offset);
   }
   dwMain.writeAt<int>(16, offsetList.size());

   // Write data3Offsets
   for(int offset : firstData3Offsets){
      dwMain.write<int>(offset);
   }
   std::sort(data3Offsets.begin(), data3Offsets.end());
   for(int offset : data3Offsets){
      dwMain.write<int>(dwData3s.finalOffset + offset);
   }
   for(int offset : linkData3Offsets){
      dwMain.write<int>(dwLinkData3s.finalOffset + offset);
   }
   dwMain.writeAt<int>(20, firstData3Offsets.size() + data3Offsets.size() + linkData3Offsets.size());

   // Cleanup
   for(DataWriter* dw : dwPond1Arrays){
      delete dw;
   }
   for(DataWriter* dw : dwPond2Stuff){
      delete dw;
   }

   // Write to disk
   FILE* file = _wfopen(ffxPath.c_str(), L"wb");
   if(file == NULL) {
      ffxWriteError(ffxPath, L"Cannot open file");
      return;
   }
   fwrite(dwMain.bytes.data(), 1, dwMain.bytes.size(), file);
   fclose(file);
}
