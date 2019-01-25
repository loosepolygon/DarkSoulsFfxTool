#include "header.hpp"
#include "md5.hpp"

int jsonVersion = 1;

void ffxReadError(const std::wstring& path, const std::wstring& text){
   std::wstring errorMessage = L"Error reading file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());

   throw;
}

void ffxWriteError(const std::wstring& path, const std::wstring& text){
   std::wstring errorMessage = L"Error writing ffx file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());
   throw;
}

void jsonReadError(const std::wstring& path, const std::wstring& text){
   std::wstring errorMessage = L"Error reading json file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());
   throw;
}

void jsonWriteError(const std::wstring& path, const std::wstring& text){
   std::wstring errorMessage = L"Error writing json file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());
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

void importEveryFfx(std::wstring dir){
   std::vector<std::wstring> fileList;
   {
      std::wstring fileListPath = dir + L"fileListFull.txt";
      FILE* file = _wfopen(fileListPath.c_str(), L"r");

      fseek(file, 0, SEEK_END);
      long fileSize = ftell(file);
      fseek(file, 0, SEEK_SET);

      wchar_t endianness;
      fread(&endianness, sizeof(wchar_t), 1, file);

      std::wstring fullText;
      fullText.resize(fileSize - sizeof(wchar_t));
      fread(&fullText[0], sizeof(fullText[0]), fullText.size(), file);
      fclose(file);

      std::wstringstream ss(fullText);
      std::wstring item;
      while (std::getline(ss, item, L'\n')) {
         if(item[0] != '\0'){
            fileList.push_back(item);
         }
      }
   }

   system("mkdir json");
   for(const std::wstring& fileName : fileList){
      ffxToJson(dir + fileName, L"json/" + fileName + L".json");
   }
}

void exportEveryFfxAndTest(std::wstring originalDir, std::wstring jsonDir, std::wstring ffxDir){
   std::vector<std::wstring> fileList;
   {
      std::wstring fileListPath = originalDir + L"fileListFull.txt";
      FILE* file = _wfopen(fileListPath.c_str(), L"r");

      fseek(file, 0, SEEK_END);
      long fileSize = ftell(file);
      fseek(file, 0, SEEK_SET);

      wchar_t endianness;
      fread(&endianness, sizeof(wchar_t), 1, file);

      std::wstring fullText;
      fullText.resize(fileSize - sizeof(wchar_t));
      fread(&fullText[0], sizeof(fullText[0]), fullText.size(), file);
      fclose(file);

      std::wstringstream ss(fullText);
      std::wstring item;
      while (std::getline(ss, item, L'\n')) {
         if(item[0] != '\0'){
            fileList.push_back(item);
         }
      }
   }

   for(const std::wstring& fileName : fileList){
      wprintf(L"%s\n", fileName.c_str());

      jsonToFfx(jsonDir + fileName + L".json", ffxDir + fileName);

      std::string md5Original = getMD5(originalDir + fileName);
      std::string md5Rebuilt = getMD5(ffxDir + fileName);

      if(md5Original != md5Rebuilt){
         throw;
      }
   }
}


int wmain(int argCount, wchar_t** args) {
   //std::wstring testDir = L"C:/Program Files (x86)/Steam/steamapps/common/Dark Souls Prepare to Die Edition/DATA-BR/sfx/Dark Souls (PC)/data/Sfx/OutputData/Main/Effect_win32/";

   //for(int ffxId : {459}){
   //   wchar_t wBuffer[250];
   //   swprintf(wBuffer, sizeof(wBuffer), L"%sf%07d.ffx", testDir.c_str(), ffxId);
   //   std::wstring ffxPath = wBuffer;
   //   swprintf(wBuffer, sizeof(wBuffer), L"json/f%07d.ffx.json", ffxId);
   //   std::wstring jsonPath = wBuffer;
   //   ffxToJson(ffxPath, jsonPath);
   //}

   //importEveryFfx(testDir);


   //for(int ffxId : {2125}){
   //   wchar_t wBuffer[250];
   //   swprintf(wBuffer, sizeof(wBuffer), L"json/f%07d.ffx.json", ffxId);
   //   std::wstring jsonPath = wBuffer;
   //   swprintf(wBuffer, sizeof(wBuffer), L"rebuilt/f%07d.ffx", ffxId);
   //   std::wstring ffxPath = wBuffer;
   //   jsonToFfx(jsonPath, ffxPath);
   //}

   //exportEveryFfxAndTest(testDir, L"json/", L"rebuilt/");

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

   // inputPath
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

   //system("pause");
}
