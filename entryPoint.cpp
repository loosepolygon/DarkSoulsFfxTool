#include "header.hpp"

int jsonVersion = 5;

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
   throw;
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
