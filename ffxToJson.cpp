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

   json::JSON root;
   root["test"] = {
      "var3", 1,
      "var2", 2,
      "var1", 3,
      "var4", 4,
      "var5", 5,
   };

   std::string jsonText = root.dump(1, "   ");
   
}
