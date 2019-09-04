#include "header.hpp"

void ffxToJson(const std::wstring& ffxPath, const std::wstring& jsonPath, const TestFunctions& testFunctions){
   DataReader dr;
   dr.path = ffxPath;
   //dr.bpOnRead = 8984;
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
   int lastByteAfterPond3 = 0; // rounds to 8 after
   int lastByteAfterData3 = 0; // DSR padding
   int astSize;
   int longSize;

   std::function<json::JSON(int)> readData3;
   std::function<json::JSON(int)> readAST;

   readData3 = [&](int addr) -> json::JSON{
      json::JSON data3 = json::Object();

      int type = dr.readLong(addr);
      data3["data3Type"] = type;

      if(type == 112 || type == 113 || type == 129 || type == 130 || type == 131 || type == 132 || type == 136){
         // empty
      }else if(type == 1 || type == 59 || type == 66 || type == 68 || type == 111 || type == 138 || type == 139){
         data3["unk1"] = dr.readBadLong();
      }else if(type == 7 || type == 70){
         data3["unk1"] = dr.readBadFloat();
      }else if(type == 79 || type == 140){
         data3["unk1"] = dr.readInt();
         data3["unk2"] = dr.readInt();
      }else if(type == 81 || type == 85){
         data3["unk1"] = dr.readFloat();
         data3["unk2"] = dr.readFloat();
      }else if(type == 137){
         data3["unk1"] = dr.readInt();
         data3["unk2"] = dr.readInt();
         data3["unk3"] = dr.readInt();
      }else if(type == 2){
         int offset = dr.readInt();
         int count = dr.readInt();

         json::JSON& bunchaInts = data3["bunchaInts"] = json::Array();
         for(int n = 0; n < count; ++n){
            bunchaInts.append(dr.readInt(offset + n * 4)); // FIX
         }
      }else if(type == 3 || type == 5 || type == 6 || type == 9){
         int offsetToFloats = dr.readLong();
         int offsetToInts = dr.readLong();
         int count = dr.readBadLong();
         
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
         int offsetToFloats = dr.readInt();
         int offsetToInts = dr.readInt();
         int count = dr.readInt();
         int unk1 = dr.readInt();
         int unk2 = dr.readInt();

         data3["note"] = "counts must match; unk1 and unk2 are always 1 and 0";
         data3["floats"] = json::Array();
         data3["ints"] = json::Array();
         json::JSON& floats = data3["floats"];
         json::JSON& ints = data3["ints"];

         for(int n = 0; n < count; ++n){
            floats.append(dr.readFloat(offsetToFloats + n * 4)); // FIX
            ints.append(dr.readInt(offsetToInts + n * 4));
         }

         data3["unk1"] = unk1;
         data3["unk2"] = unk2;
      }else if(type == 11 || type == 12){
         int offsetA = dr.readLong();
         int offsetB = dr.readLong();
         int count = dr.readBadLong();

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
         int offsetA = dr.readInt();
         int offsetB = dr.readInt();
         int count = dr.readInt();

         data3["note"] = "floatsB count is floatsA count x3";
         data3["floatsA"] = json::Array();
         data3["floatsB"] = json::Array();
         json::JSON& floatsA = data3["floatsA"];
         json::JSON& floatsB = data3["floatsB"];

         for(int n = 0; n < count; ++n){
            floatsA.append(dr.readFloat(offsetA + n * 4)); // VERY FIX
            floatsB.append(dr.readFloat(offsetB + n * 3 * 4 + 0));
            floatsB.append(dr.readFloat(offsetB + n * 3 * 4 + 4));
            floatsB.append(dr.readFloat(offsetB + n * 3 * 4 + 8));
         }
      }else if(type == 19 || type == 20){
         int offsetA = dr.readInt();
         int offsetB = dr.readInt();
         int count = dr.readInt();

         data3["note"] = "floatsB count is floatsA count x4";
         data3["floatsA"] = json::Array();
         data3["floatsB"] = json::Array();
         json::JSON& floatsA = data3["floatsA"];
         json::JSON& floatsB = data3["floatsB"];

         for(int n = 0; n < count; ++n){
            floatsA.append(dr.readFloat(offsetA + n * 4)); // AHHHH
            floatsB.append(dr.readFloat(offsetB + n * 4 * 4 + 0));
            floatsB.append(dr.readFloat(offsetB + n * 4 * 4 + 4));
            floatsB.append(dr.readFloat(offsetB + n * 4 * 4 + 8));
            floatsB.append(dr.readFloat(offsetB + n * 4 * 4 + 12));
         }
      }else if(type == 91 || type == 95){
         int offsetA = dr.readInt();
         int offsetB = dr.readInt();
         int count = dr.readInt();
         int unk1 = dr.readInt();
         int data3Offset = dr.readInt();

         data3["note"] = "counts must match; unk1 always 1";
         data3["floatsA"] = json::Array();
         data3["floatsB"] = json::Array();
         json::JSON& floatsA = data3["floatsA"];
         json::JSON& floatsB = data3["floatsB"];

         for(int n = 0; n < count; ++n){
            floatsA.append(dr.readFloat(offsetA + n * 4)); // FIX
            floatsB.append(dr.readFloat(offsetB + n * 4));
         }

         data3["unk1"] = unk1;
         data3["data3"] = readData3(data3Offset);
      }else if(type == 37){
         int templateFfxId = dr.readBadLong();
         int astOffset = dr.readLong();
         dr.readLong();

         data3["templateFfxId"] = templateFfxId;
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
      }else if(type == 38){
         int t38Subtype = dr.readBadLong();
         int astOffset = dr.readLong();
         dr.readLong();

         data3["t38Subtype"] = t38Subtype;
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
      }else if(type == 44 || type == 45 || type == 46 || type == 47 || type == 60 || type == 71 || type == 87 || type == 114 || type == 115){
         int value;
         if(type == 44 || type == 45 || type == 46 || type == 47 || type == 71){
            value = dr.readLong();
         }else{
            value = dr.readBadLong();
         }
         data3["unk1"] = reinterpret_cast<short*>((&value))[0];
         data3["unk2"] = reinterpret_cast<short*>((&value))[1];
      }else if(type == 128){
         int data3Offset = dr.readLong();

         data3["data3"] = readData3(data3Offset);
      }else if(type == 120 || type == 121 || type == 122 || type == 123 || type == 124 || type == 126 || type == 127){
         int data3OffsetA = dr.readLong();
         int data3OffsetB = dr.readLong();

         data3["data3A"] = readData3(data3OffsetA);
         data3["data3B"] = readData3(data3OffsetB);
      }else{
         swprintf(wBuffer, sizeof(wBuffer), L"Data3 type %d unfamiliar", type);
         ffxReadError(ffxPath, wBuffer);
      }

      if(dr.marker > lastByteAfterData3){
         lastByteAfterData3 = dr.marker;
      }

      testFunctions.onData3(data3, {root, root});

      return std::move(data3);
   };

   DataReader drP; // pond2
   readAST = [&](int addr) -> json::JSON{
      int astAddress = addr == -1 ? dr.marker : addr;
      int oldMarker = dr.marker;

      int pond1Offset = dr.readLong(astAddress);
      int data3Count = dr.readInt();
      int data3Count2 = dr.readInt();
      bool flag1 = (bool)dr.readByte();
      bool flag2 = (bool)dr.readByte();
      bool flag3 = (bool)dr.readByte();
      dr.readByte();
      dr.readInt(); // 0xcdcdcdcd
      int pond2Offset = dr.readLong();
      int pond3Offset = dr.readLong();

      json::JSON ast = json::Object();
      sprintf(sBuffer, "offset = %d", astAddress);
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
         int currentOffset = pond1Offset;
         for(int n = 0; n < data3Count; ++n){
            // For checking all bytes are read
            if(pond1Offset + n * longSize > lastPond1Data3Offset){
               lastPond1Data3Offset = pond1Offset + n * longSize;
            }

            int offsetToData3 = dr.readLong(pond1Offset + n * longSize);
            data3s.append(std::move(readData3(offsetToData3)));
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

            if(type != 1 && dr.marker > lastByteAfterPond3){
               lastByteAfterPond3 = dr.marker;
            }
         }
      }

      if(pond2Offset){
         int pond2Address = pond2Offset;
         json::JSON* currentObject = nullptr;
         int headerBeforeDR = dr.isRemaster ? 40 : 24;

         auto readZero = [&]() -> void{
            drP.readInt();
         };
         auto readInt = [&](const char* name = nullptr) -> int{
            if(name == nullptr){
               sprintf(sBuffer, "unk%dInt", currentObject->size() - 1);
               name = sBuffer;
            };

            int result = drP.readInt();
            (*currentObject)[name] = result;

            return result;
         };
         auto readLong = [&](const char* name = nullptr) -> int{
            if(name == nullptr){
               sprintf(sBuffer, "unk%dInt", currentObject->size() - 1);
               name = sBuffer;
            };

            int result = drP.readLong();
            (*currentObject)[name] = result;

            return result;
         };
         auto readFloat = [&](const char* name = nullptr) -> float{
            if(name == nullptr){
               sprintf(sBuffer, "unk%dFloat", currentObject->size() - 1);
               name = sBuffer;
            };

            float result = drP.readFloat();
            (*currentObject)[name] = result;

            return result;
         };
         auto readSubtype = [&](const char* name = nullptr, int readAtLocalOffsetInstead = 0) -> void{
            if(readAtLocalOffsetInstead != 0){
               drP.marker = readAtLocalOffsetInstead;
            }
            int subtype = drP.readLong();
            int fullOffset = drP.readLong();
            int oldMarker = drP.marker;
            drP.marker = fullOffset - pond2Address - headerBeforeDR;

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

               int arraySize = drP.readInt();

               if(hasRange){
                  obj["range"] = json::Array();
               }

               obj["times"] = json::Array();
               json::JSON& times = obj["times"];
               for(int n = 0; n < arraySize; ++n){
                  obj["times"].append(drP.readFloat());
               }

               if(numArrays == 2){
                  obj["values"] = json::Array();
                  json::JSON& values = obj["values"];
                  for(int n = 0; n < arraySize; ++n){
                     values.append(drP.readFloat());
                  }
               }else{
                  obj["values1"] = json::Array();
                  obj["values2"] = json::Array();
                  obj["values3"] = json::Array();
                  json::JSON& values1 = obj["values1"];
                  json::JSON& values2 = obj["values2"];
                  json::JSON& values3 = obj["values3"];
                  for(int n = 0; n < arraySize; ++n){
                     values1.append(drP.readFloat());
                     values2.append(drP.readFloat());
                     values3.append(drP.readFloat());
                  }
               }

               if(hasRange){
                  obj["range"].append(drP.readFloat());
                  obj["range"].append(drP.readFloat());
               }

               if(hasPreDataIndex){
                  obj["preDataIndex"] = drP.readInt();
               }
            }else if(subtype == 24){
               obj["constant"] = drP.readFloat();
            }else if(subtype == 25){
               obj["base"] = drP.readFloat();
               obj["unknownA"] = drP.readFloat();
               obj["unknownB"] = drP.readFloat();
            }else if(subtype == 26){
               obj["unk1"] = drP.readFloat();
               obj["unk2"] = drP.readInt();
            }else if(subtype == 27){
               obj["unk1"] = drP.readFloat();
               obj["unk2"] = drP.readFloat();
               obj["unk3"] = drP.readFloat();
               obj["preDataIndex"] = drP.readInt();
            }else if(subtype == 28){
               // Empty, fullOffset is 0
            }else{
               wchar_t wBuffer[80];
               swprintf(wBuffer, sizeof(wBuffer), L"Pond2 subtype %d unfamiliar, addr = %d\n", subtype, fullOffset);
               ffxReadError(ffxPath, wBuffer);
            }

            drP.marker = oldMarker;

            testFunctions.onPond2Subtype(obj, {*currentObject, root});
         };
         auto readSubtypes = [&](int count) -> void{
            for(int n = 0; n < count; ++n){
               readSubtype();
            }
         };


         json::JSON& pond2 = ast["pond2"] = json::Object();
         int type = dr.readInt(pond2Offset);
         pond2["pond2Type"] = type;
         pond2["note"] = "";
         // Round to padding
         int totalSize = (dr.readInt() + 15) / 16 * 16;
         int preDataCount = dr.readLong();
         int offsetToPreDataNumbers = dr.readLong();
         int offsetToPreDataSubtypes = dr.readLong();
         int offsetToSourceAST = dr.readLong();

         drP.bytes.resize(0);
         int headerSize = dr.marker - pond2Offset;
         for(int n = headerSize; n < totalSize; ++n){
            drP.bytes.push_back(dr.readByte(pond2Offset + n));
         }
         drP.bytesRead.resize(0);
         drP.bytesRead.resize(drP.bytes.size());
         drP.marker = 0;
         drP.isRemaster = dr.isRemaster;
         drP.path = dr.path;

         // Pre data
         json::JSON& preData = pond2["preData"] = json::Array();
         if(preDataCount > 0){
            int localNumbersOffset = offsetToPreDataNumbers - pond2Address - headerBeforeDR;
            int localSubtypesOffset = offsetToPreDataSubtypes - pond2Address - headerBeforeDR;
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
            readFloat();
            if(dr.isRemaster) readZero(); // ??? maybe it rounds to nearest 16 in both versions?
            readInt("texId");
            readInt();
            readZero();
            readSubtypes(10);
            readInt();
            readInt();
            readInt();
            readFloat();

            if(dr.isRemaster){
               readSubtype("remasterDataFromHere");
               readSubtypes(4);
               readInt();
               readInt();
               readInt();
               readInt();
               readInt();
               readInt();
            }
         }else if(type == 28){
            pond2["note"] = "Pond2 type 28: initial speed";
            // Are these XYZ?
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
            if(dr.isRemaster){
               readSubtype("remasterDataFromHere");
               readSubtypes(4);
               readInt();
               readInt();
               readInt();
               readInt();
               readInt();
               readInt();
            }
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

            if(dr.isRemaster){
               readSubtype("remasterDataFromHere");
               readSubtypes(4);
               readInt();
               readInt();
               readInt();
               readInt();
               readInt();
               readInt();
            }
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
            if(dr.isRemaster) readZero(); // ???
            readInt("texId");
            readInt();
            readInt();
            readInt();
            readInt();
            readSubtypes(10);
            readInt();
            readInt();
            readSubtypes(10);
            if(dr.isRemaster){
               readInt("remaster1");
               readInt("remaster2");
               readInt("remaster3");
               readInt("remaster4");
            }
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
            if(dr.isRemaster){
               readSubtype("remasterFromHere");
               readSubtypes(4);
               readFloat();
               readInt();
               readInt();
               readInt();
               readInt();
               readInt();
            }
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
            readLong();
            readSubtypes(1);
         }else if(type == 107){
            readFloat();
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
         }else if(type == 117){
            // DSR
            readSubtypes(6);
            readInt();
            readInt();
            readSubtypes(1);
         }else{
            ffxReadError(ffxPath, L"pond2 unknown type");
         }

         // TODO: test if all bytes are read

         testFunctions.onPond2(pond2, {ast, root});
      }

      if(addr != -1){
         dr.marker = oldMarker;
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

   bool isRemaster = dataStartAfterHeader != 32;
   root["gameVersion"] = isRemaster ? "DSR" : "PTD";
   if(isRemaster){
      dr.isRemaster = true;
      dr.readInt();
   }

   int data2Start = dr.readInt();
   int data2Count = dr.readInt();
   int data3Count = dr.readInt();
   root["unk1"] = dr.readInt();
   root["unk2"] = dr.readInt();
   dr.readPadding(16);

   astSize = isRemaster ? 2*4 + 4*8 : 6*4;
   longSize = isRemaster ? 8 : 4;

   int firstData3Type = dr.readLong(dataStartAfterHeader);
   int offsetToType133Offsets;
   int type133Count;
   int ffxId;
   if(firstData3Type == 133){
      ffxId = dr.readBadLong(); 
      offsetToType133Offsets = 8;
      type133Count = 1;
   }else if(firstData3Type == 134){
      ffxId = dr.readBadLong(); 
      dr.readBadLong(); 
      offsetToType133Offsets = dr.readLong();
      type133Count = dr.readBadLong();
   }else{
      ffxReadError(ffxPath, L"First data3 is not 133 or 134");
   }

   // Read array of Type133s
   for(int n = 0; n < type133Count; ++n){
      int addr = firstData3Type == 133 ? dataStartAfterHeader : dr.readLong(offsetToType133Offsets + n * longSize);

      int type = firstData3Type == 133 ? 133 : dr.readLong(addr);
      if(type != 133){
         ffxReadError(ffxPath, L"Type is not 133");
      }

      root["ffxId"] = firstData3Type == 133 ? ffxId : dr.readBadLong();

      for(int n = 0; n < 3; ++n){
         dr.readBadLong();
         dr.readLong();
      }
      dr.readLong();

      json::JSON t133 = json::Object();
      sprintf(sBuffer, "Type133 index = %d", n);
      t133["note"] = sBuffer;
      t133["always8Or10"] = dr.readBadLong();
      int oldMarker = dr.marker;
      t133["preAST1"] = readAST(-1);
      testFunctions.onAST(t133["preAST1"], {root, root}, TestFunctions::Pre1, t133["always8Or10"].ToInt());
      dr.marker = oldMarker + astSize * 1;
      t133["preAST2"] = readAST(-1);
      testFunctions.onAST(t133["preAST2"], {root, root}, TestFunctions::Pre2, t133["always8Or10"].ToInt());
      dr.marker = oldMarker + astSize * 2;

      t133["houses"] = json::Array();
      int housesOffset = dr.readLong();
      int houseCount = dr.readBadLong();
      int houseOffset = housesOffset;
      for(int h = 0; h < houseCount; ++h){
         int linksOffset = dr.readLong(houseOffset);
         int blossomOffset = dr.readLong();
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
            int linkedHouseOffset = dr.readLong(linkOffset);
            int data3Offset = dr.readLong();
            linkOffset = dr.marker;

            json::JSON entry = {
               "linkedHouseIndex", (linkedHouseOffset - housesOffset) / houseSize,
               "data3", readData3(data3Offset)
            };

            links.append(std::move(entry));
         }

         json::JSON& blossoms = house["blossoms"] = json::Array();
         for(int b = 0; b < blossomCount; ++b){
            int blossomType = dr.readLong(blossomOffset);
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
   if(!dr.isRemaster && lastPond1Data3Offset != 0){
      dr.readPadding(16, lastPond1Data3Offset + 4);
   }
   if(dr.isRemaster && lastByteAfterData3 != 0){
      dr.readPadding(16, lastByteAfterData3);
   }

   // Round a section to 8 because there's a long after
   if(dr.isRemaster && lastByteAfterPond3 != 0){
      dr.readPadding(8, lastByteAfterPond3);
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
      dr.marker = data2Start;
      for(int n = 0; n < data2Count + data3Count; ++n){
         dr.readLong();
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
