#include "header.hpp"


struct DataReader{
   std::vector<byte> bytes;
   std::vector<bool> bytesRead;

   int readInt(int addr){
      std::vector<bool>& br = this->bytesRead;
      br[addr+3] = br[addr+2] = br[addr+1] = br[addr+0] = true;
      return *reinterpret_cast<int*>(&this->bytes[addr]);
   }
   
   float readFloat(int addr){
      std::vector<bool>& br = this->bytesRead;
      br[addr+3] = br[addr+2] = br[addr+1] = br[addr+0] = true;
      return *reinterpret_cast<float*>(&this->bytes[addr]);
   }

   byte readByte(int addr){
      std::vector<bool>& br = this->bytesRead;
      br[addr] = true;
      return *reinterpret_cast<byte*>(&this->bytes[addr]);
   }
};

struct FlexibleData{
   int type = -1;

   template<typename T>
   T* get(){
      return reinterpret_cast<T*>(this->data.data());
   }

   template<typename T>
   void set(T& t){
      this->data.resize(sizeof(T));
      *reinterpret_cast<T*>(this->data.data()) = t;
   }

private:
   std::vector<int> data;
};

struct Pond3Type0{
   int unk1;
};

struct Pond3Type1{
   int unk1;
   int unk2;
   int unk3;
   float unk4; // often -0.0
   float unk5;
   int unk6;
   int unk7;
   int unk8;
   int unk9;
   int unk10;
   float unk11; // often 1.0
};

struct Pond3Type2{
   float unk1;
   float unk2;
   int unk3;
   float unk4;
   int unk5;
};

struct Pond3Type3{
   float unk1;
   int unk2;
   float unk3;
   int unk4;
};

struct Pond3Type4{
   int unk1;
};

struct Pond3Type5{
   int unk1;
};

struct Pond3Type6{
   float unk1;
   int unk2;
};

struct Pond3Type7{
   int unk1;
};

struct Pond1{
   std::vector<FlexibleData> data3s;
};

// Pond2 in header

struct Pond3{
   FlexibleData data;
};

struct AST{
   Pond1* pond1 = nullptr;
   int axisNumber = -1;
   Pond2* pond2 = nullptr;
   Pond3* pond3 = nullptr;
};

struct Type1{
   int unk1;
};

struct Type2{
   std::vector<int> bunchaInts;
};

struct Type3{
   std::vector<float> floatThings;
   std::vector<int> intThings;
};

// 4 and 5 are Pond2 subtypes (but 5 is below...)

struct Type5{
   std::vector<float> floatThings;
   std::vector<int> intThings;
};

struct Type6{
   std::vector<float> floatThings;
   std::vector<int> intThings;
};

struct Type7{
   float unk1;
};

// 8 is a pond2 subtype

struct Type9{
   std::vector<float> floatThings;
   std::vector<int> intThings;
};

struct Type11{
   std::vector<float> floatThings;
   std::vector<int> intThings;
};

struct Type12{
   std::vector<float> floatsA;
   std::vector<float> floatsB;
};

struct Type13{
   std::vector<float> floatsA;
   std::vector<float> floatsB;
   int count;
};

struct Type14{
   std::vector<float> floatsA;
   std::vector<float> floatsB;
   int count;
};

// 16 and 17 are pond2 subtypes

struct Type19{
   std::vector<float> floatsA;
   std::vector<float> floatsB;
   int count;
};

struct Type20{
   std::vector<float> floatsA;
   std::vector<float> floatsB;
   int count;
};

// 25 and 28 are pond2 subtypes (despite the below...)

// 27 to 32 are pond2 types

struct Type37{
   int refFfxId;
   bool hasAST;
   AST ast;
   int zero;
};

struct Type38{
   int unknown;
   bool hasAST;
   AST ast;
   int zero;
};

struct Type44{
   int unk1;
};

struct Type45{
   short unk1;
   short unk2;
};

struct Type46{
   int unk1;
};

struct Type47{
   short unk1;
   short unk2;
};

struct Type59{
   int unk1;
};

struct Type60{
   int unk1;
};

struct Type66{
   int unk1;
};

struct Type68{
   int unk1;
};

struct Type70{
   float unk1;
};

struct Type71{
   int unk1;
};

struct Type79{
   int unk1;
   int unk2;
};

struct Type81{
   float unk1;
   float unk2;
};

// 84 is a pond2 type

struct Type85{
   float unk1;
   float unk2;
};

struct Type87{
   short unk1;
   short unk2;
};

struct Type89{
   std::vector<float> floatThings;
   std::vector<int> intThings;
   int unk1;
   int unk2;
};

struct Type91{
   std::vector<float> floatsA;
   std::vector<float> floatsB;
   int unk1;
   FlexibleData data3;
};

struct Type95{
   std::vector<float> floatsA;
   std::vector<float> floatsB;
   int unk1;
   FlexibleData data3;
};

struct Type111{
   int unk1;
};

// 112 has no params

// 113 has no params

struct Type114{
   short unk1;
   short unk2;
};

struct Type115{
   int unk1;
};

struct Type120{
   FlexibleData data3A;
   FlexibleData data3B;
};

struct Type121{
   FlexibleData data3A;
   FlexibleData data3B;
};

struct Type122{
   FlexibleData data3A;
   FlexibleData data3B;
};

struct Type123{
   FlexibleData data3A;
   FlexibleData data3B;
};

struct Type124{
   FlexibleData data3A;
   FlexibleData data3B;
};

struct Type126{
   FlexibleData data3A;
   FlexibleData data3B;
};

struct Type127{
   FlexibleData data3A;
   FlexibleData data3B;
};

struct Type128{
   FlexibleData data3;
};

// 129 has no params

// 130 has no params

// 131 has no params

// 132 has no params

struct House{
   int offset;

   int linksOffset;
   int blossomOffset;
   int linkCount;
   int blossomCount;

   struct Link{
      House* house;
      FlexibleData data3;
   };

   struct Blossom{
      int blossomNumber;
      AST ast;
   };

   std::vector<Link> links;
   std::vector<Blossom> blossoms;
};
struct Type133{
   int ffxId;
   int alwaysZero[7];
   int always8Or10;
   AST ast1;
   AST ast2;
   int house;
      //int crepe1;
      //   int monkey;
      //      int monkeyTail1;
      //      int monkeyTail2;
      //         int hair;
      //         AST hairAST;
      //      int monkeyTail3;
      //      int monkeyTail4;
      //   int turtle;
      //      FlexibleData turtleEgg;
      //int crepe2;
      //   int blossom;
      //   AST blossomAST;
      //int tulip1;
      //int tulip2;
   //int ostrich;

   int houseCount;

   std::vector<House*> houses;
};

struct Type134{
   int ffxId;
   int alwaysZero;
   std::vector<Type133> type133s;
};

// 136 has no params

struct Type137{
   int unk1;
   int unk2;
   int unk3;
   int unk4;
};

struct Type138{
   int unk1;
};

struct Type139{
   int unk1;
};

struct Type140{
   int unk1;
   int unk2;
};


void loadFfxFile(Ffx& ffx, std::wstring path, std::set<int>* allPond2Types, std::vector<Pond2*>* allPond2s){
   ffx = Ffx();

   DataReader dr;
   {
      FILE* file = _wfopen(path.c_str(), L"rb");
      currentGlobalFile = file;
      if(file == NULL) {
         ffxReadError(path, L"Cannot open file");
         return;
      }

      fseek(file, 0, SEEK_END);
      long fileSize = ftell(file);
      fseek(file, 0, SEEK_SET);

      dr.bytes.resize(fileSize);
      fread(dr.bytes.data(), 1, dr.bytes.size(), file);

      fclose(file);
   }

   dr.bytesRead.resize(dr.bytes.size());
   auto& br = dr.bytesRead;

   //bool hasReadPond3Yet = false;
   int lastPond1DataOffset = 0;

   std::function<AST(int)> readAST;
   std::function<FlexibleData(int)> readData3;
   auto readPadding = [&](int addr, int multiple) -> void{
      int firstAddress = addr;
      while(addr % multiple != 0){
         if(dr.bytesRead[addr]){
            ffxReadError(path, L"padding already read");
         }
         int zero = dr.readInt(addr);
         if(zero != 0){
            ffxReadError(path, L"padding not zero");
         }
         addr += 4;
      }
   };
   readData3 = [&](int addr) -> FlexibleData{
      FlexibleData data3;

      data3.type = dr.readInt(addr);

      if(data3.type == 1){
         Type1 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 2){
         Type2 t;

         int offset = dr.readInt(addr + 4);
         int count = dr.readInt(addr + 8);

         for(int n = 0; n < count; ++n){
            t.bunchaInts.push_back(dr.readInt(offset + n * 4));
         }

         data3.set(t);
      }else if(data3.type == 3){
         Type3 t;

         int offsetToFloats = dr.readInt(addr + 4);
         if(offsetToFloats == 0) ffxReadError(path, L"Type5");
         int offsetToInts = dr.readInt(addr + 8);
         if(offsetToInts == 0) ffxReadError(path, L"Type5");

         int count = dr.readInt(addr + 12);
         if(count > 1){
            int bp=42;
         }
         for(int n = 0; n < count; ++n){
            t.floatThings.push_back(dr.readFloat(offsetToFloats + n * 4));
            t.intThings.push_back(dr.readInt(offsetToInts + n * 4));
         }

         data3.set(t);
      }else if(data3.type == 5){
         Type5 t;

         int offsetToFloats = dr.readInt(addr + 4);
         if(offsetToFloats == 0) ffxReadError(path, L"Type5");
         int offsetToInts = dr.readInt(addr + 8);
         if(offsetToInts == 0) ffxReadError(path, L"Type5");

         int count = dr.readInt(addr + 12);
         for(int n = 0; n < count; ++n){
            t.floatThings.push_back(dr.readFloat(offsetToFloats + n * 4));
            t.intThings.push_back(dr.readInt(offsetToInts + n * 4));
         }

         data3.set(t);
      }else if(data3.type == 6){
         Type6 t;

         int offsetToFloats = dr.readInt(addr + 4);
         if(offsetToFloats == 0) ffxReadError(path, L"Type6");
         int offsetToInts = dr.readInt(addr + 8);
         if(offsetToInts == 0) ffxReadError(path, L"Type6");

         int count = dr.readInt(addr + 12);
         if(count != 1){
            int bp=42;
         }
         for(int n = 0; n < count; ++n){
            t.floatThings.push_back(dr.readFloat(offsetToFloats + n * 4));
            t.intThings.push_back(dr.readInt(offsetToInts + n * 4));
         }

         data3.set(t);
      }else if(data3.type == 7){
         Type7 t;
         t.unk1 = dr.readFloat(addr + 4);
         data3.set(t);
      }else if(data3.type == 9){
         Type9 t;

         int offsetToFloats = dr.readInt(addr + 4);
         if(offsetToFloats == 0) ffxReadError(path, L"Type9");
         int offsetToInts = dr.readInt(addr + 8);
         if(offsetToInts == 0) ffxReadError(path, L"Type9");

         int count = dr.readInt(addr + 12);
         if(count != 1){
            int bp=42;
         }
         for(int n = 0; n < count; ++n){
            t.floatThings.push_back(dr.readFloat(offsetToFloats + n * 4));
            t.intThings.push_back(dr.readInt(offsetToInts + n * 4));
         }

         data3.set(t);
      }else if(data3.type == 11){
         Type11 t;

         int offsetToFloats = dr.readInt(addr + 4);
         if(offsetToFloats == 0) ffxReadError(path, L"Type11");
         int offsetToInts = dr.readInt(addr + 8);
         if(offsetToInts == 0) ffxReadError(path, L"Type11");

         int count = dr.readInt(addr + 12);
         if(count != 1){
            int bp=42;
         }
         for(int n = 0; n < count; ++n){
            t.floatThings.push_back(dr.readFloat(offsetToFloats + n * 4));
            t.intThings.push_back(dr.readInt(offsetToInts + n * 4));
         }

         data3.set(t);
      }else if(data3.type == 12){
         Type12 t;

         int offsetA = dr.readInt(addr + 4);
         if(offsetA == 0) ffxReadError(path, L"Type12");
         int offsetB = dr.readInt(addr + 8);
         if(offsetB == 0) ffxReadError(path, L"Type12");

         int count = dr.readInt(addr + 12);
         for(int n = 0; n < count; ++n){
            t.floatsA.push_back(dr.readFloat(offsetA + n * 4));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 4));
         }

         data3.set(t);
      }else if(data3.type == 13){
         Type13 t;

         int offsetA = dr.readInt(addr + 4);
         if(offsetA == 0) ffxReadError(path, L"Type14");
         int offsetB = dr.readInt(addr + 8);
         if(offsetB == 0) ffxReadError(path, L"Type14");

         t.count = dr.readInt(addr + 12);
         for(int n = 0; n < t.count; ++n){
            t.floatsA.push_back(dr.readFloat(offsetA + n * 4));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 3 * 4 + 0));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 3 * 4 + 4));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 3 * 4 + 8));
         }

         data3.set(t);
      }else if(data3.type == 14){
         Type14 t;

         int offsetA = dr.readInt(addr + 4);
         if(offsetA == 0) ffxReadError(path, L"Type14");
         int offsetB = dr.readInt(addr + 8);
         if(offsetB == 0) ffxReadError(path, L"Type14");

         t.count = dr.readInt(addr + 12);
         for(int n = 0; n < t.count; ++n){
            t.floatsA.push_back(dr.readFloat(offsetA + n * 4));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 3 * 4 + 0));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 3 * 4 + 4));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 3 * 4 + 8));
         }

         data3.set(t);
      }else if(data3.type == 19){
         Type19 t;

         int offsetA = dr.readInt(addr + 4);
         if(offsetA == 0) ffxReadError(path, L"Type19");
         int offsetB = dr.readInt(addr + 8);
         if(offsetB == 0) ffxReadError(path, L"Type19");

         t.count = dr.readInt(addr + 12);
         for(int n = 0; n < t.count; ++n){
            t.floatsA.push_back(dr.readFloat(offsetA + n * 4));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 4 * 4 + 0));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 4 * 4 + 4));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 4 * 4 + 8));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 4 * 4 + 12));
         }

         // Also weird data type, want to examine more
         //if(ffx.id > 130){
         //   int bp=42;
         //}

         data3.set(t);
      }else if(data3.type == 20){
         Type20 t;

         int offsetA = dr.readInt(addr + 4);
         if(offsetA == 0) ffxReadError(path, L"Type20");
         int offsetB = dr.readInt(addr + 8);
         if(offsetB == 0) ffxReadError(path, L"Type20");

         t.count = dr.readInt(addr + 12);
         for(int n = 0; n < t.count; ++n){
            t.floatsA.push_back(dr.readFloat(offsetA + n * 4));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 4 * 4 + 0));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 4 * 4 + 4));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 4 * 4 + 8));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 4 * 4 + 12));
         }

         // Weird data type, want to examine more
         //if(ffx.id > 130){
         //   int bp=42;
         //}

         data3.set(t);
      }else if(data3.type == 37){
         Type37 t;

         t.refFfxId = dr.readInt(addr + 4);
         int astOffset = dr.readInt(addr + 8);
         t.hasAST = astOffset != 0;
         if(t.hasAST){
            t.ast = readAST(astOffset);
         }
         t.zero = dr.readInt(addr + 12);

         data3.set(t);
      }else if(data3.type == 38){
         Type38 t;

         t.unknown = dr.readInt(addr + 4);
         int astOffset = dr.readInt(addr + 8);
         t.hasAST = astOffset != 0;
         if(t.hasAST){
            t.ast = readAST(astOffset);
         }
         t.zero = dr.readInt(addr + 12);

         data3.set(t);
      }else if(data3.type == 44){
         Type44 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 45){
         Type45 t;
         int all = dr.readInt(addr + 4);
         short* sp = reinterpret_cast<short*>(&all);
         t.unk1 = sp[0];
         t.unk2 = sp[1];
         data3.set(t);
      }else if(data3.type == 46){
         Type46 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 47){
         Type47 t;
         int all = dr.readInt(addr + 4);
         short* sp = reinterpret_cast<short*>(&all);
         t.unk1 = sp[0];
         t.unk2 = sp[1];
         data3.set(t);
      }else if(data3.type == 59){
         Type59 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 60){
         Type60 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 66){
         Type66 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 68){
         Type68 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 70){
         Type70 t;
         t.unk1 = dr.readFloat(addr + 4);
         data3.set(t);
      }else if(data3.type == 71){
         Type71 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 79){
         Type79 t;
         t.unk1 = dr.readInt(addr + 4);
         t.unk2 = dr.readInt(addr + 8);
         data3.set(t);
      }else if(data3.type == 81){
         Type81 t;
         t.unk1 = dr.readFloat(addr + 4);
         t.unk2 = dr.readFloat(addr + 8);
         data3.set(t);
      }else if(data3.type == 85){
         Type81 t;
         t.unk1 = dr.readFloat(addr + 4);
         t.unk2 = dr.readFloat(addr + 8);
         data3.set(t);
      }else if(data3.type == 87){
         Type87 t;
         int all = dr.readInt(addr + 4);
         short* sp = reinterpret_cast<short*>(&all);
         t.unk1 = sp[0];
         t.unk2 = sp[1];
         data3.set(t);
      }else if(data3.type == 89){
         Type89 t;

         int offsetToFloats = dr.readInt(addr + 4);
         if(offsetToFloats == 0) ffxReadError(path, L"Type11");
         int offsetToInts = dr.readInt(addr + 8);
         if(offsetToInts == 0) ffxReadError(path, L"Type11");

         int count = dr.readInt(addr + 12);
         for(int n = 0; n < count; ++n){
            t.floatThings.push_back(dr.readFloat(offsetToFloats + n * 4));
            t.intThings.push_back(dr.readInt(offsetToInts + n * 4));
         }

         t.unk1 = dr.readInt(addr + 16);
         t.unk2 = dr.readInt(addr + 20);

         if(t.unk1 != 1 || t.unk2 != 0){
            int bp=42;
         }

         data3.set(t);
      }else if(data3.type == 91){
         Type91 t;

         int offsetA = dr.readInt(addr + 4);
         if(offsetA == 0) ffxReadError(path, L"Type95");
         int offsetB = dr.readInt(addr + 8);
         if(offsetB == 0) ffxReadError(path, L"Type95");

         int count = dr.readInt(addr + 12);
         for(int n = 0; n < count; ++n){
            t.floatsA.push_back(dr.readFloat(offsetA + n * 4));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 4));
         }

         t.unk1 = dr.readInt(addr + 16);
         int data3Offset = dr.readInt(addr + 20);
         if(t.unk1 != 1 || data3Offset == 0){
            int bp=42;
         }
         t.data3 = readData3(data3Offset);

         data3.set(t);
      }else if(data3.type == 95){
         Type95 t;

         int offsetA = dr.readInt(addr + 4);
         if(offsetA == 0) ffxReadError(path, L"Type95");
         int offsetB = dr.readInt(addr + 8);
         if(offsetB == 0) ffxReadError(path, L"Type95");

         int count = dr.readInt(addr + 12);
         for(int n = 0; n < count; ++n){
            t.floatsA.push_back(dr.readFloat(offsetA + n * 4));
            t.floatsB.push_back(dr.readFloat(offsetB + n * 4));
         }

         t.unk1 = dr.readInt(addr + 16);
         int data3Offset = dr.readInt(addr + 20);
         if(t.unk1 != 1 || data3Offset == 0){
            int bp=42;
         }
         t.data3 = readData3(data3Offset);

         data3.set(t);
      }else if(data3.type == 120){
         Type120 t;
         int data3OffsetA = dr.readInt(addr + 4);
         int data3OffsetB = dr.readInt(addr + 8);
         t.data3A = readData3(data3OffsetA);
         t.data3B = readData3(data3OffsetB);
         data3.set(t);
      }else if(data3.type == 112){
         // No params
      }else if(data3.type == 113){
         // No params
      }else if(data3.type == 111){
         Type115 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 114){
         Type114 t;
         int all = dr.readInt(addr + 4);
         short* sp = reinterpret_cast<short*>(&all);
         t.unk1 = sp[0];
         t.unk2 = sp[1];
         data3.set(t);
      }else if(data3.type == 115){
         Type115 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 121){
         Type121 t;

         int data3OffsetA = dr.readInt(addr + 4);
         int data3OffsetB = dr.readInt(addr + 8);

         t.data3A = readData3(data3OffsetA);
         t.data3B = readData3(data3OffsetB);

         data3.set(t);
      }else if(data3.type == 122){
         Type122 t;

         int data3OffsetA = dr.readInt(addr + 4);
         int data3OffsetB = dr.readInt(addr + 8);

         t.data3A = readData3(data3OffsetA);
         t.data3B = readData3(data3OffsetB);

         data3.set(t);
      }else if(data3.type == 123){
         Type123 t;

         int data3OffsetA = dr.readInt(addr + 4);
         int data3OffsetB = dr.readInt(addr + 8);

         t.data3A = readData3(data3OffsetA);
         t.data3B = readData3(data3OffsetB);

         data3.set(t);
      }else if(data3.type == 124){
         Type124 t;

         int data3OffsetA = dr.readInt(addr + 4);
         int data3OffsetB = dr.readInt(addr + 8);

         t.data3A = readData3(data3OffsetA);
         t.data3B = readData3(data3OffsetB);

         data3.set(t);
      }else if(data3.type == 126){
         Type126 t;

         int data3OffsetA = dr.readInt(addr + 4);
         int data3OffsetB = dr.readInt(addr + 8);

         t.data3A = readData3(data3OffsetA);
         t.data3B = readData3(data3OffsetB);

         data3.set(t);
      }else if(data3.type == 127){
         Type127 t;

         int data3OffsetA = dr.readInt(addr + 4);
         int data3OffsetB = dr.readInt(addr + 8);

         t.data3A = readData3(data3OffsetA);
         t.data3B = readData3(data3OffsetB);

         data3.set(t);
      }else if(data3.type == 128){
         Type128 t;

         int data3Offset = dr.readInt(addr + 4);

         t.data3 = readData3(data3Offset);

         data3.set(t);
      }else if(data3.type == 129){
         // No params
      }else if(data3.type == 130){
         // No params
      }else if(data3.type == 131){
         // No params
      }else if(data3.type == 132){
         // No params
      }else if(data3.type == 133){
         Type133 t133;

         t133.ffxId = dr.readInt(addr + 4);
         for(int n = 0; n < 7; ++n){
            t133.alwaysZero[n] = dr.readInt(addr + 8 + n * 4);
         }
         t133.always8Or10 = dr.readInt(addr + 9 * 4);
         t133.ast1 = readAST(addr + 10 * 4);
         t133.ast2 = readAST(addr + 16 * 4);

         t133.house = dr.readInt(addr + 22 * 4);
         t133.houseCount = dr.readInt(addr + 23 * 4);
         for(int h = 0; h < t133.houseCount; ++h){
            House* house = new House;
            house->offset = t133.house + h * 4 * 4;
            house->linksOffset = dr.readInt(house->offset + 0);
            house->blossomOffset = dr.readInt(house->offset + 4);
            house->linkCount = dr.readInt(house->offset + 8);
            house->blossomCount = dr.readInt(house->offset + 12);
            t133.houses.push_back(house);
         }

         for(House* house : t133.houses){
            for(int l = 0; l < house->linkCount; ++l){
               int linkedHouseOffset = dr.readInt(house->linksOffset + l * 8 + 0);
               int data3Offset = dr.readInt(house->linksOffset + l * 8 + 4);

               House::Link link;
               {
                  link.house = nullptr;
                  for(House* otherHouse : t133.houses){
                     if(otherHouse->offset == linkedHouseOffset){
                        link.house = otherHouse;
                     }
                  }
                  if(link.house == nullptr){
                     ffxReadError(path, L"invalid house link");
                  }
                  link.data3 = readData3(data3Offset);
               }
               house->links.push_back(link);
            }

            for(int b = 0; b < house->blossomCount; ++b){
               House::Blossom blossom;
               {
                  int blossomAndAstSize = 4 + 6 * 4;
                  blossom.blossomNumber = dr.readInt(house->blossomOffset + b * blossomAndAstSize + 0);
                  blossom.ast = readAST(house->blossomOffset + b * blossomAndAstSize + 4);
               }
               house->blossoms.push_back(blossom);
            }
         }

         data3.set(t133);
      }else if(data3.type == 134){
         Type134 t134;

         t134.ffxId = dr.readInt(addr + 4);
         t134.alwaysZero = dr.readInt(addr + 8);
         if(t134.alwaysZero != 0){
            ffxReadError(path, L"Type134 alwaysZero is not 0");
         }

         int offsetToArrayOfOffsets = dr.readInt(addr + 12);
         int type133Count = dr.readInt(addr + 16);
         for(int n = 0; n < type133Count; ++n){
            int offsetToType133 = dr.readInt(offsetToArrayOfOffsets + n * 4);
            t134.type133s.push_back(*readData3(offsetToType133).get<Type133>());
         }

         data3.set(t134);
      }else if(data3.type == 136){
         // No params
      }else if(data3.type == 137){
         Type137 t;
         t.unk1 = dr.readInt(addr + 4);
         t.unk2 = dr.readInt(addr + 8);
         t.unk3 = dr.readInt(addr + 12);
         t.unk4 = dr.readInt(addr + 16);
         data3.set(t);
      }else if(data3.type == 138){
         Type138 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 139){
         Type139 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 140){
         Type140 t;
         t.unk1 = dr.readInt(addr + 4);
         t.unk2 = dr.readInt(addr + 8);
         data3.set(t);
      }else{
         wchar_t wBuffer[80];
         swprintf(wBuffer, sizeof(wBuffer), L"Data3 type %d unfamiliar, addr = %d\n", data3.type, addr);
         ffxReadError(path, wBuffer);
      }

      return data3;
   };
   readAST = [&](int addr) -> AST{
      AST ast;

      int pond1Offset = dr.readInt(addr+0);
      int moose1 = dr.readInt(addr+4);
      int moose2 = dr.readInt(addr+8);
      if(moose1 != moose2){
         ffxReadError(path, L"moose1 != moose2");
      }
      ast.axisNumber = dr.readInt(addr+12);
      int pond2Offset = dr.readInt(addr+16);
      int pond3Offset = dr.readInt(addr+20);

      int an = ast.axisNumber;
      if(an != 0x00010000 && an != 0x00010100 && an != 0x00010101 && an != 0x00010001){
         printf("AST's axisNumber is unfamiliar: %d\n", ast.axisNumber);
      }

      if(pond2Offset % 16 != 0){
         ffxReadError(path, L"16-byte alignment");
      }

      if(pond1Offset){
         Pond1* pond1 = new Pond1;
         ast.pond1 = pond1;

         int lastAddr;
         for(int n = 0; n < moose1; ++n){
            lastAddr = pond1Offset + n * 4;
            int offsetToData3 = dr.readInt(lastAddr);
            pond1->data3s.push_back(readData3(offsetToData3));
         }

         if(lastAddr > lastPond1DataOffset){
            lastPond1DataOffset = lastAddr;
         }
      }

      if(pond2Offset){
         Pond2* pond2 = new Pond2;
         ast.pond2 = pond2;

         pond2->address = pond2Offset;
         pond2->type = dr.readInt(pond2Offset + 0);
         pond2->totalSize = dr.readInt(pond2Offset + 4);
         pond2->preDataCount = dr.readInt(pond2Offset + 8);
         //int snc = pond2->preDataCount;
         //if(snc != 0 && snc != 1 && snc != 2 && snc != 3 && snc != 5 && snc != 6){
         //   ffxReadError(path, L"pond2 unk1");
         //}
         pond2->offsetToPreDataNumbers = dr.readInt(pond2Offset + 12);
         pond2->offsetToPreDataSubtypes = dr.readInt(pond2Offset + 16);
         if(pond2->offsetToPreDataSubtypes - pond2->offsetToPreDataNumbers != pond2->preDataCount * 4){
            ffxReadError(path, L"pond2 someNumbersCount");
         }
         pond2->offsetToSourceAST = dr.readInt(pond2Offset + 20);
         for(int n = 24; n < pond2->totalSize; ++n){
            pond2->data.push_back(dr.readByte(pond2Offset + n));
         }

         if(allPond2Types){
            allPond2Types->insert(pond2->type);
            allPond2s->push_back(pond2);
         }

         DataReader drP;
         drP.bytes = pond2->data;
         drP.bytesRead.resize(drP.bytes.size());

         int currentDataOffset = 0;

         auto readInt = [&]() -> int{
            int result = drP.readInt(currentDataOffset);
            currentDataOffset += 4;
            return result;
         };
         auto readFloat = [&]() -> float{
            float result = drP.readFloat(currentDataOffset);
            currentDataOffset += 4;
            return result;
         };
         auto readSubtype = [&](int readAtLocalOffsetInstead = 0) -> void{
            if(readAtLocalOffsetInstead == 0){
               readAtLocalOffsetInstead = currentDataOffset;
               currentDataOffset += 8;
            }
            int subtype = drP.readInt(readAtLocalOffsetInstead + 0);
            int fullOffset = drP.readInt(readAtLocalOffsetInstead + 4);
            int offset = fullOffset - pond2->address - 24;

            if(subtype == 0){
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2; ++n){
                  drP.readFloat(offset + 4 + n * 4);
               }
            }else if(subtype == 4){
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2; ++n){
                  drP.readFloat(offset + 4 + n * 4);
               }
            }else if(subtype == 5){
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2 + 2; ++n){
                  drP.readFloat(offset + 4 + n * 4);
               }
            }else if(subtype == 6){
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2; ++n){
                  drP.readFloat(offset + 4 + n * 4);
               }
               int index = drP.readInt(offset + 4 + (floatCount * 2) * 4);
            }else if(subtype == 7){
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2 + 2; ++n){
                  drP.readFloat(offset + 4 + n * 4);
               }
               int index = drP.readInt(offset + 4 + (floatCount * 2 + 2) * 4);
            }else if(subtype == 8){
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 4; ++n){
                  drP.readFloat(offset + 4 + n * 4);
               }
            }else if(subtype == 9){
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 4 + 2; ++n){
                  drP.readFloat(offset + 4 + n * 4);
               }
            }else if(subtype == 12){
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2; ++n){
                  drP.readFloat(offset + 4 + n * 4);
               }
            }else if(subtype == 16){
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2; ++n){
                  drP.readFloat(offset + 4 + n * 4);
               }
            }else if(subtype == 17){
               int floatCount = drP.readInt(offset + 0);
               for(int n = 0; n < floatCount * 2 + 2; ++n){
                  drP.readFloat(offset + 4 + n * 4);
               }
            }else if(subtype == 24){
               drP.readFloat(offset + 0);
            }else if(subtype == 25){
               drP.readFloat(offset + 0);
               drP.readFloat(offset + 4);
               drP.readFloat(offset + 8);
            }else if(subtype == 26){
               drP.readFloat(offset + 0);
               drP.readInt(offset + 4);
            }else if(subtype == 27){
               drP.readFloat(offset + 0);
               drP.readFloat(offset + 4);
               drP.readFloat(offset + 8);
               drP.readFloat(offset + 12);
            }else if(subtype == 28){
               if(fullOffset != 0){
                  ffxReadError(path, L"Pond2 subtype 28 offset not 0");
               }
            }else{
               wchar_t wBuffer[80];
               swprintf(wBuffer, sizeof(wBuffer), L"Pond2 subtype %d unfamiliar, addr = %d\n", subtype, fullOffset);
               ffxReadError(path, wBuffer);
            }
         };
         auto readSubtypes = [&](int count) -> void{
            for(int n = 0; n < count; ++n){
               readSubtype();
            }
         };

         // Pre data
         if(pond2->preDataCount > 0){
            if(pond2->offsetToPreDataNumbers >= pond2->address + pond2->totalSize){
               ffxReadError(path, L"pond2 offsetToSomeNumbers is outside data");
            }else{
               int localNumbersOffset = pond2->offsetToPreDataNumbers - pond2->address - 24;
               int localSubtypesOffset = pond2->offsetToPreDataSubtypes - pond2->address - 24;
               for(int n = 0; n < pond2->preDataCount; ++n){
                  drP.readInt(localNumbersOffset + n * 4);
                  readSubtype(localSubtypesOffset + n * 8);
               }
            }
         }
            
         // Main data
         if(pond2->type == 27){
            readFloat();
            readFloat();
            readFloat();
            int shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            int texId = readInt();
            readInt();
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readSubtypes(10);
            readInt();
            readInt();
            readInt();
            readFloat();
         }else if(pond2->type == 28){
            readSubtypes(3);
            readInt();
         }else if(pond2->type == 29){
            readSubtypes(5);
            readInt();
         }else if(pond2->type == 30){
            readSubtypes(4);
            readFloat();
            readInt();
            readInt();
         }else if(pond2->type == 31){
            readSubtypes(4);
            readInt();
            readInt();
         }else if(pond2->type == 32){
            readSubtypes(6);
            readInt();
            readInt();
         }else if(pond2->type == 40){
            int shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readFloat();
            int texId = readInt();
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readInt();
            readInt();
            readInt();
            readSubtypes(4);
            readFloat();
            readFloat();
            readInt();
            readInt();
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readSubtypes(4);
            readInt();
            readInt();
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readSubtypes(1);
            readInt();
            readFloat();
            readSubtypes(2);
            readInt();
         }else if(pond2->type == 43){
            readFloat();
            int shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            int texId = readInt();
            readInt();
            readInt();
            readInt();
            readInt();
            readInt();
            readSubtypes(13);
            readInt();
         }else if(pond2->type == 55){
            readSubtypes(3);
            int shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readFloat();
         }else if(pond2->type == 59){
            int shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            int texId = readInt();
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readInt();
            readInt();
            readSubtypes(5);
            readInt();
            readInt();
            readSubtypes(8);
            readInt();
            readInt();
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readInt();
            readFloat();
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
         }else if(pond2->type == 61){
            int shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            int modelId = readInt();
            readInt();
            readInt();
            readInt();
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readSubtypes(3);
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readInt();
            readFloat();
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readSubtypes(1);
            readInt();
            readInt();
            readSubtypes(10);
            readInt();
            readInt();
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
         }else if(pond2->type == 66){
            readFloat();
            readFloat();
            int shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readInt();
            readFloat();
            readInt();
            readSubtypes(26);
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
         }else if(pond2->type == 70){
            readFloat();
            readFloat();
            readFloat();
            readInt();
            readFloat();
            int texId1 = readInt();
            int texId2 = readInt();
            int texId3 = readInt();
            readInt();
            readInt();
            readInt();
            readSubtypes(30);
            readInt();
            int shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readInt();
            readInt();
            readInt();
            readFloat();
            readInt();
            readFloat();
            readInt();
         }else if(pond2->type == 71){
            readFloat();
            readFloat();
            readFloat();
            readInt();
            readFloat();
            int texId = readInt();
            int shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readInt();
            readInt();
            readInt();
            readSubtypes(10);
            readInt();
            readInt();
            readSubtypes(10);
            readInt();
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readInt();
            readInt();
            readInt();
            readFloat();
            readInt();
            readFloat();
            readInt();
         }else if(pond2->type == 84){
            readSubtypes(3);
            int shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readFloat();
            readSubtypes(1);
            readInt();
         }else if(pond2->type == 105){
            readSubtypes(3);
            int shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readFloat();
            readSubtypes(1);
            readInt();
            readSubtypes(1);
         }else if(pond2->type == 107){
            int shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            int texId = readInt();
            readInt();
            readSubtypes(7);
         }else if(pond2->type == 108){
            readFloat();
            readFloat();
            readFloat();
            readInt();
            readFloat();
            int ffxId = readInt();
            readInt();
            readInt();
            readInt();
            int shouldBeZero = readInt();
            if(shouldBeZero != 0) ffxReadError(path, L"Pond2");
            readSubtypes(12);
            readInt();
            readInt();
            readSubtypes(14);
            readInt();
            readInt();
            readInt();
            readFloat();
            readInt();
         }else{
            ffxReadError(path, L"pond2 unknown type");
         }

         if(pond2->address + 24 + currentDataOffset != pond2->offsetToPreDataNumbers){
            ffxReadError(path, L"pond2 offsetToSomeNumbers");
         }

         bool hasMissedAtLeastOne = false;
         for(size_t n = 0; n < drP.bytesRead.size(); ++n){
            if(drP.bytesRead[n] == false){
               if(hasMissedAtLeastOne == false){
                  wprintf(L"%s pond2 @ %d has not been read 100%%\n", path.c_str(), pond2->address);
                  hasMissedAtLeastOne = true;
               }

               if(n % 4 == 0){
                  printf("int @ %6d (%d) has not been read\n", pond2->address + 24 + n, *((int*)(&drP.bytes[n])));
               }
            }
         }

         if(hasMissedAtLeastOne){
            printf("\n\n");
         }

         readPadding(pond2->address + pond2->totalSize, 16);
      }

      if(pond3Offset){
         //if(hasReadPond3Yet == false){
         //   hasReadPond3Yet = true;

         //   int padding1 = dr.readInt(pond3Offset - 8);
         //   int padding2 = dr.readInt(pond3Offset - 4);
         //   if(padding1 != 0 || padding2 != 0){
         //      int bp=42;
         //   }
         //}

         Pond3* pond3 = new Pond3;

         pond3->data.type = dr.readInt(pond3Offset + 0);

         int type = pond3->data.type;
         if(type == 0){
            Pond3Type0 t;
            t.unk1 = dr.readInt(pond3Offset + 4);
            pond3->data.set(t);
         }else if(type == 1){
            Pond3Type1 t;
            t.unk1 = dr.readInt(pond3Offset + 4);
            t.unk2 = dr.readInt(pond3Offset + 8);
            t.unk3 = dr.readInt(pond3Offset + 12);
            t.unk4 = dr.readFloat(pond3Offset + 16);
            t.unk5 = dr.readFloat(pond3Offset + 20);
            t.unk6 = dr.readInt(pond3Offset + 24);
            t.unk7 = dr.readInt(pond3Offset + 28);
            t.unk8 = dr.readInt(pond3Offset + 32);
            t.unk9 = dr.readInt(pond3Offset + 36);
            t.unk10 = dr.readInt(pond3Offset + 40);
            t.unk11 = dr.readFloat(pond3Offset + 44);
            pond3->data.set(t);
         }else if(type == 2){
            Pond3Type2 t;
            t.unk1 = dr.readFloat(pond3Offset + 4);
            t.unk2 = dr.readFloat(pond3Offset + 8);
            t.unk3 = dr.readInt(pond3Offset + 12);
            t.unk4 = dr.readFloat(pond3Offset + 16);
            t.unk5 = dr.readInt(pond3Offset + 20);
            pond3->data.set(t);
         }else if(type == 3){
            Pond3Type3 t;
            t.unk1 = dr.readFloat(pond3Offset + 4);
            t.unk2 = dr.readInt(pond3Offset + 8);
            t.unk3 = dr.readFloat(pond3Offset + 12);
            t.unk4 = dr.readInt(pond3Offset + 16);
            pond3->data.set(t);
         }else if(type == 4){
            Pond3Type4 t;
            t.unk1 = dr.readInt(pond3Offset + 4);
            pond3->data.set(t);
         }else if(type == 5){
            Pond3Type5 t;
            t.unk1 = dr.readInt(pond3Offset + 4);
            pond3->data.set(t);
         }else if(type == 6){
            Pond3Type6 t;
            t.unk1 = dr.readFloat(pond3Offset + 4);
            t.unk2 = dr.readInt(pond3Offset + 8);
            pond3->data.set(t);
         }else if(type == 7){
            Pond3Type7 t;
            t.unk1 = dr.readInt(pond3Offset + 4);
            if(t.unk1 != 0){
               int bp=42;
            }
            pond3->data.set(t);
         }else{
            wchar_t wBuffer[80];
            swprintf(wBuffer, sizeof(wBuffer), L"Pond3 type %d umfamiliar, addr = %d\n", type, pond3Offset);
            ffxReadError(path, wBuffer);
         }

         ast.pond3 = pond3;
      }

      return ast;
   };

   char sig1[4] = "FXR";
   char sig2[4];
   memcpy(sig2, &dr.bytes[0], 4);
   if(memcmp(sig1, sig2, 4) != 0){
      ffxReadError(path, L"Not an ffx file");
   }
   br[3] = br[2] = br[1] = br[0] = true;

   int versionRaw = dr.readInt(4);
   ffx.version = (int)((short*)(&versionRaw))[1];
   int dataStartAfterHeader = dr.readInt(8);
   int data2Start = dr.readInt(12);
   int data2Count = dr.readInt(16);
   int data3Start = data2Start + data2Count * 4;
   int data3Count = dr.readInt(20);
   ffx.unk1 = dr.readInt(24);
   ffx.unk2 = dr.readInt(28);

   FlexibleData topData3 = readData3(dataStartAfterHeader);

   if(lastPond1DataOffset != 0){
      readPadding(lastPond1DataOffset + 4, 16);
   }

   // data2 section always starts at a multiple of 16 for some reason
   int lastByteRead = 0;
   for(size_t b = 0; b < dr.bytesRead.size(); b += 4){
      if(dr.bytesRead[b] == true){
         lastByteRead = b;
      }else{
         break;
      }
   }
   readPadding(lastByteRead + 4, 16);

   // Count data2 and data3 offsets at the end as read
   for(int n = 0; n < data2Count; ++n){
      dr.readInt(data2Start + n * 4);
   }
   for(int n = 0; n < data3Count; ++n){
      dr.readInt(data3Start + n * 4);
   }

   bool hasMissedAtLeastOne = false;
   for(size_t n = 0; n < dr.bytesRead.size(); ++n){
      if(dr.bytesRead[n] == false){
         if(hasMissedAtLeastOne == false){
            wprintf(L"%s has not been read 100%%\n", path.c_str());
            hasMissedAtLeastOne = true;
         }

         if(n % 4 == 0){
            printf("int @ %6d (%d) has not been read\n", n, *((int*)(&dr.bytes[n])));
         }
      }
   }

   if(hasMissedAtLeastOne){
      printf("\n\n");
   }
}
