#include "header.hpp"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

void getPathInfo(const std::wstring& path, std::wstring& dir, std::wstring& fileName){
   dir = L"./";
   fileName = path;

   for (int n = path.size(); n --> 0;) {
      if (path[n] == L'/' || path[n] == L'\\') {
         dir = path.substr(0, n + 1);
         fileName = &path[n + 1];
         break;
      }
   }
}

std::wstring getExtension(const std::wstring& path){
   const wchar_t* result = &path[path.size() - 1];
   while(*result != L'.' && result != &path[0]){
      --result;
   }

   return result;
}

std::vector<std::wstring> getFileNamesInDir(std::wstring path){
   std::vector<std::wstring> fileNames;

   wchar_t lastChar = path.back();
   if(lastChar != L'/' || lastChar != L'\\'){
      path += L"/";
   }

   path += L"*";

   WIN32_FIND_DATAW findData;
   HANDLE hFind = FindFirstFileW(
      path.c_str() ,
      &findData
   );

   if(hFind == INVALID_HANDLE_VALUE){
      throw;
   }

   if(!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
      fileNames.push_back(findData.cFileName);
   }

   while(FindNextFileW(hFind , &findData)){
      if(!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
         fileNames.push_back(std::move(findData.cFileName));
      }
   }

   FindClose(hFind);

   return std::move(fileNames);
}
