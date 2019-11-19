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
   std::vector<std::pair<std::vector<float>*, int>> lengths; // array, length
public:
   ValueAnalyzer arraySize;
   std::vector<float> smallestSum;
   std::vector<float> largestSum;
   std::vector<float> medianSum;
   std::vector<float> shortest;
   std::vector<float> longest;
   std::vector<std::pair<float, float>> valueRanges; // min, max

   void feed(const std::vector<float>& values){
      this->arraySize.feed((float)values.size());
      this->data.push_back(values);
   }

   void analyze(){
      // arraySize
      this->arraySize.analyze();

      for(auto& a : this->data){
         float sum = std::accumulate(a.begin(), a.end(), 0.0f);
         this->sums.emplace_back(&a, sum);

         this->lengths.emplace_back(&a, a.size());
      }

      // Sums
      std::sort(
         this->sums.begin(),
         this->sums.end(),
         [&](auto& a, auto& b) -> bool{
            return a.second < b.second;
         }
      );
      this->smallestSum = *this->sums.front().first;
      this->largestSum = *this->sums.back().first;
      this->medianSum = *this->sums[this->sums.size() / 2].first; // not truly median but whatever

      // Lengths
      std::sort(
         this->lengths.begin(),
         this->lengths.end(),
         [&](auto& a, auto& b) -> bool{
            return a.second < b.second;
         }
      );
      this->shortest = *this->lengths.front().first;
      this->longest = *this->lengths.back().first;

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


SpreadsheetTable stData3T112 = {"Data3 Type112", 112, {{}}};
SpreadsheetTable stData3T113 = {"Data3 Type113", 113, {{}}};
SpreadsheetTable stData3T129 = {"Data3 Type129", 129, {{}}};
SpreadsheetTable stData3T130 = {"Data3 Type130", 130, {{}}};
SpreadsheetTable stData3T131 = {"Data3 Type131", 131, {{}}};
SpreadsheetTable stData3T132 = {"Data3 Type132", 132, {{}}};
SpreadsheetTable stData3T136 = {"Data3 Type136", 136, {{}}};

SpreadsheetTable stData3T1 = {"Data3 Type1", 1, {{"unk1", AnalyzerType::value}}};
SpreadsheetTable stData3T41 = {"Data3 Type41", 41, {{"unk1", AnalyzerType::value}}};
SpreadsheetTable stData3T59 = {"Data3 Type59", 59, {{"unk1", AnalyzerType::value}}};
SpreadsheetTable stData3T66 = {"Data3 Type66", 66, {{"unk1", AnalyzerType::value}}};
SpreadsheetTable stData3T68 = {"Data3 Type68", 68, {{"unk1", AnalyzerType::value}}};
SpreadsheetTable stData3T69 = {"Data3 Type69", 69, {{"unk1", AnalyzerType::value}}};
SpreadsheetTable stData3T111 = {"Data3 Type111", 111, {{"unk1", AnalyzerType::value}}};
SpreadsheetTable stData3T138 = {"Data3 Type138", 138, {{"unk1", AnalyzerType::value}}};
SpreadsheetTable stData3T139 = {"Data3 Type139", 139, {{"unk1", AnalyzerType::value}}};

SpreadsheetTable stData3T7 = {"Data3 Type7", 7, {{"unk1", AnalyzerType::value}}};
SpreadsheetTable stData3T70 = {"Data3 Type70", 70, {{"unk1", AnalyzerType::value}}};

SpreadsheetTable stData3T79 = {"Data3 Type79", 79, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};
SpreadsheetTable stData3T140 = {"Data3 Type140", 140, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};

SpreadsheetTable stData3T81 = {"Data3 Type81", 81, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};
SpreadsheetTable stData3T85 = {"Data3 Type85", 85, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};

SpreadsheetTable stData3T137 = {"Data3 Type137", 137, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
   {"unk3", AnalyzerType::value},
}};

SpreadsheetTable stData3T2 = {"Data3 Type2", 2, {
   {"bunchaInts", AnalyzerType::array},
}};

SpreadsheetTable stData3T3 = {"Data3 Type3", 3, {
   {"floats", AnalyzerType::array},
   {"ints", AnalyzerType::array},
}};
SpreadsheetTable stData3T5 = {"Data3 Type5", 5, {
   {"floats", AnalyzerType::array},
   {"ints", AnalyzerType::array},
}};
SpreadsheetTable stData3T6 = {"Data3 Type6", 6, {
   {"floats", AnalyzerType::array},
   {"ints", AnalyzerType::array},
}};
SpreadsheetTable stData3T9 = {"Data3 Type9", 9, {
   {"floats", AnalyzerType::array},
   {"ints", AnalyzerType::array},
}};
SpreadsheetTable stData3T89 = {"Data3 Type89", 89, {
   {"floats", AnalyzerType::array},
   {"ints", AnalyzerType::array},
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};

SpreadsheetTable stData3T11 = {"Data3 Type11", 11, {
   {"floatsA", AnalyzerType::array},
   {"floatsB", AnalyzerType::array},
}};
SpreadsheetTable stData3T12 = {"Data3 Type12", 12, {
   {"floatsA", AnalyzerType::array},
   {"floatsB", AnalyzerType::array},
}};
SpreadsheetTable stData3T91 = {"Data3 Type91", 91, {
   {"floatsA", AnalyzerType::array},
   {"floatsB", AnalyzerType::array},
   {"unk1", AnalyzerType::value},
   {"data3 type", AnalyzerType::value},
}};
SpreadsheetTable stData3T95 = {"Data3 Type91", 95, {
   {"floatsA", AnalyzerType::array},
   {"floatsB", AnalyzerType::array},
   {"unk1", AnalyzerType::value},
   {"data3 type", AnalyzerType::value},
}};

SpreadsheetTable stData3T13 = {"Data3 Type13", 13, {
   {"floatsA", AnalyzerType::array},
   {"floatsB", AnalyzerType::array},
}};
SpreadsheetTable stData3T14 = {"Data3 Type14", 14, {
   {"floatsA", AnalyzerType::array},
   {"floatsB", AnalyzerType::array},
}};

SpreadsheetTable stData3T19 = {"Data3 Type19", 19, {
   {"floatsA", AnalyzerType::array},
   {"floatsB", AnalyzerType::array},
}};
SpreadsheetTable stData3T20 = {"Data3 Type20", 20, {
   {"floatsA", AnalyzerType::array},
   {"floatsB", AnalyzerType::array},
}};
SpreadsheetTable stData3T27 = {"Data3 Type27", 27, {
   {"floatsA", AnalyzerType::array},
   {"floatsB", AnalyzerType::array},
}};
SpreadsheetTable stData3T28 = {"Data3 Type28", 28, {
   {"floatsA", AnalyzerType::array},
   {"floatsB", AnalyzerType::array},
}};

SpreadsheetTable stData3T21 = {"Data3 Type21", 21, {
   {"floatsA", AnalyzerType::array},
   {"floatsB", AnalyzerType::array},
}};
SpreadsheetTable stData3T22 = {"Data3 Type22", 22, {
   {"floatsA", AnalyzerType::array},
   {"floatsB", AnalyzerType::array},
}};
SpreadsheetTable stData3T29 = {"Data3 Type29", 29, {
   {"floatsA", AnalyzerType::array},
   {"floatsB", AnalyzerType::array},
}};

SpreadsheetTable stData3T37 = {"Data3 Type37", 37, {
   {"templateFfxId", AnalyzerType::value},
}};
SpreadsheetTable stData3T38 = {"Data3 Type38", 38, {
   {"t38Subtype", AnalyzerType::value},
}};

SpreadsheetTable stData3T44 = {"Data3 Type44", 44, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};
SpreadsheetTable stData3T45 = {"Data3 Type45", 45, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};
SpreadsheetTable stData3T46 = {"Data3 Type46", 46, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};
SpreadsheetTable stData3T47 = {"Data3 Type47", 47, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};
SpreadsheetTable stData3T60 = {"Data3 Type60", 60, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};
SpreadsheetTable stData3T71 = {"Data3 Type71", 71, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};
SpreadsheetTable stData3T87 = {"Data3 Type87", 87, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};
SpreadsheetTable stData3T114 = {"Data3 Type114", 114, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};
SpreadsheetTable stData3T115 = {"Data3 Type115", 115, {
   {"unk1", AnalyzerType::value},
   {"unk2", AnalyzerType::value},
}};

SpreadsheetTable stData3T128 = {"Data3 Type128", 128, {
   {"data3 type", AnalyzerType::value},
}};

SpreadsheetTable stData3T120 = {"Data3 Type120", 120, {
   {"data3A type", AnalyzerType::value},
   {"data3B type", AnalyzerType::value},
}};
SpreadsheetTable stData3T121 = {"Data3 Type121", 121, {
   {"data3A type", AnalyzerType::value},
   {"data3B type", AnalyzerType::value},
}};
SpreadsheetTable stData3T122 = {"Data3 Type122", 122, {
   {"data3A type", AnalyzerType::value},
   {"data3B type", AnalyzerType::value},
}};
SpreadsheetTable stData3T123 = {"Data3 Type123", 123, {
   {"data3A type", AnalyzerType::value},
   {"data3B type", AnalyzerType::value},
}};
SpreadsheetTable stData3T124 = {"Data3 Type124", 124, {
   {"data3A type", AnalyzerType::value},
   {"data3B type", AnalyzerType::value},
}};
SpreadsheetTable stData3T126 = {"Data3 Type126", 126, {
   {"data3A type", AnalyzerType::value},
   {"data3B type", AnalyzerType::value},
}};
SpreadsheetTable stData3T127 = {"Data3 Type127", 127, {
   {"data3A type", AnalyzerType::value},
   {"data3B type", AnalyzerType::value},
}};


std::vector<SpreadsheetTable*> data3Tables = {
   &stData3T112,
   &stData3T113,
   &stData3T129,
   &stData3T130,
   &stData3T131,
   &stData3T132,
   &stData3T136,
   &stData3T1,
   &stData3T41,
   &stData3T59,
   &stData3T66,
   &stData3T68,
   &stData3T69,
   &stData3T111,
   &stData3T138,
   &stData3T139,
   &stData3T7,
   &stData3T70,
   &stData3T79,
   &stData3T140,
   &stData3T81,
   &stData3T85,
   &stData3T137,
   &stData3T2,
   &stData3T3,
   &stData3T5,
   &stData3T6,
   &stData3T9,
   &stData3T89,
   &stData3T11,
   &stData3T12,
   &stData3T91,
   &stData3T95,
   &stData3T13,
   &stData3T14,
   &stData3T19,
   &stData3T20,
   &stData3T27,
   &stData3T28,
   &stData3T21,
   &stData3T22,
   &stData3T29,
   &stData3T37,
   &stData3T38,
   &stData3T44,
   &stData3T45,
   &stData3T46,
   &stData3T47,
   &stData3T60,
   &stData3T71,
   &stData3T87,
   &stData3T114,
   &stData3T115,
   &stData3T128,
   &stData3T120,
   &stData3T121,
   &stData3T122,
   &stData3T123,
   &stData3T124,
   &stData3T126,
   &stData3T127
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
   snprintf(buffer, sizeof(buffer), "Count:,%i\n", table.count);
   csvText += buffer;

   if(table.rows.empty()){
      csvText += "(no vars)\n";

      return;
   }
   if(table.count == 0){
      csvText += "\n";

      return;
   }

   csvText += '\n';
   csvText += "var,min,max,common1,common2,common3,common4,common5";

   // Analyze data and add relevant array columns
   bool usesArrays = false;
   size_t maxArrayLength = 0;
   for(SpreadsheetRow& row : table.rows){
      if(row.type == AnalyzerType::value){
         row.valueAnalyzer.analyze();
      }else if(row.type == AnalyzerType::array){
         row.arrayAnalyzer.analyze();

         usesArrays = true;
         if(row.arrayAnalyzer.longest.size() > maxArrayLength){
            maxArrayLength = row.arrayAnalyzer.longest.size();
         }
      }
   }
   if(usesArrays){
      csvText += ",smallestSum,medianSum,largestSum,shortest,longest";

      for(size_t n = 0; n < maxArrayLength; ++n){
         snprintf(buffer, sizeof(buffer), ",range%i", n);
         csvText += buffer;
      }
   }

   csvText += '\n';

   for(SpreadsheetRow& row : table.rows){
      if(row.type == AnalyzerType::value){
         snprintf(
            buffer,
            sizeof(buffer),
            "%s,%g,%g",
            row.name.c_str(),
            row.valueAnalyzer.min,
            row.valueAnalyzer.max
         );
         csvText += buffer;

         std::vector<float>& mostCommon5 = row.valueAnalyzer.mostCommon5;
         for(size_t n = 0; n < 5; ++n){
            if(n < mostCommon5.size()){
               snprintf(buffer, sizeof(buffer), ",%g", mostCommon5[n]);
               csvText += buffer;
            }else{
               csvText += ",";
            }
         }
      }else if(row.type == AnalyzerType::array){
         // Array size

         snprintf(
            buffer,
            sizeof(buffer),
            "%s,%g,%g",
            row.name.c_str(),
            row.arrayAnalyzer.arraySize.min,
            row.arrayAnalyzer.arraySize.max
         );
         csvText += buffer;

         std::vector<float>& mostCommon5 = row.arrayAnalyzer.arraySize.mostCommon5;
         for(size_t n = 0; n < 5; ++n){
            if(n < mostCommon5.size()){
               snprintf(buffer, sizeof(buffer), ",%g", mostCommon5[n]);
               csvText += buffer;
            }else{
               csvText += ",";
            }
         }

         // Array examples

         auto outputArray = [&](std::vector<float>& values){
            csvText += ",\"[";
            for(size_t n = 0; n < values.size(); ++n){
               sprintf(buffer, "%g", values[n]);
               csvText += buffer;
               if(n != values.size() - 1){
                  csvText += ",";
               }
            }
            csvText += "]\"";
         };

         outputArray(row.arrayAnalyzer.smallestSum);
         outputArray(row.arrayAnalyzer.medianSum);
         outputArray(row.arrayAnalyzer.largestSum);
         outputArray(row.arrayAnalyzer.shortest);
         outputArray(row.arrayAnalyzer.longest);
         
         // Array ranges

         for(auto& pair : row.arrayAnalyzer.valueRanges){
            snprintf(buffer, sizeof(buffer), ",%g - %g", pair.first, pair.second);
            csvText += buffer;
         }
      }

      csvText += "\n";
   }
}


void generateAnalysisSpreadsheet(std::wstring originalDir, std::wstring jsonDir){
   TestFunctions testFunctions;

   std::vector<float> arrayBuffer;

   testFunctions.onData3 = [&](json::JSON& obj, TestFunctions::Context context) {
      int data3Type = obj["data3Type"].ToInt();

      bool found = false;
      for(SpreadsheetTable* table : data3Tables){
         if(table->typeId == data3Type){
            found = true;

            ++table->count;

            for(SpreadsheetRow& row : table->rows){
               if(row.name == "data3 type"){
                  row.valueAnalyzer.feed((float)obj["data3"]["data3Type"].ToInt());
               }else if(row.name == "data3A type"){
                  row.valueAnalyzer.feed((float)obj["data3A"]["data3Type"].ToInt());
               }else if(row.name == "data3B type"){
                  row.valueAnalyzer.feed((float)obj["data3B"]["data3Type"].ToInt());
               }else if(row.type == AnalyzerType::value){
                  json::JSON& jsonVar = obj[row.name];

                  if(jsonVar.JSONType() == json::JSON::Class::Integral){
                     row.valueAnalyzer.feed((float)jsonVar.ToInt());
                  }else{
                     row.valueAnalyzer.feed(jsonVar.ToFloat());
                  }
               }else if(row.type == AnalyzerType::array){
                  json::JSON& jsonVar = obj[row.name];

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

   // Data3
   {
      std::string csvText;
      csvText += "\n\n\n\n\n";
      for(auto table : data3Tables){
         outputTable(csvText, *table);
         csvText += "\n\n\n\n";
      }

      FILE* file = _wfopen(L"spreadsheet/data3.csv", L"w");
      if(file == NULL) {
         throw;
      }
      fwrite(csvText.data(), 1, csvText.size(), file);
      fclose(file);
   }

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
