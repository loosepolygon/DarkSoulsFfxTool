#include <iostream>
#include <vector>
#include <sstream>

typedef unsigned char byte;

void ffxReadError(const std::wstring& path, const std::wstring& text){
   std::wstring errorMessage = L"Error reading file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());
   throw;
}

void ffxAssumptionWrong(const std::wstring& path, const std::wstring& text){
   const wchar_t* filePath = &path[path.size() - 1];
   while(--filePath != path.data()){
      if(*filePath == L'/'){
         ++filePath;
         break;
      }
   }
   std::wstring errorMessage = L"FFX assumption proved wrong for file '" + path + L"': " + text;
   fwprintf_s(stderr, L"%s\n", errorMessage.c_str());
   throw;
}

struct RawFfxFile{
   struct Header{
      char signature[4];
      int version;
      int preDataStart;
      int data2Start;
      int data2Count;
      int data3Count;
      int unk1;
      int unk2;

      int unknownCount;
      int ffxId;

      byte unknownSection[80];
   } header;

   std::vector<byte> data1;
   std::vector<int> data2;
   std::vector<int> data3;
};

void loadRawFfxFile(const std::wstring& path, RawFfxFile* rawFfx){
   memset(rawFfx, 0, sizeof(RawFfxFile));

   FILE* file = _wfopen(path.c_str(), L"rb");
   if (file == NULL) {
      ffxReadError(path, L"Cannot open file");
      return;
   }

   fseek(file, 0, SEEK_END);
   long fileSize = ftell(file);
   fseek(file, 0, SEEK_SET);

   fread(&rawFfx->header, 1, sizeof(RawFfxFile::Header), file);
   char signature[4] = "FXR";
   if(memcmp(signature, rawFfx->header.signature, 4) != 0){
      ffxReadError(path, L"Not an FFX file");
   }

   int data1Size = rawFfx->header.data2Start - sizeof(RawFfxFile::Header);
   rawFfx->data1.resize(data1Size);
   fread(rawFfx->data1.data(), 1, rawFfx->data1.size(), file);

   rawFfx->data2.resize(rawFfx->header.data2Count);
   fread(rawFfx->data2.data(), 4, rawFfx->data2.size(), file);

   rawFfx->data3.resize(rawFfx->header.data3Count);
   fread(rawFfx->data3.data(), 4, rawFfx->data3.size(), file);

   if(ftell(file) != fileSize){
      ffxReadError(path, L"Still more file to read");
   }

   fclose(file);
}

void testEveryFfx(std::wstring dir){
   std::vector<std::wstring> fileList;
   {
      std::wstring fileListPath = dir + L"fileList.txt";
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
         fileList.push_back(item);
      }
   }

   RawFfxFile rawFfx;
   int index = 1;
   for(const std::wstring& path : fileList){
      //wprintf_s(L"File %d/%d\n", index, fileList.size());
      ++index;

      loadRawFfxFile(dir + path, &rawFfx);
   }
}

int main(int argCount, char** args) {
   printf("Hello\n");

   //RawFfxFile rawFfx;
   //std::wstring path = L"C:/Projects/Dark Souls/FFX research/";
   //path += L"f0000482.ffx";
   //loadRawFfxFile(path, &rawFfx);

   testEveryFfx(L"C:/Program Files (x86)/Steam/steamapps/common/Dark Souls Prepare to Die Edition/DATA-BR/sfx/Dark Souls (PC)/data/Sfx/OutputData/Main/Effect_win32/");
}
