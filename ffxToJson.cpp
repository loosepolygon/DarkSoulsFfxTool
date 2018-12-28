#include "header.hpp"
#include "json.hpp"

void ffxToJson(std::wstring ffxPath, std::wstring jsonPath){
   DataReader dr;
   {
      FILE* file = _wfopen(ffxPath.c_str(), L"rb");
      currentGlobalFile = file;
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

   std::function<json::JSON(int)> readData3;
   std::function<json::JSON(int)> readAST;
   readData3 = [&](int addr) -> json::JSON{
      json::JSON data3 = json::Object();

      int type = dr.readInt(addr);
      data3["type"] = type;

      if(type == 112 || type == 113 || type == 129 || type == 130 || type == 131 || type == 132 || type == 136){
         // empty
      }else if(type == 1 || type == 44 || type == 46 || type == 59 || type == 60 || type == 66 || type == 68 || type == 71 || type == 111 || type == 138 || type == 139){
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
         data3["unk4"] = dr.readInt(addr + 16);
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
         json::JSON& floats = data3["floats"] = json::Array();
         json::JSON& ints = data3["ints"] = json::Array();

         for(int n = 0; n < count; ++n){
            floats.append(dr.readFloat(offsetToFloats + n * 4));
            ints.append(dr.readInt(offsetToInts + n * 4));
         }
      }else if(type == 89){
         int offsetToFloats = dr.readInt(addr + 4);
         int offsetToInts = dr.readInt(addr + 8);
         int count = dr.readInt(addr + 12);

         data3["note"] = "counts must match; unk1 and unk2 are always 1 and 0";
         json::JSON& floats = data3["floats"] = json::Array();
         json::JSON& ints = data3["ints"] = json::Array();

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
         json::JSON& floatsA = data3["floatsA"] = json::Array();
         json::JSON& floatsB = data3["floatsB"] = json::Array();
         for(int n = 0; n < count; ++n){
            floatsA.append(dr.readFloat(offsetA + n * 4));
            floatsB.append(dr.readFloat(offsetB + n * 4));
         }
      }else if(type == 13 || type == 14){
         int offsetA = dr.readInt(addr + 4);
         int offsetB = dr.readInt(addr + 8);
         int count = dr.readInt(addr + 12);

         data3["note"] = "floatsB count is floatsA count x3";
         json::JSON& floatsA = data3["floatsA"] = json::Array();
         json::JSON& floatsB = data3["floatsB"] = json::Array();
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
         json::JSON& floatsA = data3["floatsA"] = json::Array();
         json::JSON& floatsB = data3["floatsB"] = json::Array();
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
         json::JSON& floatsA = data3["floatsA"] = json::Array();
         json::JSON& floatsB = data3["floatsB"] = json::Array();
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
            data3["ast"] = readAST(astOffset);
         }else{
            data3["ast"] = {};
         }
      }else if(type == 38){
         data3["unk1"] = dr.readInt(addr + 4);
         int astOffset = dr.readInt(addr + 8);
         if(astOffset){
            data3["ast"] = readAST(astOffset);
         }else{
            data3["ast"] = {};
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
   readAST = [&](int addr) -> json::JSON{
      int pond1Offset = dr.readInt(addr+0);
      int pond2Offset = dr.readInt(addr+16);
      int pond3Offset = dr.readInt(addr+20);

      json::JSON ast = json::Object();
      ast["flag1"] = (bool)dr.readByte(addr + 12);
      ast["flag2"] = (bool)dr.readByte(addr + 13);
      ast["flag3"] = (bool)dr.readByte(addr + 14);
      if(pond1Offset){
         ast["type"] = "Unknown, has Data3s";
      }else if(pond2Offset){
         ast["type"] = "Shader params and stuff?";
      }else{
         ast["type"] = "Empty";
      }

      if(pond1Offset){
         json::JSON& data3s = ast["data3s"] = json::Array();
         int data3Count = dr.readInt(addr+4);
         for(int n = 0; n < data3Count; ++n){
            int offsetToData3 = dr.readInt(pond1Offset + n * 4);
            data3s.append(readData3(offsetToData3));
         }

         if(pond2Offset){

         }
      }

      return std::move(ast);
   };


   char sig[4];
   memcpy(sig, &dr.bytes[0], 4);
   if(memcmp("FXR", sig, 4) != 0){
      ffxReadError(ffxPath, L"Not an ffx file");
   }

   int versionRaw = dr.readInt(4);
   int version = (int)((short*)(&versionRaw))[1];
   int dataStartAfterHeader = dr.readInt(8);
   //int data2Start = dr.readInt(12);
   //int data2Count = dr.readInt(16);
   //int data3Start = data2Start + data2Count * 4;
   //int data3Count = dr.readInt(20);
   int unk1 = dr.readInt(24);
   int unk2 = dr.readInt(28);

   json::JSON root;
   root["version"] = version;
   //root["unk1"] = unk1;
   //root["unk2"] = unk2;
   root["ffxId"] = -1;

   //readData3(dataStartAfterHeader, root);

   json::JSON& type133s = root["Type133s"] = json::Array();

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
      t133["always8Or10"] = dr.readInt(addr + 9 * 4);
      t133["ast1"] = readAST(addr + 10 * 4);
      t133["ast2"] = readAST(addr + 16 * 4);
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
         sprintf(sBuffer, "index = %d", h);
         house["note"] = sBuffer;

         json::JSON& links = house["links"] = json::Array();
         for(int l = 0; l < linkCount; ++l){
            int linkedHouseOffset = dr.readInt(linksOffset + l * 8 + 0);
            int data3Offset = dr.readInt(linksOffset + l * 8 + 4);

            json::JSON entry = {
               "index", (linkedHouseOffset - housesOffset) / (4 * 4),
               "data3", readData3(data3Offset)
            };

            links.append(std::move(entry));
         }

         json::JSON& blossoms = house["blossoms"] = json::Array();
         for(int b = 0; b < blossomCount; ++b){
            int blossomAndAstSize = 4 + 6 * 4;

            json::JSON entry = {
               "probablyType", dr.readInt(blossomOffset + b * blossomAndAstSize + 0),
               "ast", readAST(blossomOffset + b * blossomAndAstSize + 4)
            };

            blossoms.append(std::move(entry));
         }

         t133["houses"].append(std::move(house));
      }

      type133s.append(std::move(t133));
   }

   std::string jsonText = root.dump(1, "   ");
}
