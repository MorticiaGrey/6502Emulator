#include "main.h"

// This is the header file for the testing.cpp file

struct SetMem
{
    Byte Location;
    Byte Data;

    void Initialize(Word InLocation, u32 InData)
    {
        Location = InLocation;
        Data = InData;
    }
};

struct SetExpectValue
{
    int& Value;
    int ExpectedValue;

    void Initialize(int& iValue, int iExpectedValue)
    {
        Value = iValue;
        ExpectedValue = iExpectedValue;
    }
};

class Testing
{
    public:
        void Init();
        void Execute(Byte opcode, u32 operand);
        void Test();
    private:
        void SetExpectedProcessorStatus(ProcessorStatus Status);
        void SetExpectedProcessorStatus(char Flag, bool Value);
        void SetExpectedRegister(char Register, Byte Value);
        void SetExpectedMemory(Word Addr, u32 Value);
        void SetMemOnExecute(Word Addr, u32 Data);
        void SetRegisterOnExecute(char Register, Byte Data);
        void ExecuteStartSequence(Byte opcode, u32 operand);

        // Tests value based on different addressing modes automatically
        // -3 = A, -2 = X, -1 = Y, else = memory[operand]

        void TestValueIM(Byte Opcode, s32 Operand);
        void TestValueZP(Byte Opcode, s32 Operand);
        void TestValueZPX(Byte Opcode, s32 Operand);
        void TestValueZPY(Byte Opcode, s32 Operand);
        void TestValueABS(Byte Opcode, s32 Operand);
        void TestValueABSX(Byte Opcode, s32 Operand);
        void TestValueABSY(Byte Opcode, s32 Operand);
        void TestValueINDX(Byte Opcode, s32 Operand);
        void TestValueINDY(Byte Opcode, s32 Operand);

        void DefaultLoadOpExpectedPS();

        Mem memory, ExpectedMemory;
        union
        {
            Byte ExpectedProcessorStatus;
            ProcessorStatus UnionProcessorStatus;
        };
        CPU cpu;
        s32 Cycles;
        Byte ExA, ExX, ExY;
        Byte A, X, Y;
        SetMem SetMemTemplate;
        SetMem TestMemorySet[99] = {};
        SetMem ExpectedMemSet[99] = {};
        int ExMemSetAmount = 0;
        int MemSetAmount = 0;
        bool CompMem = true;
        bool CompStatus = true;
        bool CompRegisters = true;

        bool InspectMode = false;
};
