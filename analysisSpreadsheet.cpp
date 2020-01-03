#include "header.hpp"

class ValueAnalyzer{
   std::vector<float> data;
public:
   int count = 0;
   float min = std::numeric_limits<float>::max();
   float max = -min;
   std::vector<std::pair<float, int>> mostCommon; // value, count

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
   }
};

enum class ValueType{
   value,
   array,
   data3,
   pond2Subtype,
};

struct SpreadsheetRow{
   std::string name;
   ValueType type;
   ValueAnalyzer valueAnalyzer;
   ArrayAnalyzer arrayAnalyzer;

   SpreadsheetRow(std::string name, ValueType type) : name(name), type(type){}
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

SpreadsheetTable stData3T1 = {"Data3 Type1", 1, {{"unk1", ValueType::value}}};
SpreadsheetTable stData3T41 = {"Data3 Type41", 41, {{"unk1", ValueType::value}}};
SpreadsheetTable stData3T59 = {"Data3 Type59", 59, {{"unk1", ValueType::value}}};
SpreadsheetTable stData3T66 = {"Data3 Type66", 66, {{"unk1", ValueType::value}}};
SpreadsheetTable stData3T68 = {"Data3 Type68", 68, {{"unk1", ValueType::value}}};
SpreadsheetTable stData3T69 = {"Data3 Type69", 69, {{"unk1", ValueType::value}}};
SpreadsheetTable stData3T111 = {"Data3 Type111", 111, {{"unk1", ValueType::value}}};
SpreadsheetTable stData3T138 = {"Data3 Type138", 138, {{"unk1", ValueType::value}}};
SpreadsheetTable stData3T139 = {"Data3 Type139", 139, {{"unk1", ValueType::value}}};

SpreadsheetTable stData3T7 = {"Data3 Type7", 7, {{"unk1", ValueType::value}}};
SpreadsheetTable stData3T70 = {"Data3 Type70", 70, {{"unk1", ValueType::value}}};

SpreadsheetTable stData3T79 = {"Data3 Type79", 79, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};
SpreadsheetTable stData3T140 = {"Data3 Type140", 140, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};

SpreadsheetTable stData3T81 = {"Data3 Type81", 81, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};
SpreadsheetTable stData3T85 = {"Data3 Type85", 85, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};

SpreadsheetTable stData3T137 = {"Data3 Type137", 137, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
   {"unk3", ValueType::value},
}};

SpreadsheetTable stData3T2 = {"Data3 Type2", 2, {
   {"bunchaInts", ValueType::array},
}};

SpreadsheetTable stData3T3 = {"Data3 Type3", 3, {
   {"floats", ValueType::array},
   {"ints", ValueType::array},
}};
SpreadsheetTable stData3T5 = {"Data3 Type5", 5, {
   {"floats", ValueType::array},
   {"ints", ValueType::array},
}};
SpreadsheetTable stData3T6 = {"Data3 Type6", 6, {
   {"floats", ValueType::array},
   {"ints", ValueType::array},
}};
SpreadsheetTable stData3T9 = {"Data3 Type9", 9, {
   {"floats", ValueType::array},
   {"ints", ValueType::array},
}};
SpreadsheetTable stData3T89 = {"Data3 Type89", 89, {
   {"floats", ValueType::array},
   {"ints", ValueType::array},
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};

SpreadsheetTable stData3T11 = {"Data3 Type11", 11, {
   {"floatsA", ValueType::array},
   {"floatsB", ValueType::array},
}};
SpreadsheetTable stData3T12 = {"Data3 Type12", 12, {
   {"floatsA", ValueType::array},
   {"floatsB", ValueType::array},
}};
SpreadsheetTable stData3T91 = {"Data3 Type91", 91, {
   {"floatsA", ValueType::array},
   {"floatsB", ValueType::array},
   {"unk1", ValueType::value},
   {"data3", ValueType::data3},
}};
SpreadsheetTable stData3T95 = {"Data3 Type91", 95, {
   {"floatsA", ValueType::array},
   {"floatsB", ValueType::array},
   {"unk1", ValueType::value},
   {"data3", ValueType::data3},
}};

SpreadsheetTable stData3T13 = {"Data3 Type13", 13, {
   {"floatsA", ValueType::array},
   {"floatsB", ValueType::array},
}};
SpreadsheetTable stData3T14 = {"Data3 Type14", 14, {
   {"floatsA", ValueType::array},
   {"floatsB", ValueType::array},
}};

SpreadsheetTable stData3T19 = {"Data3 Type19", 19, {
   {"floatsA", ValueType::array},
   {"floatsB", ValueType::array},
}};
SpreadsheetTable stData3T20 = {"Data3 Type20", 20, {
   {"floatsA", ValueType::array},
   {"floatsB", ValueType::array},
}};
SpreadsheetTable stData3T27 = {"Data3 Type27", 27, {
   {"floatsA", ValueType::array},
   {"floatsB", ValueType::array},
}};
SpreadsheetTable stData3T28 = {"Data3 Type28", 28, {
   {"floatsA", ValueType::array},
   {"floatsB", ValueType::array},
}};

SpreadsheetTable stData3T21 = {"Data3 Type21", 21, {
   {"floatsA", ValueType::array},
   {"floatsB", ValueType::array},
}};
SpreadsheetTable stData3T22 = {"Data3 Type22", 22, {
   {"floatsA", ValueType::array},
   {"floatsB", ValueType::array},
}};
SpreadsheetTable stData3T29 = {"Data3 Type29", 29, {
   {"floatsA", ValueType::array},
   {"floatsB", ValueType::array},
}};

SpreadsheetTable stData3T37 = {"Data3 Type37", 37, {
   {"templateFfxId", ValueType::value},
}};
SpreadsheetTable stData3T38 = {"Data3 Type38", 38, {
   {"t38Subtype", ValueType::value},
}};

SpreadsheetTable stData3T44 = {"Data3 Type44", 44, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};
SpreadsheetTable stData3T45 = {"Data3 Type45", 45, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};
SpreadsheetTable stData3T46 = {"Data3 Type46", 46, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};
SpreadsheetTable stData3T47 = {"Data3 Type47", 47, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};
SpreadsheetTable stData3T60 = {"Data3 Type60", 60, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};
SpreadsheetTable stData3T71 = {"Data3 Type71", 71, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};
SpreadsheetTable stData3T87 = {"Data3 Type87", 87, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};
SpreadsheetTable stData3T114 = {"Data3 Type114", 114, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};
SpreadsheetTable stData3T115 = {"Data3 Type115", 115, {
   {"unk1", ValueType::value},
   {"unk2", ValueType::value},
}};

SpreadsheetTable stData3T128 = {"Data3 Type128", 128, {
   {"data3", ValueType::data3},
}};

SpreadsheetTable stData3T120 = {"Data3 Type120", 120, {
   {"data3A", ValueType::data3},
   {"data3B", ValueType::data3},
}};
SpreadsheetTable stData3T121 = {"Data3 Type121", 121, {
   {"data3A", ValueType::data3},
   {"data3B", ValueType::data3},
}};
SpreadsheetTable stData3T122 = {"Data3 Type122", 122, {
   {"data3A", ValueType::data3},
   {"data3B", ValueType::data3},
}};
SpreadsheetTable stData3T123 = {"Data3 Type123", 123, {
   {"data3A", ValueType::data3},
   {"data3B", ValueType::data3},
}};
SpreadsheetTable stData3T124 = {"Data3 Type124", 124, {
   {"data3A", ValueType::data3},
   {"data3B", ValueType::data3},
}};
SpreadsheetTable stData3T126 = {"Data3 Type126", 126, {
   {"data3A", ValueType::data3},
   {"data3B", ValueType::data3},
}};
SpreadsheetTable stData3T127 = {"Data3 Type127", 127, {
   {"data3A", ValueType::data3},
   {"data3B", ValueType::data3},
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
      {"astCountMaybe", ValueType::value},
   }
};
SpreadsheetTable stPond3T1 = {
   "Pond3 Type1", 1, {
      {"unk1", ValueType::value},
      {"unk2", ValueType::value},
      {"unk3", ValueType::value},
      {"offsetX", ValueType::value},
      {"offsetY", ValueType::value},
      {"unk6", ValueType::value},
      {"unk7", ValueType::value},
      {"unk8", ValueType::value},
      {"unk9", ValueType::value},
      {"unk10", ValueType::value},
      {"unk11", ValueType::value},
   }
};
SpreadsheetTable stPond3T2 = {
   "Pond3 Type2", 2, {
      {"lifetime", ValueType::value},
      {"unk2", ValueType::value},
      {"unk3", ValueType::value},
      {"unk4", ValueType::value},
      {"unk5", ValueType::value},
      {"unk6", ValueType::value},
   }
};
SpreadsheetTable stPond3T3 = {
   "Pond3 Type3", 3, {
      {"unk1", ValueType::value},
      {"unk2", ValueType::value},
      {"unk3", ValueType::value},
      {"unk4", ValueType::value},
   }
};
SpreadsheetTable stPond3T4 = {
   "Pond3 Type4", 4, {
      {"unk1", ValueType::value},
   }
};
SpreadsheetTable stPond3T5 = {
   "Pond3 Type5", 5, {
      {"unk1", ValueType::value},
   }
};
SpreadsheetTable stPond3T6 = {
   "Pond3 Type6", 6, {
      {"unk1", ValueType::value},
      {"unk2", ValueType::value},
   }
};
SpreadsheetTable stPond3T7 = {
   "Pond3 Type7", 7, {
      {"unk1", ValueType::value},
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


SpreadsheetTable stPond2T27 = {
   "Pond2 Type27", 27, {
      {"unk1", ValueType::value},
      {"unk2", ValueType::value},
      {"unk3", ValueType::value},
      {"unk4", ValueType::value},
      {"texId", ValueType::value},
      {"unk6", ValueType::value},
      {"unk7", ValueType::pond2Subtype},
      {"unk8", ValueType::pond2Subtype},
      {"unk9", ValueType::pond2Subtype},
      {"unk10", ValueType::pond2Subtype},
      {"unk11", ValueType::pond2Subtype},
      {"unk12", ValueType::pond2Subtype},
      {"unk13", ValueType::pond2Subtype},
      {"unk14", ValueType::pond2Subtype},
      {"unk15", ValueType::pond2Subtype},
      {"unk16", ValueType::pond2Subtype},
      {"unk17", ValueType::value},
      {"unk18", ValueType::value},
      {"unk19", ValueType::value},
      {"unk20", ValueType::value},
   }
};
SpreadsheetTable stPond2T28 = {
   "Pond2 Type28", 28, {
      {"transverseSpeed", ValueType::pond2Subtype},
      {"unk2", ValueType::pond2Subtype},
      {"forwardSpeed", ValueType::pond2Subtype},
      {"unk4", ValueType::value},
   }
};
SpreadsheetTable stPond2T29 = {
   "Pond2 Type29", 29, {
      {"unk1", ValueType::pond2Subtype},
      {"unk2", ValueType::pond2Subtype},
      {"unk3", ValueType::pond2Subtype},
      {"unk4", ValueType::pond2Subtype},
      {"unk5", ValueType::pond2Subtype},
      {"unk6", ValueType::value},
   }
};
SpreadsheetTable stPond2T30 = {
   "Pond2 Type30", 30, {
      {"unk1", ValueType::pond2Subtype},
      {"unk2", ValueType::pond2Subtype},
      {"unk3", ValueType::pond2Subtype},
      {"unk4", ValueType::pond2Subtype},
      {"unk5", ValueType::value},
      {"unk6", ValueType::value},
      {"unk7", ValueType::value},
   }
};
SpreadsheetTable stPond2T31 = {
   "Pond2 Type31", 31, {
      {"unk1", ValueType::pond2Subtype},
      {"unk2", ValueType::pond2Subtype},
      {"unk3", ValueType::pond2Subtype},
      {"unk4", ValueType::pond2Subtype},
      {"unk5", ValueType::value},
      {"unk6", ValueType::value},
   }
};
SpreadsheetTable stPond2T32 = {
   "Pond2 Type32", 32, {
      {"duplicateOffsetX", ValueType::pond2Subtype},
      {"duplicateOffsetY", ValueType::pond2Subtype},
      {"duplicateOffsetZ", ValueType::pond2Subtype},
      {"unk4", ValueType::pond2Subtype},
      {"unk5", ValueType::pond2Subtype},
      {"weirdPerpendicularDuplicateAmount", ValueType::pond2Subtype},
      {"unk7", ValueType::value},
      {"unk8", ValueType::value},
   }
};
SpreadsheetTable stPond2T40 = {
   "Pond2 Type40", 40, {
      {"unk1", ValueType::value},
      {"texId", ValueType::value},
      {"unk3", ValueType::value},
      {"unk4", ValueType::value},
      {"unk5", ValueType::value},
      {"unk6", ValueType::pond2Subtype},
      {"unk7", ValueType::pond2Subtype},
      {"unk8", ValueType::pond2Subtype},
      {"unk9", ValueType::pond2Subtype},
      {"unk10", ValueType::value},
      {"unk11", ValueType::value},
      {"unk12", ValueType::value},
      {"unk13", ValueType::value},
      {"unk14", ValueType::pond2Subtype},
      {"unk15", ValueType::pond2Subtype},
      {"unk16", ValueType::pond2Subtype},
      {"unk17", ValueType::pond2Subtype},
      {"unk18", ValueType::value},
      {"unk19", ValueType::value},
      {"unk20", ValueType::pond2Subtype},
      {"unk21", ValueType::value},
      {"unk22", ValueType::value},
      {"unk23", ValueType::pond2Subtype},
      {"unk24", ValueType::pond2Subtype},
      {"unk25", ValueType::value},
   }
};
SpreadsheetTable stPond2T43 = {
   "Pond2 Type43", 43, {
      {"unk1", ValueType::value},
      {"texId", ValueType::value},
      {"unk3", ValueType::value},
      {"unk4", ValueType::value},
      {"unk5", ValueType::value},
      {"unk6", ValueType::value},
      {"unk7", ValueType::value},
      {"unk8", ValueType::pond2Subtype},
      {"unk9", ValueType::pond2Subtype},
      {"unk10", ValueType::pond2Subtype},
      {"unk11", ValueType::pond2Subtype},
      {"unk12", ValueType::pond2Subtype},
      {"unk13", ValueType::pond2Subtype},
      {"unk14", ValueType::pond2Subtype},
      {"unk15", ValueType::pond2Subtype},
      {"unk16", ValueType::pond2Subtype},
      {"unk17", ValueType::pond2Subtype},
      {"unk18", ValueType::pond2Subtype},
      {"unk19", ValueType::pond2Subtype},
      {"unk20", ValueType::pond2Subtype},
      {"unk21", ValueType::pond2Subtype},
   }
};
SpreadsheetTable stPond2T55 = {
   "Pond2 Type55", 55, {
      {"unk1", ValueType::pond2Subtype},
      {"unk2", ValueType::pond2Subtype},
      {"unk3", ValueType::pond2Subtype},
      {"unk4", ValueType::value},
   }
};
SpreadsheetTable stPond2T59 = {
   "Pond2 Type59", 59, {
      {"unk1", ValueType::value},
      {"texId", ValueType::value},
      {"unk3", ValueType::value},
      {"unk4", ValueType::value},
      {"unk4", ValueType::pond2Subtype},
      {"unk5", ValueType::pond2Subtype},
      {"unk6", ValueType::pond2Subtype},
      {"unk7", ValueType::pond2Subtype},
      {"unk8", ValueType::pond2Subtype},
      {"unk9", ValueType::pond2Subtype},
      {"unk10", ValueType::value},
      {"unk11", ValueType::value},
      {"unk12", ValueType::pond2Subtype},
      {"unk13", ValueType::pond2Subtype},
      {"unk14", ValueType::pond2Subtype},
      {"unk15", ValueType::pond2Subtype},
      {"unk16", ValueType::pond2Subtype},
      {"unk17", ValueType::pond2Subtype},
      {"unk18", ValueType::pond2Subtype},
      {"unk19", ValueType::pond2Subtype},
      {"unk20", ValueType::value},
      {"unk21", ValueType::value},
      {"unk22", ValueType::value},
      {"unk23", ValueType::value},
   }
};
SpreadsheetTable stPond2T61 = {
   "Pond2 Type61", 61, {
      {"modelId", ValueType::value},
      {"unk2", ValueType::value},
      {"unk3", ValueType::value},
      {"unk4", ValueType::value},
      {"unk5", ValueType::pond2Subtype},
      {"unk6", ValueType::pond2Subtype},
      {"unk7", ValueType::pond2Subtype},
      {"unk8", ValueType::value},
      {"unk9", ValueType::value},
      {"unk10", ValueType::pond2Subtype},
      {"unk11", ValueType::value},
      {"unk12", ValueType::value},
      {"unk13", ValueType::pond2Subtype},
      {"unk14", ValueType::pond2Subtype},
      {"unk15", ValueType::pond2Subtype},
      {"unk16", ValueType::pond2Subtype},
      {"unk17", ValueType::pond2Subtype},
      {"unk18", ValueType::pond2Subtype},
      {"unk19", ValueType::pond2Subtype},
      {"unk20", ValueType::pond2Subtype},
      {"unk21", ValueType::pond2Subtype},
      {"unk22", ValueType::pond2Subtype},
      {"unk23", ValueType::value},
      {"unk24", ValueType::value},
   }
};
SpreadsheetTable stPond2T66 = {
   "Pond2 Type66", 66, {
      {"unk1", ValueType::value},
      {"unk2", ValueType::value},
      {"unk3", ValueType::value},
      {"unk4", ValueType::value},
      {"unk5", ValueType::value},
      {"unk6", ValueType::pond2Subtype},
      {"unk7", ValueType::pond2Subtype},
      {"unk8", ValueType::pond2Subtype},
      {"unk9", ValueType::pond2Subtype},
      {"unk10", ValueType::pond2Subtype},
      {"unk11", ValueType::pond2Subtype},
      {"unk12", ValueType::pond2Subtype},
      {"unk13", ValueType::pond2Subtype},
      {"unk14", ValueType::pond2Subtype},
      {"unk15", ValueType::pond2Subtype},
      {"unk16", ValueType::pond2Subtype},
      {"unk17", ValueType::pond2Subtype},
      {"unk18", ValueType::pond2Subtype},
      {"unk19", ValueType::pond2Subtype},
      {"unk20", ValueType::pond2Subtype},
      {"unk21", ValueType::pond2Subtype},
      {"unk22", ValueType::pond2Subtype},
      {"unk23", ValueType::pond2Subtype},
      {"unk24", ValueType::pond2Subtype},
      {"unk25", ValueType::pond2Subtype},
      {"unk26", ValueType::pond2Subtype},
      {"unk27", ValueType::pond2Subtype},
      {"unk28", ValueType::pond2Subtype},
      {"unk29", ValueType::pond2Subtype},
      {"unk30", ValueType::pond2Subtype},
      {"unk31", ValueType::pond2Subtype},
   }
};
SpreadsheetTable stPond2T70 = {
   "Pond2 Type70", 70, {
      {"unk1", ValueType::value},
      {"unk2", ValueType::value},
      {"unk3", ValueType::value},
      {"unk4", ValueType::value},
      {"unk5", ValueType::value},
      {"texId1", ValueType::value},
      {"texId2", ValueType::value},
      {"texId3", ValueType::value},
      {"unk9", ValueType::value},
      {"unk10", ValueType::value},
      {"unk11", ValueType::value},
      {"unk12", ValueType::pond2Subtype},
      {"unk13", ValueType::pond2Subtype},
      {"unk14", ValueType::pond2Subtype},
      {"unk15", ValueType::pond2Subtype},
      {"unk16", ValueType::pond2Subtype},
      {"unk17", ValueType::pond2Subtype},
      {"unk18", ValueType::pond2Subtype},
      {"unk19", ValueType::pond2Subtype},
      {"unk20", ValueType::pond2Subtype},
      {"unk21", ValueType::pond2Subtype},
      {"unk22", ValueType::pond2Subtype},
      {"unk23", ValueType::pond2Subtype},
      {"unk24", ValueType::pond2Subtype},
      {"unk25", ValueType::pond2Subtype},
      {"unk26", ValueType::pond2Subtype},
      {"unk27", ValueType::pond2Subtype},
      {"unk28", ValueType::pond2Subtype},
      {"unk29", ValueType::pond2Subtype},
      {"unk30", ValueType::pond2Subtype},
      {"unk31", ValueType::pond2Subtype},
      {"unk32", ValueType::pond2Subtype},
      {"unk33", ValueType::pond2Subtype},
      {"unk34", ValueType::pond2Subtype},
      {"unk35", ValueType::pond2Subtype},
      {"unk36", ValueType::pond2Subtype},
      {"unk37", ValueType::pond2Subtype},
      {"unk38", ValueType::pond2Subtype},
      {"unk39", ValueType::pond2Subtype},
      {"unk40", ValueType::pond2Subtype},
      {"unk41", ValueType::pond2Subtype},
      {"unk42", ValueType::value},
      {"unk43", ValueType::value},
      {"unk44", ValueType::value},
      {"unk45", ValueType::value},
      {"unk46", ValueType::value},
      {"unk47", ValueType::value},
      {"unk48", ValueType::value},
      {"unk49", ValueType::value},
   }
};
SpreadsheetTable stPond2T71 = {
   "Pond2 Type71", 71, {
      {"unk1", ValueType::value},
      {"unk2", ValueType::value},
      {"unk3", ValueType::value},
      {"unk4", ValueType::value},
      {"unk5", ValueType::value},
      {"texId", ValueType::value},
      {"unk7", ValueType::value},
      {"unk8", ValueType::value},
      {"unk9", ValueType::value},
      {"unk10", ValueType::value},
      {"unk11", ValueType::pond2Subtype},
      {"unk12", ValueType::pond2Subtype},
      {"unk13", ValueType::pond2Subtype},
      {"unk14", ValueType::pond2Subtype},
      {"unk15", ValueType::pond2Subtype},
      {"unk16", ValueType::pond2Subtype},
      {"unk17", ValueType::pond2Subtype},
      {"unk18", ValueType::pond2Subtype},
      {"unk19", ValueType::pond2Subtype},
      {"unk20", ValueType::pond2Subtype},
      {"unk21", ValueType::value},
      {"unk22", ValueType::value},
      {"unk23", ValueType::pond2Subtype},
      {"unk24", ValueType::pond2Subtype},
      {"unk25", ValueType::pond2Subtype},
      {"unk26", ValueType::pond2Subtype},
      {"unk27", ValueType::pond2Subtype},
      {"unk28", ValueType::pond2Subtype},
      {"unk29", ValueType::pond2Subtype},
      {"unk30", ValueType::pond2Subtype},
      {"unk31", ValueType::pond2Subtype},
      {"unk32", ValueType::pond2Subtype},
      {"unk33", ValueType::value},
      {"unk34", ValueType::value},
      {"unk35", ValueType::value},
      {"unk36", ValueType::value},
      {"unk37", ValueType::value},
      {"unk38", ValueType::value},
      {"unk39", ValueType::value},
      {"unk40", ValueType::value},
   }
};
SpreadsheetTable stPond2T84 = {
   "Pond2 Type84", 84, {
      {"unk1", ValueType::pond2Subtype},
      {"unk2", ValueType::pond2Subtype},
      {"unk3", ValueType::pond2Subtype},
      {"unk4", ValueType::value},
      {"unk5", ValueType::pond2Subtype},
      {"unk6", ValueType::value},
   }
};
SpreadsheetTable stPond2T105 = {
   "Pond2 Type105", 105, {
      {"unk1", ValueType::pond2Subtype},
      {"unk2", ValueType::pond2Subtype},
      {"unk3", ValueType::pond2Subtype},
      {"unk4", ValueType::value},
      {"unk5", ValueType::pond2Subtype},
      {"unk6", ValueType::value},
      {"unk7", ValueType::pond2Subtype},
   }
};
SpreadsheetTable stPond2T107 = {
   "Pond2 Type107", 107, {
      {"unk1", ValueType::value},
      {"texId", ValueType::value},
      {"unk3", ValueType::value},
      {"unk4", ValueType::pond2Subtype},
      {"unk5", ValueType::pond2Subtype},
      {"unk6", ValueType::pond2Subtype},
      {"unk7", ValueType::pond2Subtype},
      {"unk8", ValueType::pond2Subtype},
      {"unk9", ValueType::pond2Subtype},
      {"unk10", ValueType::pond2Subtype},
   }
};
SpreadsheetTable stPond2T108 = {
   "Pond2 Type108", 108, {
      {"unk1", ValueType::value},
      {"curvesTimeMaybe", ValueType::value},
      {"unk3", ValueType::value},
      {"unk4", ValueType::value},
      {"unk5", ValueType::value},
      {"modelId", ValueType::value},
      {"unk7", ValueType::value},
      {"unk8", ValueType::value},
      {"unk9", ValueType::value},
      {"scaleData1X", ValueType::pond2Subtype},
      {"scaleData1Y", ValueType::pond2Subtype},
      {"scaleData1Z", ValueType::pond2Subtype},
      {"scaleData2X", ValueType::pond2Subtype},
      {"scaleData2Y", ValueType::pond2Subtype},
      {"scaleData2Z", ValueType::pond2Subtype},
      {"pitchSpeed", ValueType::pond2Subtype},
      {"yawSpeed", ValueType::pond2Subtype},
      {"rollSpeed", ValueType::pond2Subtype},
      {"rotData1Maybe", ValueType::pond2Subtype},
      {"rotData2Maybe", ValueType::pond2Subtype},
      {"rotData3Maybe", ValueType::pond2Subtype},
      {"unk22", ValueType::value},
      {"unk23", ValueType::value},
      {"unk24", ValueType::pond2Subtype},
      {"unk25", ValueType::pond2Subtype},
      {"unk26", ValueType::pond2Subtype},
      {"unk27", ValueType::pond2Subtype},
      {"unk28", ValueType::pond2Subtype},
      {"unk29", ValueType::pond2Subtype},
      {"colorData1R", ValueType::pond2Subtype},
      {"colorData1G", ValueType::pond2Subtype},
      {"colorData1B", ValueType::pond2Subtype},
      {"colorData1A", ValueType::pond2Subtype},
      {"colorData2R", ValueType::pond2Subtype},
      {"colorData2G", ValueType::pond2Subtype},
      {"colorData2B", ValueType::pond2Subtype},
      {"colorData2A", ValueType::pond2Subtype},
      {"unk38", ValueType::value},
      {"unk39", ValueType::value},
      {"unk40", ValueType::value},
      {"unk41", ValueType::value},
      {"unk42", ValueType::value},
   }
};

std::vector<SpreadsheetTable*> pond2Tables = {
   &stPond2T27,
   &stPond2T28,
   &stPond2T29,
   &stPond2T30,
   &stPond2T31,
   &stPond2T32,
   &stPond2T40,
   &stPond2T43,
   &stPond2T55,
   &stPond2T59,
   &stPond2T61,
   &stPond2T66,
   &stPond2T70,
   &stPond2T71,
   &stPond2T84,
   &stPond2T105,
   &stPond2T107,
   &stPond2T108,
};


struct SubtypeTable{
   int pond2Type = 0;
   std::string varName;
   int subtype = 0;

   SpreadsheetTable* st;
};

std::vector<SubtypeTable> subtypeTables;

void makeSubtypeSpreadsheetTable(SubtypeTable& subtypeTable){
   char buffer[256];
   snprintf(
      buffer,
      sizeof(buffer),
      "%s when subtype is %i",
      subtypeTable.varName.c_str(),
      subtypeTable.subtype
   );

   int subtype = subtypeTable.subtype;
   bool isCurve = (
      subtype == 0 || subtype == 4 || subtype == 5 || subtype == 6 || subtype == 7 ||
      subtype == 8 || subtype == 9 || subtype == 12 || subtype == 16 || subtype == 17 ||
      subtype == 20
   );
   if(isCurve){
      bool hasRange = subtype == 5 || subtype == 7 || subtype == 9 || subtype == 17;
      bool hasPreDataIndex = subtype == 6 || subtype == 7;
      int numArrays = subtype == 8 || subtype == 9 || subtype == 20 ? 4 : 2;

      subtypeTable.st = new SpreadsheetTable(buffer, {{"times", ValueType::array}});

      if(numArrays == 2){
         subtypeTable.st->rows.emplace_back("values", ValueType::array);
      }else{
         subtypeTable.st->rows.emplace_back("values1", ValueType::array);
         subtypeTable.st->rows.emplace_back("values2", ValueType::array);
         subtypeTable.st->rows.emplace_back("values3", ValueType::array);
      }

      if(hasRange){
         subtypeTable.st->rows.emplace_back("range", ValueType::array);
      }

      if(hasPreDataIndex){
         subtypeTable.st->rows.emplace_back("preDataIndex", ValueType::value);
      }
   }else if(subtype == 24){
      subtypeTable.st = new SpreadsheetTable(buffer, {
         {"constant", ValueType::value},
      });
   }else if(subtype == 25){
      subtypeTable.st = new SpreadsheetTable(buffer, {
         {"base", ValueType::value},
         {"unknownA", ValueType::value},
         {"unknownB", ValueType::value},
      });
   }else if(subtype == 26){
      subtypeTable.st = new SpreadsheetTable(buffer, {
         {"unk1", ValueType::value},
         {"unk2", ValueType::value},
      });
   }else if(subtype == 27){
      subtypeTable.st = new SpreadsheetTable(buffer, {
         {"unk1", ValueType::value},
         {"unk2", ValueType::value},
         {"unk3", ValueType::value},
         {"preDataIndex", ValueType::value},
      });
   }else if(subtype == 28){
      subtypeTable.st = new SpreadsheetTable(buffer, {});
   }
}


void outputTable(std::string& csvText, SpreadsheetTable& table){
   char buffer[512];

   csvText += table.name;
   snprintf(buffer, sizeof(buffer), ",Count: %i\n", table.count);
   csvText += buffer;

   if(table.rows.empty()){
      csvText += "(no vars)\n";

      return;
   }
   if(table.count == 0){
      csvText += "\n";

      return;
   }

   //csvText += '\n';
   csvText += "var,min,max,common1,common2,common3,common4,common5,common6";

   // Analyze data and add relevant array columns
   bool usesArrays = false;
   size_t maxArrayLength = 0;
   for(SpreadsheetRow& row : table.rows){
      if(row.type == ValueType::value){
         row.valueAnalyzer.analyze();
      }else if(row.type == ValueType::array){
         row.arrayAnalyzer.analyze();

         usesArrays = true;
         if(row.arrayAnalyzer.longest.size() > maxArrayLength){
            maxArrayLength = row.arrayAnalyzer.longest.size();
         }
      }else if(row.type == ValueType::data3){
         row.name += " type";
         row.valueAnalyzer.analyze();
      }else if(row.type == ValueType::pond2Subtype){
         row.name += " type";
         row.valueAnalyzer.analyze();
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

   // Output
   for(SpreadsheetRow& row : table.rows){
      if(row.type == ValueType::value || row.type == ValueType::data3 || row.type == ValueType::pond2Subtype){
         snprintf(
            buffer,
            sizeof(buffer),
            "%s,%g,%g",
            row.name.c_str(),
            row.valueAnalyzer.min,
            row.valueAnalyzer.max
         );
         csvText += buffer;

         std::vector<std::pair<float, int>>& mostCommon = row.valueAnalyzer.mostCommon;
         for(size_t n = 0; n < 6; ++n){
            if(n < mostCommon.size()){
               //snprintf(buffer, sizeof(buffer), ",%g", mostCommon[n].first);
               snprintf(buffer, sizeof(buffer), ",%g (x%i)", mostCommon[n].first, mostCommon[n].second);
               csvText += buffer;
            }else{
               csvText += ",";
            }
         }
      }else if(row.type == ValueType::array){
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

         std::vector<std::pair<float, int>>& mostCommon = row.arrayAnalyzer.arraySize.mostCommon;
         for(size_t n = 0; n < 6; ++n){
            if(n < mostCommon.size()){
               //snprintf(buffer, sizeof(buffer), ",%g", mostCommon[n].first);
               snprintf(buffer, sizeof(buffer), ",%g (x%i)", mostCommon[n].first, mostCommon[n].second);
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
               if(row.type == ValueType::value){
                  json::JSON& jsonVar = obj[row.name];

                  if(jsonVar.JSONType() == json::JSON::Class::Integral){
                     row.valueAnalyzer.feed((float)jsonVar.ToInt());
                  }else{
                     row.valueAnalyzer.feed(jsonVar.ToFloat());
                  }
               }else if(row.type == ValueType::array){
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
               }else if(row.type == ValueType::data3){
                  row.valueAnalyzer.feed((float)obj[row.name]["data3Type"].ToInt());
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
               if(row.type == ValueType::value){
                  if(jsonVar.JSONType() == json::JSON::Class::Integral){
                     row.valueAnalyzer.feed((float)jsonVar.ToInt());
                  }else{
                     row.valueAnalyzer.feed(jsonVar.ToFloat());
                  }
               }else if(row.type == ValueType::array){
                  throw;
               }
            }

            break;
         }
      }
      if(found == false){
         throw;
      }
   };

   testFunctions.onPond2 = [&](json::JSON& obj, TestFunctions::Context context){
      int pond2Type = obj["pond2Type"].ToInt();

      bool found = false;
      for(SpreadsheetTable* table : pond2Tables){
         if(table->typeId == pond2Type){
            found = true;

            ++table->count;

            for(SpreadsheetRow& row : table->rows){
               json::JSON& jsonVar = obj[row.name];

               if(row.type == ValueType::value){
                  if(jsonVar.JSONType() == json::JSON::Class::Integral){
                     row.valueAnalyzer.feed((float)jsonVar.ToInt());
                  }else{
                     row.valueAnalyzer.feed(jsonVar.ToFloat());
                  }
               }else if(row.type == ValueType::array){
                  throw;
               }else if(row.type == ValueType::pond2Subtype){
                  int subtypeType = obj[row.name]["subtypeType"].ToInt();

                  row.valueAnalyzer.feed((float)subtypeType);
               }
            }

            break;
         }
      }
      if(found == false){
         //throw;
      }
   };

   testFunctions.onPond2Subtype = [&](json::JSON& obj, TestFunctions::Context context){
      json::JSON& pond2 = context.parent;
      int pond2Type = pond2["pond2Type"].ToInt();
      int subtypeType = obj["subtypeType"].ToInt();

      int subtypeTableIndex;
      {
         auto pred = [&](SubtypeTable& table) -> bool{
            return (
               table.pond2Type == pond2Type &&
               table.varName == context.varName &&
               table.subtype == subtypeType
            );
         };
         subtypeTableIndex = findIndex(subtypeTables, pred);
         if(subtypeTableIndex == -1){
            SubtypeTable st;
            st.pond2Type = pond2Type;
            st.varName = context.varName;
            st.subtype = subtypeType;
            makeSubtypeSpreadsheetTable(st);

            // Try to put it after one with the same varName
            for(size_t n = 0; n < subtypeTables.size(); ++n){
               if(subtypeTables[n].pond2Type == pond2Type && subtypeTables[n].varName == context.varName){
                  subtypeTables.insert(subtypeTables.begin() + n + 1, st);
                  subtypeTableIndex = n + 1;
                  break;
               }
            }

            if(subtypeTableIndex == -1){
               subtypeTables.push_back(st);
               subtypeTableIndex = subtypeTables.size() - 1;
            }
         }
      }

      SpreadsheetTable* table = subtypeTables[subtypeTableIndex].st;
      ++table->count;

      std::vector<float> arrayBuffer;
      for(SpreadsheetRow& row : table->rows){
         json::JSON& jsonVar = obj[row.name];

         if(row.type == ValueType::value){
            if(jsonVar.JSONType() == json::JSON::Class::Integral){
               row.valueAnalyzer.feed((float)jsonVar.ToInt());
            }else{
               row.valueAnalyzer.feed(jsonVar.ToFloat());
            }
         }else if(row.type == ValueType::array){
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

   // Pond2
   {
      // subtypeTables are already grouped by var name, further sort by subtype
      {
         int startSort = 0;
         std::string varName = subtypeTables.front().varName;
         for(size_t n = 1; n < subtypeTables.size(); ++n){
            if(subtypeTables[n].varName != varName || n == subtypeTables.size() - 1){
               int endSort = n;
               auto pred = [&](SubtypeTable& a, SubtypeTable& b) -> bool{
                  return a.subtype < b.subtype;
               };
               std::sort(
                  subtypeTables.begin() + startSort,
                  subtypeTables.begin() + endSort,
                  pred
               );

               startSort = n + 1;
            }
         }
      }

      std::string csvText;
      for(auto table : pond2Tables){
         csvText.resize(0);
         csvText += "\n\n\n\n\n";

         // Main Pond2 data
         outputTable(csvText, *table);

         // Subtypes
         std::string varName;
         for(SubtypeTable& subtypeTable : subtypeTables){
            if(subtypeTable.pond2Type == table->typeId){
               if(subtypeTable.varName == varName){
                  csvText += "\n";
               }else{
                  csvText += "\n\n";
                  varName = subtypeTable.varName;
               }

               outputTable(csvText, *subtypeTable.st);
            }
         }

         wchar_t wBuffer[256];
         swprintf(wBuffer, sizeof(wBuffer), L"spreadsheet/pond2T%d.csv", table->typeId);
         FILE* file = _wfopen(wBuffer, L"w");
         if(file == NULL){
            throw;
         }
         fwrite(csvText.data(), 1, csvText.size(), file);
         fclose(file);
      }
   }
}
