#include "header.hpp"
#include "json.hpp"

enum class OffsetName{
   Invalid,
   house,
};

struct OffsetToFix{
   OffsetName offsetName = OffsetName::Invalid;
   int address = -1;
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
   if(root["jsonVersion"].ToInt() != 1){
      jsonReadError(jsonPath, L"jsonVersion not compatible with this reader");
   }

   DataWriter dw;

   auto writeAST = [&](int index) -> void{
      json::JSON& ast = root["asts"][index];

      dw.write<int>(-1);
      dw.write<int>(-1);
      dw.write<int>(-1);
      dw.write<int>(-1);
      dw.write<int>(-1);
      dw.write<int>(-1);
   };

   dw.write('F');
   dw.write('X');
   dw.write('R');
   dw.write('\0');

   dw.write<short>(0);
   dw.write<short>(root["version"].ToInt());
   dw.write<int>(32); // dataStartAfterHeader
   dw.write<int>(-1); // data2Start
   dw.write<int>(-1); // data2Count
   dw.write<int>(-1); // data3Count
   dw.write<int>(root["unk1"].ToInt());
   dw.write<int>(root["unk2"].ToInt());

   bool needsType134 = root["type133s"].size() > 1;
   if(needsType134){
      dw.write<int>(134);
      // TODO
      for(json::JSON& type133 : root["type133s"].ArrayRange()){

      }
   }else{
      json::JSON& type133 = root["type133s"][0];
      dw.write<int>(133);
      dw.write<int>(root["ffxId"].ToInt());
      dw.write<int>(type133["always8Or10"].ToInt());
      writeAST(type133["astIndex1"].ToInt());
      writeAST(type133["astIndex2"].ToInt());
      dw.write<int>(-1); // house offset
      dw.write<int>(type133["houses"].size());

      for(json::JSON& house : type133["houses"].ArrayRange()){

      }
   }
}
