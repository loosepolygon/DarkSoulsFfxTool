#include "header.hpp"

void ffxToJson(const std::wstring& ffxPath, const std::wstring& jsonPath, const TestFunctions& testFunctions){
   DataReader dr;
   //dr.bpOnRead = 4;
   {
      FILE* file = _wfopen(ffxPath.c_str(), L"rb");
      if(file == NULL) {
         ffxReadError(ffxPath, L"Cannot open file");
         return;
      }

      fseek(file, 0, SEEK_END);
      long fileSize = ftell(file);
      fseek(file, 0, SEEK_SET);

      dr.bytes.resize(fileSize);
      fread(dr.bytes.data(), 1, dr.bytes.size(), file);

      dr.bytesRead.resize(dr.bytes.size());

      fclose(file);
   }

   char sBuffer[200];
   wchar_t wBuffer[200];

   json::JSON root;
   root["jsonVersion"] = ::jsonVersion;
   root["gameVersion"] = "";
   root["unk1"] = -1;
   root["unk2"] = -1;
   root["ffxId"] = -1;
   root["type133s"] = json::Array();
   root["templateASTs"] = json::Array();

   int lastPond1Data3Offset = 0;

   std::function<json::JSON(int)> readData3;
   std::function<json::JSON(int)> readAST;

   readData3 = [&](int addr) -> json::JSON{
      json::JSON data3 = json::Object();

      int type = dr.readInt(addr);
      data3["data3Type"] = type;

      if(type == 112 || type == 113 || type == 129 || type == 130 || type == 131 || type == 132 || type == 136){
         // empty
      }else if(type == 1 || type == 59 || type == 66 || type == 68 || type == 111 || type == 138 || type == 139){
         data3["unk1"] = dr.readInt(addr + 4);
      }else if(type == 7 || type == 70){
         data3["unk1"] = dr.readFloat(addr + 4);
      }else if(type == 79 || type == 140){
         data3["unk1"] = dr.readInt(addr + 4);
         data3["unk2"] = dr.readInt(addr + 8);
      }else if(type == 81 || type == 85){
         data3["unk1"] = dr.readFloat(addr + 4);
         data3["unk2"] = dr.readFloat(addr + 8);
      }else if(type == 137){
         data3["unk1"] = dr.readInt(addr + 4);
         data3["unk2"] = dr.readInt(addr + 8);
         data3["unk3"] = dr.readInt(addr + 12);
      }else if(type == 2){
         int offset = dr.readInt(addr + 4);
         int count = dr.readInt(addr + 8);

         json::JSON& bunchaInts = data3["bunchaInts"] = json::Array();
         for(int n = 0; n < count; ++n){
            bunchaInts.append(dr.readInt(offset + n * 4));
         }
      }else if(type == 3 || type == 5 || type == 6 || type == 9){
         int offsetToFloats = dr.readInt(addr + 4);
         int offsetToInts = dr.readInt(addr + 8);
         int count = dr.readInt(addr + 12);
         
         data3["note"] = "counts must match";
         data3["floats"] = json::Array();
         data3["ints"] = json::Array();
         json::JSON& floats = data3["floats"];
         json::JSON& ints = data3["ints"];

         for(int n = 0; n < count; ++n){
            floats.append(dr.readFloat(offsetToFloats + n * 4));
            ints.append(dr.readInt(offsetToInts + n * 4));
         }
      }else if(type == 89){
         int offsetToFloats = dr.readInt(addr + 4);
         int offsetToInts = dr.readInt(addr + 8);
         int count = dr.readInt(addr + 12);

         data3["note"] = "counts must match; unk1 and unk2 are always 1 and 0";
         data3["floats"] = json::Array();
         data3["ints"] = json::Array();
         json::JSON& floats = data3["floats"];
         json::JSON& ints = data3["ints"];

         for(int n = 0; n < count; ++n){
            floats.append(dr.readFloat(offsetToFloats + n * 4));
            ints.append(dr.readInt(offsetToInts + n * 4));
         }

         data3["unk1"] = dr.readInt(addr + 16);
         data3["unk2"] = dr.readInt(addr + 20);
      }else if(type == 11 || type == 12){
         int offsetA = dr.readInt(addr + 4);
         int offsetB = dr.readInt(addr + 8);
         int count = dr.readInt(addr + 12);

         data3["note"] = "counts must match";
         data3["floatsA"] = json::Array();
         data3["floatsB"] = json::Array();
         json::JSON& floatsA = data3["floatsA"];
         json::JSON& floatsB = data3["floatsB"];

         for(int n = 0; n < count; ++n){
            floatsA.append(dr.readFloat(offsetA + n * 4));
            floatsB.append(dr.readFloat(offsetB + n * 4));
         }
      }else if(type == 13 || type == 14){
         int offsetA = dr.readInt(addr + 4);
         int offsetB = dr.readInt(addr + 8);
         int count = dr.readInt(addr + 12);

         data3["note"] = "floatsB count is floatsA count x3";
         data3["floatsA"] = json::Array();
         data3["floatsB"] = json::Array();
         json::JSON& floatsA = data3["floatsA"];
         json::JSON& floatsB = data3["floatsB"];

         for(int n = 0; n < count; ++n){
            floatsA.append(dr.readFloat(offsetA + n * 4));
            floatsB.append(dr.readFloat(offsetB + n * 3 * 4 + 0));
            floatsB.append(dr.readFloat(offsetB + n * 3 * 4 + 4));
            floatsB.append(dr.readFloat(offsetB + n * 3 * 4 + 8));
         }
      }else if(type == 19 || type == 20){
         int offsetA = dr.readInt(addr + 4);
         int offsetB = dr.readInt(addr + 8);
         int count = dr.readInt(addr + 12);

         data3["note"] = "floatsB count is floatsA count x4";
         data3["floatsA"] = json::Array();
         data3["floatsB"] = json::Array();
         json::JSON& floatsA = data3["floatsA"];
         json::JSON& floatsB = data3["floatsB"];

         for(int n = 0; n < count; ++n){
            floatsA.append(dr.readFloat(offsetA + n * 4));
            floatsB.append(dr.readFloat(offsetB + n * 4 * 4 + 0));
            floatsB.append(dr.readFloat(offsetB + n * 4 * 4 + 4));
            floatsB.append(dr.readFloat(offsetB + n * 4 * 4 + 8));
            floatsB.append(dr.readFloat(offsetB + n * 4 * 4 + 12));
         }
      }else if(type == 91 || type == 95){
         int offsetA = dr.readInt(addr + 4);
         int offsetB = dr.readInt(addr + 8);
         int count = dr.readInt(addr + 12);

         data3["note"] = "counts must match; unk1 always 1";
         data3["floatsA"] = json::Array();
         data3["floatsB"] = json::Array();
         json::JSON& floatsA = data3["floatsA"];
         json::JSON& floatsB = data3["floatsB"];

         for(int n = 0; n < count; ++n){
            floatsA.append(dr.readFloat(offsetA + n * 4));
            floatsB.append(dr.readFloat(offsetB + n * 4));
         }

         data3["unk1"] = dr.readInt(addr + 16);
         int data3Offset = dr.readInt(addr + 20);
         data3["data3"] = readData3(data3Offset);
      }else if(type == 37){
         int templateFfxId = dr.readInt(addr + 4);
         data3["templateFfxId"] = templateFfxId;
         int astOffset = dr.readInt(addr + 8);
         if(astOffset){
            int templateASTIndex = root["templateASTs"].size();
            data3["templateASTIndex"] = templateASTIndex;
            json::JSON& newAST = root["templateASTs"][templateASTIndex] = json::Object();
            newAST = readAST(astOffset);
            char sBuffer[80];
            sprintf(sBuffer, ", ffx = %d, index = %d", templateFfxId, templateASTIndex);
            newAST["note1"] = newAST["note1"].ToString() + sBuffer;

            testFunctions.onAST(
               newAST,
               {data3, root},
               TestFunctions::Type37,
               data3["templateFfxId"].ToInt()
            );
         }else{
            data3["templateASTIndex"] = -1;
         }

         dr.readInt(addr + 12);
      }else if(type == 38){
         int t38Subtype = dr.readInt(addr + 4);
         data3["t38Subtype"] = t38Subtype;
         int astOffset = dr.readInt(addr + 8);
         if(astOffset){
            data3["ast"] = readAST(astOffset);
            testFunctions.onAST(
               data3["ast"],
               {data3, root},
               TestFunctions::Type38,
               t38Subtype
            );
         }else{
            data3["ast"] = {};
         }

         dr.readInt(addr + 12);
      }else if(type == 44 || type == 45 || type == 46 || type == 47 || type == 60 || type == 71 || type == 87 || type == 114 || type == 115){
         int all = dr.readInt(addr + 4);
         short* sp = reinterpret_cast<short*>(&all);
         data3["unk1"] = sp[0];
         data3["unk2"] = sp[1];
      }else if(type == 128){
         int data3Offset = dr.readInt(addr + 4);

         data3["data3"] = readData3(data3Offset);
      }else if(type == 120 || type == 121 || type == 122 || type == 123 || type == 124 || type == 126 || type == 127){
         int data3OffsetA = dr.readInt(addr + 4);
         int data3OffsetB = dr.readInt(addr + 8);

         data3["data3A"] = readData3(data3OffsetA);
         data3["data3B"] = readData3(data3OffsetB);
      }else{
         swprintf(wBuffer, sizeof(wBuffer), L"Data3 type %d unfamiliar", type);
         ffxReadError(ffxPath, wBuffer);
      }

      testFunctions.onData3(data3, {root, root});

      return std::move(data3);
   };

   DataReader drP; // pond2
   readAST = [&](int addr) -> json::JSON{
      int startAddress = addr == -1 ? dr.marker : addr;

      int pond1Offset = dr.readInt(startAddress);
      int data3Count = dr.readInt();
      int data3Count2 = dr.readInt();
      bool flag1 = (bool)dr.readByte();
      bool flag2 = (bool)dr.readByte();
      bool flag3 = (bool)dr.readByte();
      dr.readByte();
      int pond2Offset = dr.readInt();
      int pond3Offset = dr.readInt();

      json::JSON ast = json::Object();
      sprintf(sBuffer, "offset = %d", startAddress);
      ast["note1"] = sBuffer;
      ast["note2"] = "";
      if(pond1Offset){
         ast["astType"] = 1;
         sprintf(sBuffer, "AST Type %d: %s", 1, "Unknown, has Data3s and optional struct");
      }else if(pond2Offset){
         ast["astType"] = 2;
         sprintf(sBuffer, "AST Type %d: %s", 2, "Sprite/model emission and data curves");
      }else{
         ast["astType"] = 0;
         sprintf(sBuffer, "AST Type %d: %s", 0, "Empty");
      }
      ast["note2"] = sBuffer;
      ast["flag1"] = flag1;
      ast["flag2"] = flag2;
      ast["flag3"] = flag3;
      

      if(pond1Offset){
         json::JSON& data3s = ast["pond1Data3s"] = json::Array();
         for(int n = 0; n < data3Count; ++n){
            int offsetToData3 = dr.readInt(pond1Offset + n * 4);
            data3s.append(std::move(readData3(offsetToData3)));

            // For checking all bytes are read
            if(pond1Offset + n * 4 > lastPond1Data3Offset){
               lastPond1Data3Offset = pond1Offset + n * 4;
            }
         }

         testFunctions.onPond1(data3s, {ast, root});

         json::JSON& pond3 = ast["pond3"] = json::Object();
         if(pond3Offset){
            int type = dr.readInt(pond3Offset + 0);
            pond3["pond3Type"] = type;
            if(type == 0){
               pond3["astCountMaybe"] = dr.readInt(pond3Offset + 4);
            }else if(type == 1){
               pond3["unk1"] = dr.readInt(pond3Offset + 4);
               pond3["unk2"] = dr.readInt(pond3Offset + 8);
               pond3["unk3"] = dr.readInt(pond3Offset + 12);
               pond3["offsetX"] = dr.readFloat(pond3Offset + 16);
               pond3["offsetY"] = dr.readFloat(pond3Offset + 20);
               pond3["unk6"] = dr.readInt(pond3Offset + 24);
               pond3["unk7"] = dr.readInt(pond3Offset + 28);
               pond3["unk8"] = dr.readInt(pond3Offset + 32);
               pond3["unk9"] = dr.readFloat(pond3Offset + 36);
               pond3["unk10"] = dr.readFloat(pond3Offset + 40);
               pond3["unk11"] = dr.readFloat(pond3Offset + 44);
            }else if(type == 2){
               pond3["lifetime"] = dr.readFloat(pond3Offset + 4);
               pond3["unk2"] = dr.readFloat(pond3Offset + 8);
               pond3["unk3"] = dr.readInt(pond3Offset + 12);
               pond3["unk4"] = dr.readFloat(pond3Offset + 16);
               pond3["unk5"] = dr.readByte(pond3Offset + 20);
               pond3["unk6"] = dr.readByte(pond3Offset + 21);
               dr.readShort(pond3Offset + 22);
            }else if(type == 3){
               pond3["unk1"] = dr.readFloat(pond3Offset + 4);
               pond3["unk2"] = dr.readInt(pond3Offset + 8);
               pond3["unk3"] = dr.readFloat(pond3Offset + 12);
               pond3["unk4"] = dr.readInt(pond3Offset + 16);
            }else if(type == 4){
               pond3["unk1"] = dr.readInt(pond3Offset + 4);
            }else if(type == 5){
               pond3["unk1"] = dr.readInt(pond3Offset + 4);
            }else if(type == 6){
               pond3["unk1"] = dr.readFloat(pond3Offset + 4);
               pond3["unk2"] = dr.readInt(pond3Offset + 8);
            }else if(type == 7){
               pond3["unk1"] = dr.readInt(pond3Offset + 4);
            }else{
               wchar_t wBuffer[80];
               swprintf(wBuffer, sizeof(wBuffer), L"Pond3 type %d umfamiliar, addr = %d\n", type, pond3Offset);
               ffxReadError(ffxPath, wBuffer);
            }

            testFunctions.onPond3(pond3, {ast, root});
         }
      }

      if(pond2Offset){
         int currentDataOffset = 0;
         int address = pond2Offset;
         json::JSON* currentObject = nullptr;

         auto readZero = [&]() -> void{
            drP.readInt(currentDataOffset);
            currentDataOffset += 4;
         };
         auto readInt = [&](const char* name = nullptr) -> int{
            if(name == nullptr){
               sprintf(sBuffer, "unk%dInt", currentObject->size() - 1);
               name = sBuffer;
            };

            int result = drP.readInt(currentDataOffset);
            (*currentObject)[name] = result;

            currentDataOffset += 4;

            return result;
         };
         auto readFloat = [&](const char* name = nullptr) -> float{
            if(name == nullptr){
               sprintf(sBuffer, "unk%dFloat", currentObject->size() - 1);
               name = sBuffer;
            };

            float result = drP.readFloat(currentDataOffset);
            (*currentObject)[name] = result;

            currentDataOffset += 4;

            return result;
         };
         auto readSubtype = [&](const char* name = nullptr, int readAtLocalOffsetInstead = 0) -> void{
            if(readAtLocalOffsetInstead == 0){
               readAtLocalOffsetInstead = currentDataOffset;
               currentDataOffset += 8;
            }
            int subtype = drP.readInt(readAtLocalOffsetInstead + 0);
            int fullOffset = drP.readInt(readAtLocalOffsetInstead + 4);
            int offset = fullOffset - address - 24;

            if(name == nullptr){
               sprintf(sBuffer, "unk%dSubtype%d", currentObject->size() - 1, subtype);
               name = sBuffer;
            };
            json::JSON& obj = (*currentObject)[name] = json::Object();
            obj["subtypeType"] = subtype;

            bool isCurve = (
               subtype == 0 || subtype == 4 || subtype == 5 || subtype == 6 || subtype == 7 ||
               subtype == 8 || subtype == 9 || subtype == 12 || subtype == 16 || subtype == 17
            );
            if(isCurve){
               bool hasRange = subtype == 5 || subtype == 7 || subtype == 9 || subtype == 17;
               bool hasPreDataIndex = subtype == 6 || subtype == 7;
               int numArrays = subtype == 8 || subtype == 9 ? 4 : 2;
               const char* curveType = "unknown";
               if(subtype == 0 || subtype == 12) curveType = "constant";
               bool repeats = subtype == 12 || subtype == 16;

               sprintf(
                  sBuffer,
                  "Subtype %d: Curve type: %s; Repeat: %s",
                  subtype, curveType, repeats ? "true" : "false"
               );
               obj["note"] = sBuffer;

               int arraySize = drP.readInt(offset);
               offset += 4;

               if(hasRange){
                  obj["range"] = json::Array();
               }

               obj["times"] = json::Array();
               json::JSON& times = obj["times"];
               for(int n = 0; n < arraySize; ++n){
                  obj["times"].append(drP.readFloat(offset));
                  offset += 4;
               }

               if(numArrays == 2){
                  obj["values"] = json::Array();
                  json::JSON& values = obj["values"];
                  for(int n = 0; n < arraySize; ++n){
                     values.append(drP.readFloat(offset));
                     offset += 4;
                  }
               }else{
                  obj["values1"] = json::Array();
                  obj["values2"] = json::Array();
                  obj["values3"] = json::Array();
                  json::JSON& values1 = obj["values1"];
                  json::JSON& values2 = obj["values2"];
                  json::JSON& values3 = obj["values3"];
                  for(int n = 0; n < arraySize; ++n){
                     values1.append(drP.readFloat(arraySize*4*0 + offset));
                     values2.append(drP.readFloat(arraySize*4*1 + offset));
                     values3.append(drP.readFloat(arraySize*4*2 + offset));
                     offset += 4;
                  }
                  offset += arraySize * 4 * 2;
               }

               if(hasRange){
                  obj["range"].append(drP.readFloat(offset + 0));
                  obj["range"].append(drP.readFloat(offset + 4));

                  offset += 8;
               }

               if(hasPreDataIndex){
                  obj["preDataIndex"] = drP.readInt(offset);
               }
            }else if(subtype == 24){
               obj["constant"] = drP.readFloat(offset + 0);
            }else if(subtype == 25){
               obj["base"] = drP.readFloat(offset + 0);
               obj["unknownA"] = drP.readFloat(offset + 4);
               obj["unknownB"] = drP.readFloat(offset + 8);
            }else if(subtype == 26){
               obj["unk1"] = drP.readFloat(offset + 0);
               obj["unk2"] = drP.readInt(offset + 4);
            }else if(subtype == 27){
               obj["unk1"] = drP.readFloat(offset + 0);
               obj["unk2"] = drP.readFloat(offset + 4);
               obj["unk3"] = drP.readFloat(offset + 8);
               obj["preDataIndex"] = drP.readInt(offset + 12);
            }else if(subtype == 28){
               // Empty, offset is 0
            }else{
               wchar_t wBuffer[80];
               swprintf(wBuffer, sizeof(wBuffer), L"Pond2 subtype %d unfamiliar, addr = %d\n", subtype, fullOffset);
               ffxReadError(ffxPath, wBuffer);
            }

            testFunctions.onPond2Subtype(obj, {*currentObject, root});
         };
         auto readSubtypes = [&](int count) -> void{
            for(int n = 0; n < count; ++n){
               readSubtype();
            }
         };


         json::JSON& pond2 = ast["pond2"] = json::Object();
         int type = dr.readInt(pond2Offset + 0);
         pond2["pond2Type"] = type;
         pond2["note"] = "";
         // Round to padding
         int totalSize = (dr.readInt(pond2Offset + 4) + 15) / 16 * 16;
         int preDataCount = dr.readInt(pond2Offset + 8);
         int offsetToPreDataNumbers = dr.readInt(pond2Offset + 12);
         int offsetToPreDataSubtypes = dr.readInt(pond2Offset + 16);
         int offsetToSourceAST = dr.readInt(pond2Offset + 20);

         drP.bytes.resize(0);
         for(int n = 24; n < totalSize; ++n){
            drP.bytes.push_back(dr.readByte(pond2Offset + n));
         }
         drP.bytesRead.resize(0);
         drP.bytesRead.resize(drP.bytes.size());

         // Pre data
         json::JSON& preData = pond2["preData"] = json::Array();
         if(preDataCount > 0){
            int localNumbersOffset = offsetToPreDataNumbers - address - 24;
            int localSubtypesOffset = offsetToPreDataSubtypes - address - 24;
            for(int n = 0; n < preDataCount; ++n){
               json::JSON entry;

               entry["number"] = drP.readInt(localNumbersOffset + n * 4);
               currentObject = &entry;
               readSubtype("subtype", localSubtypesOffset + n * 8);

               preData.append(std::move(entry));
            }
         }

         currentObject = &pond2;

         // Main data
         if(type == 27){
            readFloat();
            readFloat();
            readFloat();
            readZero();
            readZero();
            readInt("texId");
            readInt();
            readZero();
            readSubtypes(10);
            readInt();
            readInt();
            readInt();
            readFloat();
         }else if(type == 28){
            pond2["note"] = "Pond2 type 28: initial speed";
            readSubtype("transverseSpeed");
            readSubtype();
            readSubtype("forwardSpeed");
            readInt();
         }else if(type == 29){
            readSubtypes(5);
            readInt();
         }else if(type == 30){
            readSubtypes(4);
            readFloat();
            readInt();
            readInt();
         }else if(type == 31){
            readSubtypes(4);
            readInt();
            readInt();
         }else if(type == 32){
            readSubtype("duplicateOffsetX");
            readSubtype("duplicateOffsetY");
            readSubtype("duplicateOffsetZ");
            readSubtype();
            readSubtype();
            readSubtype("weirdPerpendicularDuplicateAmount");
            readInt();
            readInt();
         }else if(type == 40){
            readZero();
            readFloat();
            readInt("texId");
            readZero();
            readInt();
            readInt();
            readInt();
            readSubtypes(4);
            readFloat();
            readFloat();
            readInt();
            readInt();
            readZero();
            readSubtypes(4);
            readInt();
            readInt();
            readZero();
            readSubtypes(1);
            readInt();
            readFloat();
            readSubtypes(2);
            readInt();
         }else if(type == 43){
            readFloat();
            readZero();
            readZero();
            readInt("texId");
            readInt();
            readInt();
            readInt();
            readInt();
            readInt();
            readSubtypes(13);
            readInt();
         }else if(type == 55){
            readSubtypes(3);
            readZero();
            readFloat();
         }else if(type == 59){
            readZero();
            readZero();
            readInt("texId");
            readZero();
            readInt();
            readInt();
            readSubtypes(5);
            readInt();
            readInt();
            readSubtypes(8);
            readInt();
            readInt();
            readZero();
            readInt();
            readFloat();
            readZero();
         }else if(type == 61){
            readZero();
            readZero();
            readInt("modelId");
            readInt();
            readInt();
            readInt();
            readZero();
            readSubtypes(3);
            readZero();
            readZero();
            readInt();
            readFloat();
            readZero();
            readSubtypes(1);
            readInt();
            readInt();
            readSubtypes(10);
            readInt();
            readInt();
            readZero();
            readZero();
         }else if(type == 66){
            readFloat();
            readFloat();
            readZero();
            readInt();
            readFloat();
            readInt();
            readSubtypes(26);
            readZero();
         }else if(type == 70){
            readFloat();
            readFloat();
            readFloat();
            readInt();
            readFloat();
            readInt("texId1");
            readInt("texId2");
            readInt("texId3");
            readInt();
            readInt();
            readInt();
            readSubtypes(30);
            readInt();
            readZero();
            readZero();
            readZero();
            readZero();
            readInt();
            readInt();
            readInt();
            readFloat();
            readInt();
            readFloat();
            readInt();
         }else if(type == 71){
            readFloat();
            readFloat();
            readFloat();
            readInt();
            readFloat();
            readInt("texId");
            readZero();
            readInt();
            readInt();
            readInt();
            readSubtypes(10);
            readInt();
            readInt();
            readSubtypes(10);
            readInt();
            readZero();
            readZero();
            readZero();
            readZero();
            readInt();
            readInt();
            readInt();
            readFloat();
            readInt();
            readFloat();
            readInt();
         }else if(type == 84){
            readSubtypes(3);
            readZero();
            readFloat();
            readSubtypes(1);
            readInt();
         }else if(type == 105){
            readSubtypes(3);
            readZero();
            readFloat();
            readSubtypes(1);
            readInt();
            readSubtypes(1);
         }else if(type == 107){
            readZero();
            readZero();
            readInt("texId");
            readInt();
            readSubtypes(7);
         }else if(type == 108){
            readFloat();
            readFloat("curvesTimeMaybe");
            readFloat();
            readInt();
            readFloat();
            readInt("modelId");
            readInt();
            readInt();
            readInt();
            readZero();
            readSubtype("scaleData1X");
            readSubtype("scaleData1Y");
            readSubtype("scaleData1Z");
            readSubtype("scaleData2X");
            readSubtype("scaleData2Y");
            readSubtype("scaleData2Z");
            readSubtype("pitchSpeed");
            readSubtype("yawSpeed");
            readSubtype("rollSpeed");
            readSubtype("rotData1Maybe");
            readSubtype("rotData2Maybe");
            readSubtype("rotData3Maybe");
            readInt();
            readInt();
            readSubtypes(6);
            readSubtype("colorData1R");
            readSubtype("colorData1G");
            readSubtype("colorData1B");
            readSubtype("colorData1A");
            readSubtype("colorData2R");
            readSubtype("colorData2G");
            readSubtype("colorData2B");
            readSubtype("colorData2A");
            readInt();
            readInt();
            readInt();
            readFloat();
            readInt();
         }else{
            ffxReadError(ffxPath, L"pond2 unknown type");
         }

         testFunctions.onPond2(pond2, {ast, root});
      }

      return std::move(ast);
   };


   char sig[4];
   int sigInt = dr.readInt();
   memcpy(sig, &sigInt, 4);
   if(memcmp("FXR", sig, 4) != 0){
      ffxReadError(ffxPath, L"Not an ffx file");
   }

   int versionRaw = dr.readInt();
   int version = (int)((short*)(&versionRaw))[1];
   int dataStartAfterHeader = dr.readInt();
   int data2Start = dr.readInt();
   int data2Count = dr.readInt();
   int data3Count = dr.readInt();
   int unk1 = dr.readInt();
   int unk2 = dr.readInt();
   dr.readPadding(16);


   bool isPTD = dataStartAfterHeader == 32;
   root["gameVersion"] = isPTD ? "PTD" : "DSR";
   root["unk1"] = unk1;
   root["unk2"] = unk2;

   int astSize = isPTD ? 6*4 : 2*4 + 4*8;

   int firstData3Type = dr.readInt(dataStartAfterHeader);
   int offsetToType133Offsets;
   int type133Count;
   int ffxId;
   if(firstData3Type == 133){
      ffxId = dr.readInt(); 
      offsetToType133Offsets = 8;
      type133Count = 1;
   }else if(firstData3Type == 134){
      ffxId = dr.readInt(); 
      dr.readInt(); 
      offsetToType133Offsets = dr.readInt();
      type133Count = dr.readInt();
   }else{
      ffxReadError(ffxPath, L"First data3 is not 133 or 134");
   }

   // Read array of Type133s
   for(int n = 0; n < type133Count; ++n){
      int addr = firstData3Type == 133 ? dataStartAfterHeader : dr.readInt(offsetToType133Offsets + n * 4);

      int type = firstData3Type == 133 ? 133 : dr.readInt(addr);
      if(type != 133){
         ffxReadError(ffxPath, L"Type is not 133");
      }

      root["ffxId"] = firstData3Type == 133 ? ffxId : dr.readInt();

      for(int n = 0; n < 7; ++n){
         dr.readInt();
      }

      json::JSON t133 = json::Object();
      sprintf(sBuffer, "Type133 index = %d", n);
      t133["note"] = sBuffer;
      t133["always8Or10"] = dr.readInt();
      int oldMarker = dr.marker;
      t133["preAST1"] = {};
      t133["preAST2"] = {};
      t133["preAST1"] = readAST(-1);
      testFunctions.onAST(t133["preAST1"], {root, root}, TestFunctions::Pre1, t133["always8Or10"].ToInt());
      dr.marker = oldMarker + astSize * 1;
      t133["preAST2"] = readAST(-1);
      testFunctions.onAST(t133["preAST2"], {root, root}, TestFunctions::Pre2, t133["always8Or10"].ToInt());
      dr.marker = oldMarker + astSize * 2;

      t133["houses"] = json::Array();
      int housesOffset = dr.readInt();
      int houseCount = dr.readInt();
      int houseOffset = housesOffset;
      for(int h = 0; h < houseCount; ++h){
         int linksOffset = dr.readInt(houseOffset);
         int blossomOffset = dr.readInt();
         int linkCount = dr.readInt();
         int blossomCount = dr.readInt();
         int houseSize = dr.marker - houseOffset;
         houseOffset = dr.marker;

         json::JSON house = json::Object();
         sprintf(sBuffer, "houseIndex = %d", h);
         house["note"] = sBuffer;

         json::JSON& links = house["links"] = json::Array();
         int linkOffset = linksOffset;
         for(int l = 0; l < linkCount; ++l){
            int linkedHouseOffset = dr.readInt(linkOffset);
            int data3Offset = dr.readInt();
            linkOffset = dr.marker;

            json::JSON entry = {
               "linkedHouseIndex", (linkedHouseOffset - housesOffset) / houseSize,
               "data3", readData3(data3Offset)
            };

            links.append(std::move(entry));
         }

         json::JSON& blossoms = house["blossoms"] = json::Array();
         for(int b = 0; b < blossomCount; ++b){
            int blossomType = dr.readInt(blossomOffset);
            json::JSON entry = {
               "blossomType", blossomType,
               "blossomAST", {}
            };
            blossomOffset = dr.marker;
            entry["blossomAST"] = readAST(blossomOffset);
            testFunctions.onAST(
               entry["blossomAST"],
               {entry, root},
               TestFunctions::Blossom,
               blossomType
            );
            blossomOffset += astSize;

            blossoms.append(std::move(entry));
         }

         t133["houses"].append(std::move(house));
      }

      root["type133s"].append(std::move(t133));
   }

   // Mark some padding as read
   if(lastPond1Data3Offset != 0){
      dr.readPadding(16, lastPond1Data3Offset + 4);
   }

   // Mark padding before data2/data3 pointers as read
   {
      int lastByteRead = data2Start - 1;
      while(dr.bytesRead[lastByteRead] == false){
         lastByteRead--;
      }

      dr.readPadding(16, lastByteRead + 1);
   }

   // Mark data2 and data3 pointers as read
   {
      int offset = data2Start;
      for (int n = 0; n < data2Count + data3Count; ++n){
         dr.readInt(offset);
         offset += 4;
      }
   }

   // Test if all bytes are read exactly once
   for(size_t b = 0; b < dr.bytesRead.size(); ++b){
      if(dr.bytesRead[b] == 0){
         printf("Byte not read at 0x%X (%d)\n", b, b);
         throw;
      }else if(dr.bytesRead[b] > 1){
         printf("Byte read %d times at 0x%X (%d)\n", dr.bytesRead[b], b, b);
         throw;
      }
   }

   testFunctions.onRoot(root, {root, root});

   std::string jsonText = root.dump(0, "   ");
   FILE* file = _wfopen(jsonPath.c_str(), L"w");
   if(file == NULL) {
      jsonWriteError(jsonPath, L"Cannot open file");
      return;
   }
   fwrite(jsonText.data(), 1, jsonText.size(), file);
   fclose(file);
}
