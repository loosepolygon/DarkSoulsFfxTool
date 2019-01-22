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


int main(int argCount, char** args) {
   std::wstring dir = L"C:/Program Files (x86)/Steam/steamapps/common/Dark Souls Prepare to Die Edition/DATA-BR/sfx/Dark Souls (PC)/data/Sfx/OutputData/Main/Effect_win32/";

   //for(int ffxId : {459}){
   //   wchar_t wBuffer[250];
   //   swprintf(wBuffer, sizeof(wBuffer), L"%sf%07d.ffx", dir.c_str(), ffxId);
   //   std::wstring ffxPath = wBuffer;
   //   swprintf(wBuffer, sizeof(wBuffer), L"json/f%07d.ffx.json", ffxId);
   //   std::wstring jsonPath = wBuffer;
   //   ffxToJson(ffxPath, jsonPath);
   //}

   //importEveryFfx(dir);


   //for(int ffxId : {2125}){
   //   wchar_t wBuffer[250];
   //   swprintf(wBuffer, sizeof(wBuffer), L"json/f%07d.ffx.json", ffxId);
   //   std::wstring jsonPath = wBuffer;
   //   swprintf(wBuffer, sizeof(wBuffer), L"rebuilt/f%07d.ffx", ffxId);
   //   std::wstring ffxPath = wBuffer;
   //   jsonToFfx(jsonPath, ffxPath);
   //}

   //exportEveryFfxAndTest(dir, L"json/", L"rebuilt/");


   system("pause");
}
