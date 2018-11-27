#include "header.hpp"


struct DataReader{
   std::vector<byte> bytes;
   std::vector<bool> bytesRead;

   int readInt(int addr){
      if(addr == 468){
         int bp=42;
      }

      std::vector<bool>& br = bytesRead;
      br[addr+3] = br[addr+2] = br[addr+1] = br[addr+0] = true;
      return *reinterpret_cast<int*>(&bytes[addr]);
   }

   float readFloat(int addr){
      std::vector<bool>& br = bytesRead;
      br[addr+3] = br[addr+2] = br[addr+1] = br[addr+0] = true;
      return *reinterpret_cast<float*>(&bytes[addr]);
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

struct Pond1{
   std::vector<FlexibleData> data3s;
};

struct Pond2{
   int someCount = -1;
   int totalSize = -1;
   int alwaysZero = -1;
   int offset1 = -1;
   int offset2 = -1;
   int offsetToSourceAST = -1;
   std::vector<int> unknownData;
};

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

struct Type46{
   int unk1;
};

struct Type59{
   int unk1;
};

struct Type60{
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

struct Type85{
   float unk1;
   float unk2;
};

// 113 has no params

struct Type115{
   int unk1;
};

struct Type120{
   FlexibleData data3A;
   FlexibleData data3B;
};

struct Type133{
   int ffxId;
   int alwaysZero[7];
   int always8Or10;
   AST ast1;
   AST ast2;
   int house;
      int crepe1;
         int monkey;
            int monkeyTail1;
            int monkeyTail2;
               int hair;
               AST hairAST;
            int monkeyTail3;
            int monkeyTail4;
         int turtle;
            FlexibleData turtleEgg;
      int crepe2;
         int blossom;
         AST blossomAST;
      int tulip1;
      int tulip2;
   int ostrich;
};

struct Type134{
   int ffxId;
   int alwaysZero;
   std::vector<Type133> type133s;
};

// Type136 has no params

void loadFfxFile(Ffx& ffx, std::wstring path){
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

   bool hasReadPond3Yet = false;

   std::function<AST(int)> readAST;
   std::function<FlexibleData(int)> readData3;
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
      }else if(data3.type == 46){
         Type46 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 59){
         Type59 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 60){
         Type60 t;
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
      }else if(data3.type == 120){
         Type120 t;
         int data3OffsetA = dr.readInt(addr + 4);
         int data3OffsetB = dr.readInt(addr + 8);
         t.data3A = readData3(data3OffsetA);
         t.data3B = readData3(data3OffsetB);
         data3.set(t);
      }else if(data3.type == 113){
         // No params
      }else if(data3.type == 115){
         Type115 t;
         t.unk1 = dr.readInt(addr + 4);
         data3.set(t);
      }else if(data3.type == 133){
         Type133 t133;

         t133.ffxId = dr.readInt(addr + 4);
         ffx.id = t133.ffxId;
         for(int n = 0; n < 7; ++n){
            t133.alwaysZero[n] = dr.readInt(addr + 8 + n * 4);
         }
         t133.always8Or10 = dr.readInt(addr + 9 * 4);
         t133.ast1 = readAST(addr + 10 * 4);
         t133.ast2 = readAST(addr + 16 * 4);

         t133.house = dr.readInt(addr + 22 * 4);
         t133.ostrich = dr.readInt(addr + 23 * 4);
         if(t133.house == 0) ffxReadError(path, L"house is 0");
         if(t133.ostrich == 1 || t133.ostrich == 2 || t133.ostrich == 5){
            t133.crepe1 = dr.readInt(t133.house + 0 * 4);
            if(t133.crepe1 != 0){
               t133.monkey = dr.readInt(t133.crepe1 + 0 * 4);
               if(t133.monkey == 0) ffxReadError(path, L"monkey is 0");
               {
                  t133.monkeyTail1 = dr.readInt(t133.monkey + 0 * 4);
                  t133.monkeyTail2 = dr.readInt(t133.monkey + 1 * 4);
                  if(t133.monkeyTail2 == 0) ffxReadError(path, L"monkeyTail2 is 0");
                  {
                     t133.hair = dr.readInt(t133.monkeyTail2 + 0 * 4);
                     t133.hairAST = readAST(t133.monkeyTail2 + 1 * 4);
                  }
                  t133.monkeyTail3 = dr.readInt(t133.monkey + 2 * 4);
                  t133.monkeyTail4 = dr.readInt(t133.monkey + 3 * 4);
               }
               t133.turtle = dr.readInt(t133.crepe1 + 1 * 4);
               if(t133.turtle == 0) ffxReadError(path, L"turtle is 0");
               t133.turtleEgg = readData3(t133.turtle);
               if(t133.turtleEgg.type != 136) ffxReadError(path, L"turtleEgg");
            }else{
               t133.monkey = 0;
               t133.monkeyTail1 = 0;
               t133.monkeyTail2 = 0;
               t133.monkeyTail3 = 0;
               t133.monkeyTail4 = 0;
               t133.turtle = 0;
            }
            t133.crepe2 = dr.readInt(t133.house + 1 * 4);
            if(t133.crepe2 == 0) ffxReadError(path, L"crepe2 is 0");
            {
               t133.blossom = dr.readInt(t133.crepe2 + 0 * 4);
               bool success = false;
               for(int b : {14, 17, 79, 83, 87, 95}){
                  if(t133.blossom == b){
                     success = true;
                     break;
                  }
               }
               if(success == false){
                  ffxReadError(path, L"blossom is unfamiliar");
               }
               t133.blossomAST = readAST(t133.crepe2 + 1 * 4);
            }
            t133.tulip1 = dr.readInt(t133.house + 2 * 4);
            if(t133.tulip1 != 0 && t133.tulip1 != 1) ffxReadError(path, L"tulip1 is unfamiliar");
            t133.tulip2 = dr.readInt(t133.house + 3 * 4);
            if(t133.tulip2 != 1 && t133.tulip2 != 2) ffxReadError(path, L"tulip2 is unfamiliar");

            //if(t133.ostrich == 5){
            //   int bp=42;
            //}
         }else{
            wchar_t wBuffer[80];
            swprintf(wBuffer, 80, L"ostrich %d not supported, addr = %d\n", t133.ostrich, addr + 23 * 4);
            ffxReadError(path, wBuffer);
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

      if(pond1Offset){
         Pond1* pond1 = new Pond1;

         int lastAddr;
         for(int n = 0; n < moose1; ++n){
            lastAddr = pond1Offset + n * 4;
            int offsetToData3 = dr.readInt(lastAddr);
            pond1->data3s.push_back(readData3(offsetToData3));
         }

         lastAddr += 4;

         // Check byte alignment
         //if(moose1 && lastAddr % 16 != 0){
         //   while(lastAddr % 16 != 0){
         //      int prev1 = dr.readInt(lastAddr - 8);
         //      int prev2 = dr.readInt(lastAddr - 4);
         //      int padding = dr.readInt(lastAddr);
         //      if(padding != 0){
         //         ffxReadError(path, L"padding is not 0");
         //      }
         //      lastAddr += 4;
         //   }
         //}

         ast.pond1 = pond1;
      }

      if(pond2Offset){
         Pond2* pond2 = new Pond2;

         pond2->someCount = dr.readInt(pond2Offset + 0);
         pond2->totalSize = dr.readInt(pond2Offset + 4);
         pond2->alwaysZero = dr.readInt(pond2Offset + 8);
         pond2->offset1 = dr.readInt(pond2Offset + 12);
         pond2->offset2 = dr.readInt(pond2Offset + 16);
         pond2->offsetToSourceAST = dr.readInt(pond2Offset + 20);
         for(int n = 24; n < pond2->totalSize; n += 4){
            pond2->unknownData.push_back(dr.readInt(pond2Offset + n));
         }

         ast.pond2 = pond2;
      }

      if(pond3Offset){
         if(hasReadPond3Yet == false){
            hasReadPond3Yet = true;

            int padding1 = dr.readInt(pond3Offset - 8);
            int padding2 = dr.readInt(pond3Offset - 4);
            if(padding1 != 0 || padding2 != 0){
               int bp=42;
            }
         }

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


   ffx.version = (short)dr.readInt(4);
   int dataStartAfterHeader = dr.readInt(8);
   int data2Start = dr.readInt(12);
   int data2Count = dr.readInt(16);
   int data3Start = data2Start + data2Count * 4;
   int data3Count = dr.readInt(20);
   ffx.unk1 = dr.readInt(24);
   ffx.unk2 = dr.readInt(28);

   FlexibleData topData3 = readData3(dataStartAfterHeader);

   // Count data2 and data3 offsets at the end as read
   for(int n = 0; n < data2Count; ++n){
      dr.readInt(data2Start + n * 4);
   }
   for(int n = 0; n < data3Count; ++n){
      dr.readInt(data3Start + n * 4);
   }

   //bool hasMissedAtLeastOne = false;
   //for(size_t n = 0; n < dr.bytesRead.size(); ++n){
   //   if(dr.bytesRead[n] == false && dr.bytes[n] != 0){
   //      if(hasMissedAtLeastOne == false){
   //         wprintf(L"%s has not been read 100%%\n", path.c_str());
   //         hasMissedAtLeastOne = true;
   //      }

   //      if(n % 4 == 0){
   //         printf("byte %4d has not been read\n", n);
   //      }
   //   }
   //}

   //if(hasMissedAtLeastOne){
   //   printf("\n\n");
   //}
}
