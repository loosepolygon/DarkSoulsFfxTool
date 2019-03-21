#include "header.hpp"
#include "md5.hpp"

int jsonVersion = 2;

void ffxReadError(const std::wstring& path, const std::wstring& text){
   std::wstring errorMessage = L"Error reading file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());
   system("pause");
   throw;
}

void ffxWriteError(const std::wstring& path, const std::wstring& text){
   std::wstring errorMessage = L"Error writing ffx file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());
   system("pause");
   throw;
}

void jsonReadError(const std::wstring& path, const std::wstring& text){
   std::wstring errorMessage = L"Error reading json file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());
   system("pause");
   throw;
}

void jsonWriteError(const std::wstring& path, const std::wstring& text){
   std::wstring errorMessage = L"Error writing json file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());
   system("pause");
   throw;
}

void argError(const std::wstring& text){
   std::wstring outputText = L"Error parsing args: " + text;

   outputText += L"\n\n";

   outputText += L"* Usage: DSFfxTool [inputPath] [outputPath]\n";
   outputText += L"* (outputPath is optional)\n";
   outputText += L"* Examples:\n";
   outputText += L"   * DSFfxTool f0000026.ffx\n";
   outputText += L"   * DSFfxTool f0000026.ffx \"My Test File.json\"\n";
   outputText += L"   * DSFfxTool \"My Test File.json\" f0000026.ffx\n";

   fwprintf_s(stderr, L"%s\n", outputText.c_str());
   system("pause");
   exit(1);
}


std::string getMD5(const std::wstring& filePath){
   std::vector<byte> bytes;
   {
      FILE* file = _wfopen(filePath.c_str(), L"rb");
      if(file == NULL) {
         ffxReadError(filePath, L"Cannot open file");
         return "";
      }

      fseek(file, 0, SEEK_END);
      long fileSize = ftell(file);
      fseek(file, 0, SEEK_SET);

      bytes.resize(fileSize);
      fread(bytes.data(), 1, bytes.size(), file);

      fclose(file);
   }

   MD5 md5;
   md5.update(bytes.data(), bytes.size());
   md5.finalize();
   return std::move(md5.hexdigest());
}

void importEveryFfx(std::wstring originalDir, std::wstring jsonDir){
   std::wstring dirArg = L"mkdir " + jsonDir;
   if(dirArg.back() == L'/') dirArg.resize(dirArg.size() - 1);
   _wsystem(dirArg.c_str());

   for(const std::wstring& fileName : getFileNamesInDir(originalDir)){
      ffxToJson(originalDir + fileName, jsonDir + fileName + L".json");
   }
}

void exportEveryFfxAndTest(std::wstring originalDir, std::wstring jsonDir, std::wstring ffxDir){
   std::wstring dirArg = L"mkdir " + ffxDir;
   if(dirArg.back() == L'/') dirArg.resize(dirArg.size() - 1);
   _wsystem(dirArg.c_str());

   for(const std::wstring& fileName : getFileNamesInDir(originalDir)){
      wprintf(L"%s\n", fileName.c_str());

      jsonToFfx(jsonDir + fileName + L".json", ffxDir + fileName);

      std::string md5Original = getMD5(originalDir + fileName);
      std::string md5Rebuilt = getMD5(ffxDir + fileName);

      if(md5Original != md5Rebuilt){
         throw;
      }
   }

   system("pause");
}

void outputTree(json::JSON& root, std::wstring outputPath){
   char sBuffer[300];
   std::string outputText;

   int depth = 0;
   auto printLineStart = [&](){
      for(int n = 0; n < depth; ++n){
         outputText += "   ";
      }
      outputText += "* ";
   };

   std::function<void(json::JSON&, int, int, bool)> parseAST;
   parseAST = [&](json::JSON& ast, int astIndex, int arg, bool isT37){
      bool isMainAST = astIndex >= 0;

      if(isMainAST){
         printLineStart();
         if(isT37){
            sprintf(sBuffer, "AST[%d] (ref ffx %d)", astIndex, arg);
         }else{
            sprintf(sBuffer, "AST[%d] (T%d)", astIndex, arg);
         }
         outputText += sBuffer;
         ++depth;
      }

      int astType = ast["astType"].ToInt();
      if(astType == 0){
         outputText += " (empty)\n";
      }else if(astType == 1){
         outputText += " (properties and ASTs)";

         sprintf(sBuffer, " (x%d)", ast["pond1Data3s"].size());
         outputText += sBuffer;

         if(ast["pond3"].size() > 0){
            sprintf(sBuffer, " (with P3T%d)", ast["pond3"]["pond3Type"].ToInt());
            outputText += sBuffer;
         }

         outputText += "\n";

         for(json::JSON& data3 : ast["pond1Data3s"].ArrayRange()){
            int data3Type = data3["data3Type"].ToInt();
            if(data3Type == 37 || data3Type == 38){
               int otherASTIndex = data3["mainASTIndex"].ToInt();
               if(otherASTIndex != -1){
                  json::JSON& otherAST = root["mainASTs"][otherASTIndex];
                  if(data3.hasKey("refFfxId")){
                     parseAST(otherAST, otherASTIndex, data3["refFfxId"].ToInt(), true);
                  }else{
                     parseAST(otherAST, otherASTIndex, data3["pond1Or2TypeMaybe"].ToInt(), false);
                  }
               }
            }
         }
      }else if(astType == 2){
         outputText += " (particle emission and data curves)";

         int pond2Type = ast["pond2"]["pond2Type"].ToInt();
         for(int t : {27, 40, 43, 59, 61, 70, 71, 107, 108}){
            if(pond2Type == t){
               outputText += " **PARTICLE**";
               break;
            }
         }

         outputText += "\n";
      }

      if(isMainAST){
         --depth;
      }
   };

   if(root["type133s"].size() != 1){
      //printf("nope");
      return;
   }

   auto t133 = root["type133s"][0];
   if(
      t133["preAST1"]["astType"].ToInt() != 0 ||
      t133["preAST2"]["astType"].ToInt() != 0
   ){
      //printf("nope");
      return;
   }

   int houseCount = t133["houses"].size();
   for(int h = 0; h < houseCount; ++h){
      printLineStart();
      sprintf(sBuffer, "house[%d]\n", h);
      outputText += sBuffer;
      ++depth;

      json::JSON& house = t133["houses"][h];
      int blossomCount = house["blossoms"].size();
      for(int b = 0; b < blossomCount; ++b){
         json::JSON& blossom = house["blossoms"][b];
         json::JSON& blossomAST = blossom["blossomAST"];
         int blossomType = blossom["probablyType"].ToInt();

         printLineStart();
         sprintf(sBuffer, "blossom[%d] (T%d)", b, blossomType);
         outputText += sBuffer;
         ++depth;

         parseAST(blossomAST, -1, blossomType, false);

         --depth;
      }

      --depth;
   }

   FILE* file = _wfopen(outputPath.c_str(), L"wb");
   fwrite(outputText.data(), 1, outputText.size(), file);
   fclose(file);
}

void importEveryFfxAndResearch(std::wstring originalDir, std::wstring jsonDir){
   std::string outputText;

   std::wstring dirArg = L"mkdir " + jsonDir;
   if(dirArg.back() == L'/') dirArg.resize(dirArg.size() - 1);
   _wsystem(dirArg.c_str());

   //char sBuffer[200];

   TestFunctions testFunctions;

   /*testFunctions.onPond2Subtype = [&](json::JSON& obj, TestFunctions::Context context){
      int type = obj["subtypeType"].ToInt();
      if(type == 8){
         sprintf(sBuffer, "P2T%d, ffx %d\n", context.parent["pond2Type"].ToInt(), context.ffxId);
         outputText += sBuffer;

         json::JSON a1 = json::Array();
         json::JSON a2 = json::Array();
         json::JSON a3 = json::Array();
         json::JSON a4 = json::Array();
         int count = obj["floats"].size() / 4;
         for(int n = 0; n < count; ++n){
            a1.append(obj["floats"][count * 0 + n]);
            a2.append(obj["floats"][count * 1 + n]);
            a3.append(obj["floats"][count * 2 + n]);
            a4.append(obj["floats"][count * 3 + n]);
         }
         outputText += a1.dump() + "\n";
         outputText += a2.dump() + "\n";
         outputText += a3.dump() + "\n";
         outputText += a4.dump() + "\n\n";
      }
   };*/

   //testFunctions.onAST = [&](json::JSON& obj, int astSupertype, TestFunctions::Context context){
   //   if(astSupertype == 104){
   //      //printf("%d\n", obj["pond1Data3s"].size());
   //      for(json::JSON data3 : obj["pond1Data3s"].ArrayRange()){
   //         printf("%d, ", data3["data3Type"].ToInt());
   //      }
   //      printf("\n");
   //   }
   //};

   system("mkdir tree");
   testFunctions.onRoot = [&](json::JSON& root, TestFunctions::Context context) -> void{
      wchar_t wBuffer[250];
      swprintf(wBuffer, sizeof(wBuffer), L"tree/f%07d.txt", context.ffxId);
      outputTree(root, wBuffer);
   };


   for(const std::wstring& fileName : getFileNamesInDir(originalDir)){
      ffxToJson(originalDir + fileName, jsonDir + fileName + L".json", testFunctions);
   }


   //FILE* file = _wfopen(L"P2ST8.txt", L"wb");
   //fwrite(outputText.data(), 1, outputText.size(), file);
   //fclose(file);


   system("pause");
}

void testing(){
   std::wstring allFfxDir = L"C:/Program Files (x86)/Steam/steamapps/common/Dark Souls Prepare to Die Edition/DATA-BR/sfx/Dark Souls (PC)/data/Sfx/OutputData/Main/Effect_win32/";


   //TestFunctions testFunctions;
   //for(int ffxId : {14428}){
   //   wchar_t wBuffer[250];
   //   swprintf(wBuffer, sizeof(wBuffer), L"%sf%07d.ffx", allFfxDir.c_str(), ffxId);
   //   std::wstring ffxPath = wBuffer;
   //   swprintf(wBuffer, sizeof(wBuffer), L"json/f%07d.ffx.json", ffxId);
   //   std::wstring jsonPath = wBuffer;
   //   ffxToJson(ffxPath, jsonPath, testFunctions);
   //}

   //importEveryFfx(allFfxDir, L"json/");
   importEveryFfxAndResearch(allFfxDir, L"json/");


   //for(int ffxId : {2125}){
   //   wchar_t wBuffer[250];
   //   swprintf(wBuffer, sizeof(wBuffer), L"json/f%07d.ffx.json", ffxId);
   //   std::wstring jsonPath = wBuffer;
   //   swprintf(wBuffer, sizeof(wBuffer), L"rebuilt/f%07d.ffx", ffxId);
   //   std::wstring ffxPath = wBuffer;
   //   jsonToFfx(jsonPath, ffxPath);
   //}

   //exportEveryFfxAndTest(allFfxDir, L"json/", L"rebuilt/");
}

void mainProgram(int argCount, wchar_t** args){
   --argCount;
   ++args;
   if(argCount < 1 || argCount > 2){
      argError(L"Invalid arg count");
   }

   std::wstring dir;
   std::wstring fileName;
   std::wstring extension;
   std::wstring inputPath;
   std::wstring outputPath;

   inputPath = args[0];
   getPathInfo(inputPath, dir, fileName);
   extension = getExtension(fileName);
   if(extension != L".ffx" && extension != L".json"){
      argError(L"inputPath is not an ffx or json file");
   }

   if(argCount == 1){
      if(extension == L".ffx"){
         outputPath = inputPath + L".json";
      }else{
         outputPath = inputPath;
         outputPath.resize(outputPath.size() - 5);
         if(getExtension(outputPath) != L".ffx"){
            outputPath += L".ffx";
         }
      }
   }else{
      outputPath = args[1];
   }

   wprintf_s(L"Converting %s to %s...\n", inputPath.c_str(), outputPath.c_str());
   if(extension == L".ffx"){
      ffxToJson(inputPath, outputPath);
   }else{
      jsonToFfx(inputPath, outputPath);
   }
   wprintf_s(L"Done.\n");
}

int wmain(int argCount, wchar_t** args) {
   //testing();
   mainProgram(argCount, args);
}
