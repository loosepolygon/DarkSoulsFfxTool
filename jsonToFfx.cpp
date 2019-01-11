#include "header.hpp"
#include "json.hpp"

//enum class OffsetName{
//   Invalid,
//   house,
//};
//
//struct OffsetToFix{
//   OffsetName offsetName = OffsetName::Invalid;
//   int address = -1;
//};

struct OffsetToFix{
   DataWriter& dwSource; // Example: AST in blossoms
   int sourceOffset;
   DataWriter& dwDest; // Example: dwPond1Arrays
   int destOffset;
};

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
   DataWriter dwPond1Arrays;
   DataWriter dwType1Pond3s;
   DataWriter dwPond2Data;
   DataWriter* dwList[] = {
      &dwMainASTs,
      &dwHouses,
      &dwLinks,
      &dwBlossoms,
      &dwSubDataAndPond3s,
      &dwData3s,
      &dwPond1Arrays,
      &dwType1Pond3s,
      &dwPond2Data
   };

   
   auto writeASTAt = [&](DataWriter& dw, int offset, json::JSON& ast) -> void{
      int flags = 0;
      if(ast["flag1"].ToBool()) flags |= 0x1;
      if(ast["flag2"].ToBool()) flags |= 0x100;
      if(ast["flag3"].ToBool()) flags |= 0x10000;
      dw.writeAt<int>(offset + 12, 65536);

      dw.writeAt<int>(offset + 0,  -1);
      dw.writeAt<int>(offset + 4,  -1);
      dw.writeAt<int>(offset + 8,  -1);

      dw.writeAt<int>(offset + 16, -1);
      dw.writeAt<int>(offset + 20, -1);
   };

   auto writeAST = [&](DataWriter& dw, json::JSON& ast) -> void{
      writeASTAt(dw, dw.bytes.size(), ast);
   };

   auto writeMainAST = [&](int index) -> void{
      writeASTAt(dwMainASTs, index * sizeof(int) * 6, root["asts"][index]);
   };

   dwMain.write('F');
   dwMain.write('X');
   dwMain.write('R');
   dwMain.write('\0');

   dwMain.write<short>(0);
   dwMain.write<short>(root["ffxVersion"].ToInt());
   dwMain.write<int>(32); // dataStartAfterHeader
   dwMain.write<int>(-1); // data2Start
   dwMain.write<int>(-1); // data2Count
   dwMain.write<int>(-1); // data3Count
   dwMain.write<int>(root["unk1"].ToInt());
   dwMain.write<int>(root["unk2"].ToInt());

   bool needsType134 = root["type133s"].size() > 1;
   if(needsType134){
      // TODO
      return;

      dwMain.write<int>(134);
      for(json::JSON& type133 : root["type133s"].ArrayRange()){

      }
   }else{
      json::JSON& type133 = root["type133s"][0];
      dwMain.write<int>(133);
      dwMain.write<int>(root["ffxId"].ToInt());
      dwMain.write<int>(type133["always8Or10"].ToInt());
      json::JSON& preAST1 = type133["preAST1"];
      json::JSON& preAST2 = type133["preAST2"];
      writeAST(dwMain, preAST1);
      writeAST(dwMain, preAST2);
      dwMain.write<int>(-1); // house offset
      dwMain.write<int>(type133["houses"].size());

      int houseIndex = 0;
      for(json::JSON& house : type133["houses"].ArrayRange()){
         dwHouses.write<int>(-1); // links offset
         dwHouses.write<int>(-1); // blossom offset
         dwHouses.write<int>(house["links"].size());
         dwHouses.write<int>(house["blossoms"].size());

         for(json::JSON& blossom : house["blossoms"].ArrayRange()){
            dwBlossoms.write<int>(blossom["probablyType"].ToInt());
            writeAST(dwBlossoms, blossom["blossomAST"]);
         }


         ++houseIndex;
      }

      // Another houses loop to write links?
   }

   for(DataWriter* dw : {
      &dwMainASTs,
      &dwHouses,
      &dwLinks,
      &dwBlossoms,
      &dwSubDataAndPond3s,
      &dwData3s,
      &dwPond1Arrays,
      &dwType1Pond3s,
      &dwPond2Data
   }){
      dwMain.bytes.insert(dwMain.bytes.end(), dw->bytes.begin(), dw->bytes.end());
   }

   FILE* file = _wfopen(ffxPath.c_str(), L"wb");
   fwrite(dwMain.bytes.data(), 1, dwMain.bytes.size(), file);
   fclose(file);
}
