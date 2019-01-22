#include "header.hpp"
#include "json.hpp"

void ffxToJson(const std::wstring& ffxPath, const std::wstring& jsonPath){
   DataReader dr;
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
   root["ffxVersion"] = -1;
   root["unk1"] = -1;
   root["unk2"] = -1;
   root["ffxId"] = -1;
   root["type133s"] = json::Array();
   root["mainASTs"] = json::Array();
   json::JSON& type133s = root["type133s"];
   json::JSON& mainASTs = root["mainASTs"];

   std::function<json::JSON(int)> readData3;
   std::function<json::JSON(int, json::JSON*)> readAST;

   readData3 = [&](int addr) -> json::JSON{
      json::JSON data3 = json::Object();

      int type = dr.readInt(addr);
      data3["data3Type"] = type;

      if(type == 112 || type == 113 || type == 129 || type == 130 || type == 131 || type == 132 || type == 136){
         // empty
      }else if(type == 1 || type == 44 || type == 46 || type == 59 || type == 60 || type == 66 || type == 68 || type == 71 || type == 111 || type == 115 || type == 138 || type == 139){
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
      }else if(type == 3 || type == 5 || type == 6 || type == 9 || type == 11){
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
      }else if(type == 12){
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
         data3["refFfxId"] = dr.readInt(addr + 4);
         int astOffset = dr.readInt(addr + 8);
         if(astOffset){
            data3["mainASTIndex"] = readAST(astOffset, nullptr);
         }else{
            data3["mainASTIndex"] = -1;
         }
      }else if(type == 38){
         data3["unk1"] = dr.readInt(addr + 4);
         int astOffset = dr.readInt(addr + 8);
         if(astOffset){
            data3["mainASTIndex"] = readAST(astOffset, nullptr);
         }else{
            data3["mainASTIndex"] = -1;
         }
      }else if(type == 45 || type == 47 || type == 87 || type == 114){
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

      return std::move(data3);
   };

   DataReader drP; // pond2
   readAST = [&](int addr, json::JSON* destObjectInstead) -> int{
      int pond1Offset = dr.readInt(addr+0);
      int pond2Offset = dr.readInt(addr+16);
      int pond3Offset = dr.readInt(addr+20);

      int mainAstIndex = -1;
      if(destObjectInstead == nullptr){
         mainAstIndex = mainASTs.size();
         json::JSON placeholder = {};
         mainASTs.append(std::move(placeholder));
      }

      json::JSON ast = json::Object();
      if(destObjectInstead){
         sprintf(sBuffer, "offset = %d", addr);
      }else{
         sprintf(sBuffer, "mainAstIndex = %d, offset = %d", mainAstIndex, addr);
      }
      ast["note1"] = sBuffer;
      ast["note2"] = "";
      if(pond1Offset){
         ast["astType"] = 1;
         sprintf(sBuffer, "AST Type %d: %s", 1, "Unknown, has Data3s and optional struct");
      }else if(pond2Offset){
         ast["astType"] = 2;
         sprintf(sBuffer, "AST Type %d: %s", 2, "Shader params and stuff?");
      }else{
         ast["astType"] = 0;
         sprintf(sBuffer, "AST Type %d: %s", 0, "Empty");
      }
      ast["note2"] = sBuffer;
      ast["flag1"] = (bool)dr.readByte(addr + 12);
      ast["flag2"] = (bool)dr.readByte(addr + 13);
      ast["flag3"] = (bool)dr.readByte(addr + 14);
      

      if(pond1Offset){
         json::JSON& data3s = ast["pond1Data3s"] = json::Array();
         int data3Count = dr.readInt(addr+4);
         for(int n = 0; n < data3Count; ++n){
            int offsetToData3 = dr.readInt(pond1Offset + n * 4);
            data3s.append(std::move(readData3(offsetToData3)));
         }

         json::JSON& pond3 = ast["pond3"] = json::Object();
         if(pond3Offset){
            int type = dr.readInt(pond3Offset + 0);
            pond3["pond3Type"] = type;
            if(type == 0){
               pond3["unk1"] = dr.readInt(pond3Offset + 4);
            }else if(type == 1){
               pond3["unk1"] = dr.readInt(pond3Offset + 4);
               pond3["unk2"] = dr.readInt(pond3Offset + 8);
               pond3["unk3"] = dr.readInt(pond3Offset + 12);
               pond3["unk4"] = dr.readFloat(pond3Offset + 16);
               pond3["unk5"] = dr.readFloat(pond3Offset + 20);
               pond3["unk6"] = dr.readInt(pond3Offset + 24);
               pond3["unk7"] = dr.readInt(pond3Offset + 28);
               pond3["unk8"] = dr.readInt(pond3Offset + 32);
               pond3["unk9"] = dr.readFloat(pond3Offset + 36);
               pond3["unk10"] = dr.readFloat(pond3Offset + 40);
               pond3["unk11"] = dr.readFloat(pond3Offset + 44);
            }else if(type == 2){
               pond3["unk1"] = dr.readFloat(pond3Offset + 4);
               pond3["unk2"] = dr.readFloat(pond3Offset + 8);
               pond3["unk3"] = dr.readInt(pond3Offset + 12);
               pond3["unk4"] = dr.readFloat(pond3Offset + 16);
               pond3["unk5"] = dr.readInt(pond3Offset + 20);
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

            if(subtype == 0){
               obj["note"] = "Multiple of 2";
               json::JSON& floats = obj["floats"] = json::Array();
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2; ++n){
                  floats.append(drP.readFloat(offset + 4 + n * 4));
               }
            }else if(subtype == 4){
               obj["note"] = "Multiple of 2";
               json::JSON& floats = obj["floats"] = json::Array();
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2; ++n){
                  floats.append(drP.readFloat(offset + 4 + n * 4));
               }
            }else if(subtype == 5){
               obj["note"] = "Multiple of 2 (with 2 extra)";
               json::JSON& floats = obj["floats"] = json::Array();
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2 + 2; ++n){
                  floats.append(drP.readFloat(offset + 4 + n * 4));
               }
            }else if(subtype == 6){
               obj["note"] = "Multiple of 2";
               json::JSON& floats = obj["floats"] = json::Array();
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2; ++n){
                  floats.append(drP.readFloat(offset + 4 + n * 4));
               }
               obj["preDataIndex"] = drP.readInt(offset + 4 + (floatCount * 2) * 4);
            }else if(subtype == 7){
               obj["note"] = "Multiple of 2 (with 2 extra)";
               json::JSON& floats = obj["floats"] = json::Array();
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2 + 2; ++n){
                  floats.append(drP.readFloat(offset + 4 + n * 4));
               }
               obj["preDataIndex"] = drP.readInt(offset + 4 + (floatCount * 2 + 2) * 4);
            }else if(subtype == 8){
               obj["note"] = "Multiple of 4";
               json::JSON& floats = obj["floats"] = json::Array();
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 4; ++n){
                  floats.append(drP.readFloat(offset + 4 + n * 4));
               }
            }else if(subtype == 9){
               obj["note"] = "Multiple of 4 (with 2 extra)";
               json::JSON& floats = obj["floats"] = json::Array();
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 4 + 2; ++n){
                  floats.append(drP.readFloat(offset + 4 + n * 4));
               }
            }else if(subtype == 12){
               obj["note"] = "Multiple of 2";
               json::JSON& floats = obj["floats"] = json::Array();
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2; ++n){
                  floats.append(drP.readFloat(offset + 4 + n * 4));
               }
            }else if(subtype == 16){
               obj["note"] = "Multiple of 2";
               json::JSON& floats = obj["floats"] = json::Array();
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2; ++n){
                  floats.append(drP.readFloat(offset + 4 + n * 4));
               }
            }else if(subtype == 17){
               obj["note"] = "Multiple of 2 (with 2 extra)";
               json::JSON& floats = obj["floats"] = json::Array();
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2 + 2; ++n){
                  floats.append(drP.readFloat(offset + 4 + n * 4));
               }
            }else if(subtype == 24){
               obj["unk1"] = drP.readFloat(offset + 0);
            }else if(subtype == 25){
               obj["unk1"] = drP.readFloat(offset + 0);
               obj["unk2"] = drP.readFloat(offset + 4);
               obj["unk3"] = drP.readFloat(offset + 8);
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
         };
         auto readSubtypes = [&](int count) -> void{
            for(int n = 0; n < count; ++n){
               readSubtype();
            }
         };


         json::JSON& pond2 = ast["pond2"] = json::Object();
         int type = dr.readInt(pond2Offset + 0);
         pond2["pond2Type"] = type;
         int totalSize = dr.readInt(pond2Offset + 4);
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
            readSubtypes(3);
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
            readSubtypes(6);
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
            readFloat();
            readFloat();
            readInt();
            readFloat();
            readInt("ffxId");
            readInt();
            readInt();
            readInt();
            readZero();
            readSubtypes(12);
            readInt();
            readInt();
            readSubtypes(14);
            readInt();
            readInt();
            readInt();
            readFloat();
            readInt();
         }else{
            ffxReadError(ffxPath, L"pond2 unknown type");
         }
      }

      if(destObjectInstead){
         *destObjectInstead = std::move(ast);
         return -1;
      }else{
         mainASTs[mainAstIndex] = std::move(ast);
         return mainAstIndex;
      }
   };


   char sig[4];
   memcpy(sig, &dr.bytes[0], 4);
   if(memcmp("FXR", sig, 4) != 0){
      ffxReadError(ffxPath, L"Not an ffx file");
   }

   int versionRaw = dr.readInt(4);
   int version = (int)((short*)(&versionRaw))[1];
   int dataStartAfterHeader = dr.readInt(8);
   int unk1 = dr.readInt(24);
   int unk2 = dr.readInt(28);

   root["ffxVersion"] = version;
   root["unk1"] = unk1;
   root["unk2"] = unk2;

   int firstData3Type = dr.readInt(dataStartAfterHeader);
   int offsetToType133Offsets;
   int type133Count;
   int ffxId;
   if(firstData3Type == 133){
      ffxId = dr.readInt(dataStartAfterHeader + 4); 
      offsetToType133Offsets = 8;
      type133Count = 1;
   }else if(firstData3Type == 134){
      ffxId = dr.readInt(dataStartAfterHeader + 4); 
      offsetToType133Offsets = dr.readInt(dataStartAfterHeader + 12);
      type133Count = dr.readInt(dataStartAfterHeader + 16);
   }else{
      ffxReadError(ffxPath, L"First data3 is not 133 or 134");
   }

   // Read array of Type133s
   for(int n = 0; n < type133Count; ++n){
      int addr = dr.readInt(offsetToType133Offsets + n * 4);

      int type = dr.readInt(addr);
      if(type != 133){
         ffxReadError(ffxPath, L"Type is not 133");
      }

      root["ffxId"] = dr.readInt(addr + 4);

      json::JSON t133 = json::Object();
      sprintf(sBuffer, "Type133 index = %d", n);
      t133["note"] = sBuffer;
      t133["always8Or10"] = dr.readInt(addr + 9 * 4);
      t133["preAST1"] = {};
      t133["preAST2"] = {};
      readAST(addr + 10 * 4, &(t133["preAST1"]));
      readAST(addr + 16 * 4, &(t133["preAST2"]));

      t133["houses"] = json::Array();
      int housesOffset = dr.readInt(addr + 22 * 4);
      int houseCount = dr.readInt(addr + 23 * 4);
      for(int h = 0; h < houseCount; ++h){
         int houseOffset = housesOffset + h * 4 * 4;
         int linksOffset = dr.readInt(houseOffset + 0);
         int blossomOffset = dr.readInt(houseOffset + 4);
         int linkCount = dr.readInt(houseOffset + 8);
         int blossomCount = dr.readInt(houseOffset + 12);

         json::JSON house = json::Object();
         sprintf(sBuffer, "houseIndex = %d", h);
         house["note"] = sBuffer;

         json::JSON& links = house["links"] = json::Array();
         for(int l = 0; l < linkCount; ++l){
            int linkedHouseOffset = dr.readInt(linksOffset + l * 8 + 0);
            int data3Offset = dr.readInt(linksOffset + l * 8 + 4);

            json::JSON entry = {
               "linkedHouseIndex", (linkedHouseOffset - housesOffset) / (4 * 4),
               "data3", readData3(data3Offset)
            };

            links.append(std::move(entry));
         }

         json::JSON& blossoms = house["blossoms"] = json::Array();
         for(int b = 0; b < blossomCount; ++b){
            int blossomAndAstSize = 4 + 6 * 4;

            json::JSON entry = {
               "probablyType", dr.readInt(blossomOffset + b * blossomAndAstSize + 0),
               "blossomAST", {}
            };
            readAST(blossomOffset + b * blossomAndAstSize + 4, &(entry["blossomAST"]));

            blossoms.append(std::move(entry));
         }

         t133["houses"].append(std::move(house));
      }

      type133s.append(std::move(t133));
   }

   std::string jsonText = root.dump(0, "   ");
   FILE* file = _wfopen(jsonPath.c_str(), L"w");
   if(file == NULL) {
      jsonWriteError(jsonPath, L"Cannot open file");
      return;
   }
   fwrite(jsonText.data(), 1, jsonText.size(), file);
   fclose(file);
}
