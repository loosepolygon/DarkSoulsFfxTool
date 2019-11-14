#include "header.hpp"

class ValueAnalyzer{
   std::vector<float> data;
   std::vector<std::pair<float, int>> mostCommon; // value, count
public:
   int count = 0;
   float min = std::numeric_limits<float>::max();
   float max = std::numeric_limits<float>::min();
   std::vector<float> mostCommon5;

   void feed(int v){
      this->data.push_back((float)v);
   }

   void feed(float v){
      this->data.push_back(v);
   }

   void analyze(){
      this->count = this->data.size();

      for(float v : this->data){
         if(v < this->min) this->min = v;
         if(v > this->max) this->max = v;

         int mostCommon10Index = findIndex(
            this->mostCommon,
            [&](std::pair<float, int>& pair) -> bool{
            return pair.first == v;
         }
         );
         if(mostCommon10Index == -1){
            this->mostCommon.emplace_back(v, 0);
            mostCommon10Index = this->mostCommon.size() - 1;
         }
         ++this->mostCommon[mostCommon10Index].second;
      }

      std::sort(
         this->mostCommon.begin(),
         this->mostCommon.end(),
         [&](auto& a, auto& b) -> bool{
         return a.second > b.second;
      }
      );

      for(auto& pair : this->mostCommon){
         this->mostCommon5.push_back(pair.first);
         if(this->mostCommon5.size() >= 5) break;
      }

      int bp=42;
   }
};

class ArrayAnalyzer{
   std::vector<std::vector<float>> data;
   std::vector<std::pair<std::vector<float>*, float>> sums; // array, sum
public:
   ValueAnalyzer arraySize;
   std::vector<float> smallestSum;
   std::vector<float> largestSum;
   std::vector<float> medianSum;
   std::vector<std::pair<float, float>> valueRanges; // min, max

   void feed(const std::vector<float>& values){
      this->arraySize.feed((float)values.size());
      this->data.push_back(values);
   }

   void analyze(){
      // arraySize
      this->arraySize.analyze();

      // Sums
      for(auto& a : this->data){
         float sum = std::accumulate(a.begin(), a.end(), 0.0f);
         this->sums.emplace_back(&a, sum);
      }
      std::sort(
         this->sums.begin(),
         this->sums.end(),
         [&](auto& a, auto& b) -> bool{
         return a.second < b.second;
      }
      );
      this->smallestSum = *this->sums.front().first;
      this->largestSum = *this->sums.back().first;
      this->medianSum = *this->sums[this->sums.size() / 2].first; // not really median but whatever

      // valueRanges
      for(auto& a : this->data){
         for(size_t n = 0; n < a.size(); ++n){
            if(this->valueRanges.size() < n + 1){
               this->valueRanges.emplace_back(
                  std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::min()
               );
            }

            if(a[n] < this->valueRanges[n].first) this->valueRanges[n].first = a[n];
            if(a[n] > this->valueRanges[n].second) this->valueRanges[n].second = a[n];
         }
      }

      int bp=42;
   }
};

//enum class DataType : int{
//   data3,
//   pond1,
//   pond2,
//   pond3,
//   pond2Subtype,
//};

enum class AnalyzerType{
   value,
   array,
};

struct SpreadsheetRow{
   std::string name;
   AnalyzerType type;
   ValueAnalyzer valueAnalyzer;
   ArrayAnalyzer arrayAnalyzer;

   SpreadsheetRow(std::string name, AnalyzerType type) : name(name), type(type){}
};

struct SpreadsheetTable{
   std::string name;
   int typeId = -1;
   std::vector<SpreadsheetRow> rows;

   int count = 0;

   SpreadsheetTable(const std::string& name, int typeId, const std::vector<SpreadsheetRow>& rows) :
      name(name), typeId(typeId), rows(rows)
   {}

   SpreadsheetTable(const std::string& name, const std::vector<SpreadsheetRow>& rows) :
      name(name), rows(rows)
   {}
};


SpreadsheetTable stPond3T0 = {
   "Pond3 Type0", 0, {
      {"astCountMaybe", AnalyzerType::value},
   }
};
SpreadsheetTable stPond3T1 = {
   "Pond3 Type1", 1, {
      {"unk1", AnalyzerType::value},
      {"unk2", AnalyzerType::value},
      {"unk3", AnalyzerType::value},
      {"offsetX", AnalyzerType::value},
      {"offsetY", AnalyzerType::value},
      {"unk6", AnalyzerType::value},
      {"unk7", AnalyzerType::value},
      {"unk8", AnalyzerType::value},
      {"unk9", AnalyzerType::value},
      {"unk10", AnalyzerType::value},
      {"unk11", AnalyzerType::value},
   }
};
SpreadsheetTable stPond3T2 = {
   "Pond3 Type2", 2, {
      {"lifetime", AnalyzerType::value},
      {"unk2", AnalyzerType::value},
      {"unk3", AnalyzerType::value},
      {"unk4", AnalyzerType::value},
      {"unk5", AnalyzerType::value},
      {"unk6", AnalyzerType::value},
   }
};
SpreadsheetTable stPond3T3 = {
   "Pond3 Type3", 3, {
      {"unk1", AnalyzerType::value},
      {"unk2", AnalyzerType::value},
      {"unk3", AnalyzerType::value},
      {"unk4", AnalyzerType::value},
   }
};
SpreadsheetTable stPond3T4 = {
   "Pond3 Type4", 4, {
      {"unk1", AnalyzerType::value},
   }
};
SpreadsheetTable stPond3T5 = {
   "Pond3 Type5", 5, {
      {"unk1", AnalyzerType::value},
   }
};
SpreadsheetTable stPond3T6 = {
   "Pond3 Type6", 6, {
      {"unk1", AnalyzerType::value},
      {"unk2", AnalyzerType::value},
   }
};
SpreadsheetTable stPond3T7 = {
   "Pond3 Type7", 7, {
      {"unk1", AnalyzerType::value},
   }
};


std::vector<SpreadsheetTable*> pond3Tables = {
   &stPond3T0,
   &stPond3T1,
   &stPond3T2,
   &stPond3T3,
   &stPond3T4,
   &stPond3T5,
   &stPond3T6,
   &stPond3T7,
};


void outputTable(std::string& csvText, SpreadsheetTable& table){
   char buffer[512];

   csvText += table.name + '\n';
   snprintf(buffer, sizeof(buffer), "Count:,%i\n\n", table.count);
   csvText += buffer;
   csvText += "var,min,max,common1,common2,common3,common4,common5\n";

   for(SpreadsheetRow& row : table.rows){
      if(row.type == AnalyzerType::value){
         row.valueAnalyzer.analyze();

         snprintf(
            buffer,
            sizeof(buffer),
            "%s,%f,%f",
            row.name.c_str(),
            row.valueAnalyzer.min,
            row.valueAnalyzer.max
         );
         csvText += buffer;

         std::vector<float>& mostCommon5 = row.valueAnalyzer.mostCommon5;
         for(size_t n = 0; n < 5; ++n){
            if(n < mostCommon5.size()){
               snprintf(buffer, sizeof(buffer), ",%f", mostCommon5[n]);
               csvText += buffer;
            }else{
               csvText += ",";
            }
         }
      }else if(row.type == AnalyzerType::array){
      }

      csvText += "\n";
   }
}


void generateAnalysisSpreadsheet(std::wstring originalDir, std::wstring jsonDir){
   TestFunctions testFunctions;

   // Testing features

   //ValueAnalyzer testAnalyzer;
   //testFunctions.onPond3 = [&](json::JSON& obj, TestFunctions::Context context) {
   //   if(obj["pond3Type"].ToInt() == 1){
   //      testAnalyzer.feed(obj["offsetX"].ToFloat());
   //   }
   //};

   ArrayAnalyzer testArrayAnalyzer;
   std::vector<float> buffer;
   testFunctions.onData3 = [&](json::JSON& obj, TestFunctions::Context context) {
      if(obj["data3Type"].ToInt() == 5){
         //buffer.resize(0);
         //for(json::JSON& j : obj["floats"].ArrayRange()){
         //   buffer.push_back(j.ToFloat());
         //}

         //testArrayAnalyzer.feed(buffer);
      }
   };

   //testFunctions.onRoot = [&](json::JSON& obj, TestFunctions::Context context) {
   //   testArrayAnalyzer.analyze();
   //};

   // Make a spreadsheet segment

   std::vector<float> arrayBuffer;
   testFunctions.onPond3 = [&](json::JSON& obj, TestFunctions::Context context){
      int pond3Type = obj["pond3Type"].ToInt();

      bool found = false;
      for(SpreadsheetTable* table : pond3Tables){
         if(table->typeId == pond3Type){
            found = true;

            ++table->count;

            for(SpreadsheetRow& row : table->rows){
               json::JSON& jsonVar = obj[row.name];
               if(row.type == AnalyzerType::value){
                  if(jsonVar.JSONType() == json::JSON::Class::Integral){
                     row.valueAnalyzer.feed((float)jsonVar.ToInt());
                  }else{
                     row.valueAnalyzer.feed(jsonVar.ToFloat());
                  }
               }else if(row.type == AnalyzerType::array){
                  for(json::JSON& j : jsonVar.ArrayRange()){
                     if(j.JSONType() == json::JSON::Class::Integral){
                        arrayBuffer.push_back((float)j.ToInt());
                     }else{
                        arrayBuffer.push_back(j.ToFloat());
                     }
                  }
                  row.arrayAnalyzer.feed(arrayBuffer);
                  arrayBuffer.resize(0);
               }
            }

            break;
         }
      }
      if(found == false){
         throw;
      }
   };

   for(const std::wstring& fileName : getFileNamesInDir(originalDir)){
      ffxToJson(originalDir + fileName, jsonDir + fileName + L".json", testFunctions);
   }

   // Output csv files to spreadsheet/

   // Pond3
   {
      std::string csvText;
      csvText += "\n\n\n\n\n";
      for(auto table : pond3Tables){
         outputTable(csvText, *table);
         csvText += "\n\n\n\n";
      }

      FILE* file = _wfopen(L"spreadsheet/pond3.csv", L"w");
      if(file == NULL) {
         throw;
      }
      fwrite(csvText.data(), 1, csvText.size(), file);
      fclose(file);
   }

   int bp=42;
}
