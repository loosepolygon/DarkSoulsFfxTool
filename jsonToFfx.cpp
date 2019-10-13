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

   bool isRemaster = root["gameVersion"].ToString() == "DSR";

   DataWriter dwMain(isRemaster);
   DataWriter dwMainASTs(isRemaster);
   DataWriter dwHouses(isRemaster);
   DataWriter dwLinks(isRemaster);
   DataWriter dwBlossoms(isRemaster);
   DataWriter dwSubDataAndPond3s(isRemaster);
   DataWriter dwData3s(isRemaster);
   DataWriter dwLinkData3s(isRemaster);
   std::vector<DataWriter*> dwPond1Arrays;
   DataWriter dwType1Pond3AndSomeSubdata(isRemaster);
   std::vector<DataWriter*> dwPond2Stuff;

   // offsetList contains all offsets to be written at the end of the file;
   // DataWriter::Merge adds the majority of the offsets
   std::vector<int> offsetList;
   std::vector<int> firstData3Offsets; // Types 133 and 134
   std::vector<int> data3Offsets;
   std::vector<int> linkData3Offsets;

   wchar_t wBuffer[200];

   int longSize = isRemaster ? 8 : 4;

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

      dw.writeLong(type);

      if(type == 112 || type == 113 || type == 129 || type == 130 || type == 131 || type == 132 || type == 136){
         // empty
      }else if(
         type == 1 || type == 41 || type == 59 || type == 66 || type == 68 || type == 69 ||
         type == 111 || type == 138 || type == 139
      ){
         dw.writeLong(data3["unk1"].ToInt());
      }else if(type == 7 || type == 70){
         dw.writeFloat(data3["unk1"].ToFloat());
         if(isRemaster) dw.writeInt(0);
      }else if(type == 79 || type == 140){
         dw.writeInt(data3["unk1"].ToInt());
         dw.writeInt(data3["unk2"].ToInt());
      }else if(type == 81 || type == 85){
         dw.writeFloat(data3["unk1"].ToFloat());
         dw.writeFloat(data3["unk2"].ToFloat());
      }else if(type == 137){
         // This type is not seen in DSR
         dw.writeInt(data3["unk1"].ToInt());
         dw.writeInt(data3["unk2"].ToInt());
         dw.writeInt(data3["unk3"].ToInt());
      }else if(type == 2){
         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());

         int count = data3["bunchaInts"].size();
         dw.writeLong(count);

         for(json::JSON& value : data3["bunchaInts"].ArrayRange()){
            dwSubDataAndPond3s.writeInt(value.ToInt());
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
            dwSubDataAndPond3s.writeFloat(value.ToFloat());
         }

         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());
         for(json::JSON& value : data3["ints"].ArrayRange()){
            dwSubDataAndPond3s.writeInt(value.ToInt());
         }
         
         dw.writeLong(floatCount);

         if(type == 89){
            dw.writeLong(data3["unk1"].ToInt());
            dw.writeLong(data3["unk2"].ToInt());
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
            dwSubDataAndPond3s.writeFloat(value.ToFloat());
         }

         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());
         for(json::JSON& value : data3["floatsB"].ArrayRange()){
            dwSubDataAndPond3s.writeFloat(value.ToFloat());
         }

         dw.writeLong(floatCountA);

         // These types are not seen in DSR
         if(type == 91 || type == 95){
            dw.writeLong(data3["unk1"].ToInt());

            dw.writeOffsetToFix(dw, dw.bytes.size() + longSize);
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
            dwSubDataAndPond3s.writeFloat(value.ToFloat());
         }

         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());
         for(json::JSON& value : data3["floatsB"].ArrayRange()){
            dwSubDataAndPond3s.writeFloat(value.ToFloat());
         }

         dw.writeLong(floatCountA);
      }else if(type == 19 || type == 20 || type == 27 || type == 28){
         int floatCountA = data3["floatsA"].size();
         int floatCountB = data3["floatsB"].size();
         if(floatCountA * 4 != floatCountB){
            swprintf(wBuffer, sizeof(wBuffer), L"Data3 type %d count mismatch", type);
            jsonReadError(ffxPath, wBuffer);
         }

         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());
         for(json::JSON& value : data3["floatsA"].ArrayRange()){
            dwSubDataAndPond3s.writeFloat(value.ToFloat());
         }

         dw.writeOffsetToFix(dwType1Pond3AndSomeSubdata, dwType1Pond3AndSomeSubdata.bytes.size());
         for(json::JSON& value : data3["floatsB"].ArrayRange()){
            dwType1Pond3AndSomeSubdata.writeFloat(value.ToFloat());
         }

         dw.writeLong(floatCountA);
      }else if(type == 21 || type == 22 || type == 29){
         int floatCountA = data3["floatsA"].size();
         int floatCountB = data3["floatsB"].size();
         if(floatCountA * 12 != floatCountB){
            swprintf(wBuffer, sizeof(wBuffer), L"Data3 type %d count mismatch", type);
            jsonReadError(ffxPath, wBuffer);
         }

         dw.writeOffsetToFix(dwSubDataAndPond3s, dwSubDataAndPond3s.bytes.size());
         for(json::JSON& value : data3["floatsA"].ArrayRange()){
            dwSubDataAndPond3s.writeFloat(value.ToFloat());
         }

         dw.writeOffsetToFix(dwType1Pond3AndSomeSubdata, dwType1Pond3AndSomeSubdata.bytes.size());
         for(json::JSON& value : data3["floatsB"].ArrayRange()){
            dwType1Pond3AndSomeSubdata.writeFloat(value.ToFloat());
         }

         dw.writeLong(floatCountA);
      }else if(type == 37){
         dw.writeLong(data3["templateFfxId"].ToInt());

         int templateASTIndex = data3["templateASTIndex"].ToInt();
         if(templateASTIndex == -1){
            dw.writeLong(0);
            dw.writeLong(0);
         }else{
            dw.writeOffsetToFix(dwMainASTs, dwMainASTs.bytes.size());
            dw.writeLong(0);

            writeMainAST(root["templateASTs"][templateASTIndex]);
         }
      }else if(type == 38){
         dw.writeLong(data3["t38Subtype"].ToInt());

         json::JSON& ast = data3["ast"];
         if(ast.size() == 0){
            dw.writeLong(0);
            dw.writeLong(0);
         }else{
            dw.writeOffsetToFix(dwMainASTs, dwMainASTs.bytes.size());
            dw.writeLong(0);

            writeMainAST(ast);
         }
      }else if(type == 44 || type == 45 || type == 46 || type == 47 || type == 60 || type == 71 || type == 87 || type == 114 || type == 115){
         dw.writeShort(data3["unk1"].ToInt());
         dw.writeShort(data3["unk2"].ToInt());
         if(isRemaster) dw.writeInt(0);
      }else if(type == 128){
         dw.writeOffsetToFix(dw, dw.bytes.size() + longSize);
         writeData3(dw, nullptr, data3["data3"]);
      }else if(type == 120 || type == 121 || type == 122 || type == 123 || type == 124 || type == 126 || type == 127){
         dw.writeOffsetToFix(dw, dw.bytes.size() + longSize * 2);
         int offsetB = dw.bytes.size();
         dw.writeLong(-1);
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

      int posPond1Offset = astOffset;
      int posData3Count =  astOffset + (isRemaster ? 8 : 4);
      int posData3Count2 = astOffset + (isRemaster ? 12 : 8);
      int posflags =       astOffset + (isRemaster ? 16 : 12);
      int posPond2Offset = astOffset + (isRemaster ? 24 : 16);
      int posPond3Offset = astOffset + (isRemaster ? 32 : 20);

      dw.writeLongAt(posPond1Offset, 0);
      dw.writeIntAt(posData3Count, 0);
      dw.writeIntAt(posData3Count2, 0);
      dw.writeLongAt(posflags, flags);
      dw.writeLongAt(posPond2Offset, 0);
      dw.writeLongAt(posPond3Offset, 0);

      int astType = ast["astType"].ToInt();
      if(astType == 0){
         // Empty
      }else if(astType == 1){
         int data3Count = ast["pond1Data3s"].size();
         dw.writeIntAt(posData3Count, data3Count);
         dw.writeIntAt(posData3Count2, data3Count); // Fromsoft

         dwPond1Arrays.push_back(new DataWriter(isRemaster));
         DataWriter* dwPond1Array = dwPond1Arrays.back();
         dw.addOffsetToFixAt(posPond1Offset, *dwPond1Array, 0);

         for(int d = 0; d < data3Count; ++d){
            json::JSON& data3 = ast["pond1Data3s"][d];
            writeData3(dwData3s, dwPond1Array, data3);
         }

         json::JSON& pond3 = ast["pond3"];
         bool hasPond3 = pond3.size() > 0;
         if(hasPond3){
            int type = pond3["pond3Type"].ToInt();

            if(type == 1){
               dw.addOffsetToFixAt(
                  posPond3Offset,
                  dwType1Pond3AndSomeSubdata,
                  dwType1Pond3AndSomeSubdata.bytes.size()
               );
               dwType1Pond3AndSomeSubdata.writeInt(type);
            }else{
               dw.addOffsetToFixAt(
                  posPond3Offset,
                  dwSubDataAndPond3s,
                  dwSubDataAndPond3s.bytes.size()
               );
               dwSubDataAndPond3s.writeInt(type);
            }

            if(type == 0){
               dwSubDataAndPond3s.writeInt(pond3["astCountMaybe"].ToInt());
            }else if(type == 1){
               dwType1Pond3AndSomeSubdata.writeInt(pond3["unk1"].ToInt());
               dwType1Pond3AndSomeSubdata.writeInt(pond3["unk2"].ToInt());
               dwType1Pond3AndSomeSubdata.writeInt(pond3["unk3"].ToInt());
               dwType1Pond3AndSomeSubdata.writeFloat(pond3["offsetX"].ToFloat());
               dwType1Pond3AndSomeSubdata.writeFloat(pond3["offsetY"].ToFloat());
               dwType1Pond3AndSomeSubdata.writeInt(pond3["unk6"].ToInt());
               dwType1Pond3AndSomeSubdata.writeInt(pond3["unk7"].ToInt());
               dwType1Pond3AndSomeSubdata.writeInt(pond3["unk8"].ToInt());
               dwType1Pond3AndSomeSubdata.writeFloat(pond3["unk9"].ToFloat());
               dwType1Pond3AndSomeSubdata.writeFloat(pond3["unk10"].ToFloat());
               dwType1Pond3AndSomeSubdata.writeFloat(pond3["unk11"].ToFloat());
            }else if(type == 2){
               dwSubDataAndPond3s.writeFloat(pond3["lifetime"].ToFloat());
               dwSubDataAndPond3s.writeFloat(pond3["unk2"].ToFloat());
               dwSubDataAndPond3s.writeInt(pond3["unk3"].ToInt());
               dwSubDataAndPond3s.writeFloat(pond3["unk4"].ToFloat());
               dwSubDataAndPond3s.writeByte(pond3["unk5"].ToInt());
               dwSubDataAndPond3s.writeByte(pond3["unk6"].ToInt());
               dwSubDataAndPond3s.writeShort(0);
            }else if(type == 3){
               dwSubDataAndPond3s.writeFloat(pond3["unk1"].ToFloat());
               dwSubDataAndPond3s.writeInt(pond3["unk2"].ToInt());
               dwSubDataAndPond3s.writeFloat(pond3["unk3"].ToFloat());
               dwSubDataAndPond3s.writeInt(pond3["unk4"].ToInt());
            }else if(type == 4){
               dwSubDataAndPond3s.writeInt(pond3["unk1"].ToInt());
            }else if(type == 5){
               dwSubDataAndPond3s.writeInt(pond3["unk1"].ToInt());
            }else if(type == 6){
               dwSubDataAndPond3s.writeFloat(pond3["unk1"].ToFloat());
               dwSubDataAndPond3s.writeInt(pond3["unk2"].ToInt());
            }else if(type == 7){
               dwSubDataAndPond3s.writeInt(pond3["unk1"].ToInt());
            }else{
               swprintf(wBuffer, sizeof(wBuffer), L"Pond3 type %d umfamiliar", type);
               jsonReadError(ffxPath, wBuffer);
            }
         }
      }else if(astType == 2){
         DataWriter* dwPond2 = new DataWriter(isRemaster);
         DataWriter* dwPreDataNumbers = new DataWriter(isRemaster);
         DataWriter* dwPreDataSubtypes = new DataWriter(isRemaster);
         DataWriter* dwSubtypeData = new DataWriter(isRemaster);
         dwPond2Stuff.push_back(dwPond2);
         dwPond2Stuff.push_back(dwPreDataNumbers);
         dwPond2Stuff.push_back(dwPreDataSubtypes);
         dwPond2Stuff.push_back(dwSubtypeData);

         if(isRemaster){
            dwPreDataSubtypes->padToMultiple = 8;
         }
         dwSubtypeData->padToMultiple = 16;

         dw.addOffsetToFixAt(posPond2Offset, *dwPond2, 0);

         json::JSON& pond2 = ast["pond2"];
         int pond2Type = pond2["pond2Type"].ToInt();
         dwPond2->writeInt(pond2Type);
         dwPond2->writeInt(-1); // total size
         dwPond2->writeLong(pond2["preData"].size());
         dwPond2->writeOffsetToFix(*dwPreDataNumbers, 0);
         dwPond2->writeOffsetToFix(*dwPreDataSubtypes, 0);
         dwPond2->writeOffsetToFix(dw, astOffset);

         auto writeSubtype = [&](DataWriter& dw, json::JSON& obj) -> void{
            int subtype = obj["subtypeType"].ToInt();
            dw.writeLong(subtype);

            if(subtype == 28){
               dw.writeLong(0);
            }else{
               dw.writeOffsetToFix(*dwSubtypeData, dwSubtypeData->bytes.size());
            }

            bool isCurve = (
               subtype == 0 || subtype == 4 || subtype == 5 || subtype == 6 || subtype == 7 ||
               subtype == 8 || subtype == 9 || subtype == 12 || subtype == 16 || subtype == 17 ||
               subtype == 20
            );
            if(isCurve){
               bool hasRange = subtype == 5 || subtype == 7 || subtype == 9 || subtype == 17;
               bool hasPreDataIndex = subtype == 6 || subtype == 7;
               int numArrays = subtype == 8 || subtype == 9 || subtype == 20 ? 4 : 2;

               dwSubtypeData->writeInt(obj["times"].size());

               for(json::JSON& t : obj["times"].ArrayRange()){
                  dwSubtypeData->writeFloat(t.ToFloat());
               }

               if(numArrays == 2){
                  for(json::JSON& v : obj["values"].ArrayRange()){
                     dwSubtypeData->writeFloat(v.ToFloat());
                  }
               }else{
                  for(const json::JSON& values : {obj["values1"], obj["values2"], obj["values3"]}){
                     for(const json::JSON& v : values.ArrayRange()){
                        dwSubtypeData->writeFloat(v.ToFloat());
                     }
                  }
               }

               if(hasRange){
                  dwSubtypeData->writeFloat(obj["range"][0].ToFloat());
                  dwSubtypeData->writeFloat(obj["range"][1].ToFloat());
               }

               if(hasPreDataIndex){
                  dwSubtypeData->writeInt(obj["preDataIndex"].ToInt());
               }
            }else if(subtype == 24){
               dwSubtypeData->writeFloat(obj["constant"].ToFloat());
            }else if(subtype == 25){
               dwSubtypeData->writeFloat(obj["base"].ToFloat());
               dwSubtypeData->writeFloat(obj["unknownA"].ToFloat());
               dwSubtypeData->writeFloat(obj["unknownB"].ToFloat());
            }else if(subtype == 26){
               dwSubtypeData->writeFloat(obj["unk1"].ToFloat());
               dwSubtypeData->writeInt(obj["unk2"].ToInt());
            }else if(subtype == 27){
               dwSubtypeData->writeFloat(obj["unk1"].ToFloat());
               dwSubtypeData->writeFloat(obj["unk2"].ToFloat());
               dwSubtypeData->writeFloat(obj["unk3"].ToFloat());
               dwSubtypeData->writeInt(obj["preDataIndex"].ToInt());
            }else if(subtype == 28){
               // Empty
            }else{
               swprintf(wBuffer, sizeof(wBuffer), L"Pond2 subtype %d unfamiliar", subtype);
               jsonReadError(ffxPath, wBuffer);
            }
         };

         for(json::JSON& preData : pond2["preData"].ArrayRange()){
            dwPreDataNumbers->writeInt(preData["number"].ToInt());
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
            dwPond2->writeInt(0);
         };
         auto writeInt = [&]() -> void{
            dwPond2->writeInt(vars[varIndex++].ToInt());
         };
         auto writeLong = [&]() -> void{
            dwPond2->writeLong(vars[varIndex++].ToInt());
         };
         auto writeFloat = [&]() -> void{
            dwPond2->writeFloat(vars[varIndex++].ToFloat());
         };
         auto writeSubtypes = [&](int subtypeCount) -> void{
            if(isRemaster){
               dwPond2->writePadding(8);
            }

            for(int n = 0; n < subtypeCount; ++n){
               writeSubtype(*dwPond2, vars[varIndex++]);
            }
         };
         // Fromsoft... are you ok, do you need a hug
         auto writeSubtypesWithDataOrder = [&](std::vector<int> subtypeOrder) -> void{
            // subtypeOrder index: subtype *data* order
            // subtypeOrder value: subtype index

            if(isRemaster){
               dwPond2->writePadding(8);
            }

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

               int subtypeOffset = pond2OldSize + subtypeIndex * longSize * 2;
               int type = dwPond2->readInt(subtypeOffset + 0);

               if(type != 28){
                  dwPond2->replaceOffsetToFix(subtypeOffset + longSize, *dwSubtypeData, dwSubtypeData->bytes.size());
                  dwSubtypeData->bytes.insert(
                     dwSubtypeData->bytes.end(),
                     subtypeDataCopy.begin() + subtypeDataSlices[subtypeIndex].first,
                     subtypeDataCopy.begin() + subtypeDataSlices[subtypeIndex].second
                  );
               }
            }
         };

         auto writeRemasterThing = [&]() -> void{
            if(!isRemaster) return;

            writeSubtypesWithDataOrder({0, 1, 2, 4, 3});
            writeFloat();
            writeInt();
            writeInt();
            writeInt();
            writeInt();
            writeInt();
         };

         if(pond2Type == 27){
            writeFloat();
            writeFloat();
            writeFloat();
            writeZero();
            writeFloat();
            if(isRemaster) writeZero();
            writeInt(); // texId
            writeInt();
            writeZero();
            writeSubtypes(10);
            writeInt();
            writeInt();
            writeInt();
            writeFloat();
            writeRemasterThing();
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
            writeLong();
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
            writeInt(); // texId
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
            writeRemasterThing();
         }else if(pond2Type == 43){
            writeFloat();
            writeZero();
            writeZero();
            writeInt(); // texId
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
            writeFloat();
            writeZero();
            writeInt(); // texId
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
            writeRemasterThing();
         }else if(pond2Type == 61){
            writeZero();
            writeZero();
            writeInt(); // modelId
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
            writeRemasterThing();
         }else if(pond2Type == 66){
            writeFloat();
            writeFloat();
            writeZero();
            writeInt();
            writeFloat();
            writeInt();
            if(isRemaster){
               writeLong();
            }
            writeSubtypes(26);
            writeZero();
            writeRemasterThing();
         }else if(pond2Type == 70){
            writeFloat();
            writeFloat();
            writeFloat();
            writeInt();
            writeFloat();
            if(isRemaster) writeZero();
            writeInt(); // texId1
            writeInt(); // texId2
            writeInt(); // texId3
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
            if(isRemaster){
               writeZero();
               writeZero();
               writeZero();
               writeZero();
            }
            writeInt();
            writeInt();
            writeInt();
            writeFloat();
            writeInt();
            writeFloat();
            writeInt();
            writeRemasterThing();
         }else if(pond2Type == 71){
            writeFloat();
            writeFloat();
            writeFloat();
            writeInt();
            writeFloat();
            if(isRemaster) writeZero();
            writeInt(); // texID
            writeInt();
            writeInt();
            writeInt();
            writeInt();
            writeSubtypes(10);
            writeInt();
            writeInt();
            writeSubtypesWithDataOrder({2, 3, 4, 5, 6, 7, 8, 9, 0, 1});
            if(isRemaster){
               writeInt();
               writeInt();
               writeInt();
               writeInt();
            }
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
            writeRemasterThing();
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
            writeLong();
            writeSubtypes(1);
         }else if(pond2Type == 107){
            writeFloat();
            writeZero();
            writeInt(); // texId
            writeInt();
            writeSubtypes(7);
         }else if(pond2Type == 108){
            writeFloat();
            writeFloat();
            writeFloat();
            writeInt();
            writeFloat();
            if(isRemaster) writeZero();
            writeInt(); // modelId
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
            writeRemasterThing();
         }else if(pond2Type == 117){
            writeSubtypes(6);
            writeInt();
            writeInt();
            writeSubtypes(1);
         }else{
            jsonReadError(ffxPath, L"pond2 unknown pond2Type");
         }

         int pond2Size = (
            dwPond2->bytes.size() +
            dwPreDataNumbers->bytes.size() +
            dwPreDataSubtypes->bytes.size() +
            dwSubtypeData->bytes.size()
         );
         dwPond2->writeIntAt(4, pond2Size);
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


   dwMain.writeByte('F');
   dwMain.writeByte('X');
   dwMain.writeByte('R');
   dwMain.writeByte('\0');

   dwMain.writeShort(0);
   dwMain.writeShort(1);
   offsetList.push_back(dwMain.bytes.size());
   dwMain.writeLong(isRemaster ? 48 : 32); // dataStartAfterHeader
   dwMain.writeInt(-1); // data2Start
   dwMain.writeInt(-1); // data2Count
   dwMain.writeInt(-1); // data3Count
   dwMain.writeInt(root["unk1"].ToInt());
   dwMain.writeInt(root["unk2"].ToInt());
   dwMain.writePadding(16);

   int t133Count = root["type133s"].size();
   bool needsType134 = t133Count > 1;
   if(needsType134){
      firstData3Offsets.push_back(dwMain.bytes.size());

      dwMain.writeLong(134);
      dwMain.writeLong(root["ffxId"].ToInt());
      dwMain.writeLong(0);

      DataWriter* dwType133OffsetArray = new DataWriter(isRemaster);
      dwPond1Arrays.emplace_back(dwType133OffsetArray);

      dwMain.writeOffsetToFix(*dwType133OffsetArray, 0);
      dwMain.writeLong(t133Count);

      int t133Size = isRemaster ? 176 : 96;
      for(int n = 0; n < t133Count; ++n){
         int t133Offset = dwMain.bytes.size() + t133Size * n;
         dwType133OffsetArray->writeOffsetToFix(dwMain, t133Offset);
      }
   }
   
   for(json::JSON& type133 : root["type133s"].ArrayRange()){
      firstData3Offsets.push_back(dwMain.bytes.size());

      dwMain.writeLong(133);
      dwMain.writeLong(root["ffxId"].ToInt());
      for(int n = 0; n < 7; ++n){
         dwMain.writeLong(0);
      }
      dwMain.writeLong(type133["always8Or10"].ToInt());
      json::JSON& preAST1 = type133["preAST1"];
      json::JSON& preAST2 = type133["preAST2"];
      writeAST(dwMain, preAST1);
      writeAST(dwMain, preAST2);
      dwMain.writeOffsetToFix(dwHouses, dwHouses.bytes.size());
      dwMain.writeLong(type133["houses"].size());

      json::JSON& houses = type133["houses"];

      for(int h = 0; h < houses.size(); ++h){
         json::JSON& house = houses[h];
         int linkCount = house["links"].size();
         int blossomCount = house["blossoms"].size();

         house["_offset"] = dwHouses.bytes.size();
         if(linkCount > 0){
            dwHouses.writeLong(-1);
         }else{
            dwHouses.writeLong(0);
         }

         if(blossomCount > 0){
            dwHouses.writeOffsetToFix(dwBlossoms, dwBlossoms.bytes.size());
         }else{
            dwHouses.writeLong(0);
         }

         dwHouses.writeInt(linkCount);
         dwHouses.writeInt(blossomCount);

         for(json::JSON& blossom : house["blossoms"].ArrayRange()){
            dwBlossoms.writeLong(blossom["blossomType"].ToInt());
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
   std::vector<DataWriter*> dataWriters;
   if(isRemaster){
      if(dwSubDataAndPond3s.bytes.size() > 0){
         dwSubDataAndPond3s.padToMultiple = 8;
      }
      if(dwData3s.bytes.size() > 0){
         dwData3s.padToMultiple = 16;
      }

      dataWriters.insert(
         dataWriters.end(),
         {
            &dwMainASTs, &dwHouses, &dwLinks, &dwBlossoms,
            &dwSubDataAndPond3s, &dwLinkData3s
         }
      );
      dataWriters.insert(dataWriters.end(), dwPond1Arrays.begin(), dwPond1Arrays.end());
      dataWriters.insert(dataWriters.end(), {&dwData3s, &dwType1Pond3AndSomeSubdata});
      dataWriters.insert(dataWriters.end(), dwPond2Stuff.begin(), dwPond2Stuff.end());
   }else{
      dataWriters.insert(
         dataWriters.begin(),
         {
            &dwMainASTs, &dwHouses, &dwLinks, &dwBlossoms,
            &dwSubDataAndPond3s, &dwData3s, &dwLinkData3s
         }
      );
      dataWriters.insert(dataWriters.end(), dwPond1Arrays.begin(), dwPond1Arrays.end());
      dataWriters.push_back(&dwType1Pond3AndSomeSubdata);
      dataWriters.insert(dataWriters.end(), dwPond2Stuff.begin(), dwPond2Stuff.end());
   }
   dataWriters.back()->padToMultiple = 16;
   dwMain.merge(dataWriters, offsetList);

   // Write offsetList
   dwMain.writeIntAt(isRemaster ? 16 : 12, dwMain.bytes.size());
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
      dwMain.writeLong(offset);
   }
   dwMain.writeIntAt(isRemaster ? 20 : 16, offsetList.size());

   // Write data3Offsets
   for(int offset : firstData3Offsets){
      dwMain.writeLong(offset);
   }
   std::sort(data3Offsets.begin(), data3Offsets.end());
   for(int offset : data3Offsets){
      dwMain.writeLong(dwData3s.finalOffset + offset);
   }
   for(int offset : linkData3Offsets){
      dwMain.writeLong(dwLinkData3s.finalOffset + offset);
   }
   dwMain.writeIntAt(isRemaster ? 24 : 20, firstData3Offsets.size() + data3Offsets.size() + linkData3Offsets.size());

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
