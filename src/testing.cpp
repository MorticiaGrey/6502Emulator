#include "../lib/testing.h"
#include <iostream>
#include <string>

// This will be how I test the functionality of the cpu

void Testing::Init()
{
    ExpectedMemory.Initialize();
    ExpectedProcessorStatus = 0;
    ExA = ExX = ExY = 0;
}

void Testing::SetExpectedProcessorStatus(ProcessorStatus Status)
{
   UnionProcessorStatus = Status;
}

void Testing::SetExpectedProcessorStatus(char Flag, bool Value)
{
    switch (Flag)
    {
        case 'C':
        {
            UnionProcessorStatus.C = Value;
        } break;
        case 'Z':
        {
            UnionProcessorStatus.Z = Value;
        } break;
        case 'I':
        {
            UnionProcessorStatus.I = Value;
        } break;
        case 'D':
        {
            UnionProcessorStatus.D = Value;
        } break;
        case 'B':
        {
            UnionProcessorStatus.B = Value;
        } break;
        case 'V':
        {
            UnionProcessorStatus.V = Value;
        } break;
        case 'N':
        {
            UnionProcessorStatus.N = Value;
        } break;
        default:
        {
            printf("Invalid Flag Passed to 'Testing::SetExpectedProcessorStatus'");
            throw -1;
        } break;
    }
}

void Testing::SetExpectedRegister(char Register, Byte Value)
{
    switch (Register)
    {
        case 'A':
        {
            ExA = Value;
        } break;
        case 'X':
        {
            ExX = Value;
        } break;
        case 'Y':
        {
            ExY = Value;
        } break;
        default:
        {
            printf("Invalid Register Passed to 'Testing::SetExpectedRegister'");
            throw -1;
        } break;
    }
}

void Testing::SetExpectedMemory(Word Addr, u32 Value)
{
    SetMemTemplate.Initialize(Addr, Value);
    ExpectedMemSet[ExMemSetAmount] = SetMemTemplate;
    ExMemSetAmount++;
}

void Testing::SetMemOnExecute(Word Addr, u32 Data)
{
    SetMemTemplate.Initialize(Addr, Data);
    TestMemorySet[MemSetAmount] = SetMemTemplate;
    MemSetAmount++;
}

void Testing::SetRegisterOnExecute(char Register, Byte Value)
{
switch (Register)
    {
        case 'A':
        {
            A = Value;
        } break;
        case 'X':
        {
            X = Value;
        } break;
        case 'Y':
        {
            Y = Value;
        } break;
        default:
        {
            printf("Invalid Register Passed to 'Testing::SetRegisterOnExecute'");
            throw -1;
        } break;
    }

    SetExpectedRegister(Register, Value);
}

// Addressing mode presets

void Testing::TestValueIM(Byte Opcode, s32 Operand)
{
    switch(Operand)
    {
        case -3:
        {
            SetExpectedRegister('A', 0xFF);
        } break;
        case -2:
        {
            SetExpectedRegister('X', 0xFF);
        } break;
        case -1:
        {
            SetExpectedRegister('Y', 0xFF);
        } break;
        default:
        {
            // Sets expected memory
            SetExpectedMemory(Operand, 0xFF);
        } break;
    }
    Execute(Opcode, 0xFF);
}

void Testing::TestValueZP(Byte Opcode, s32 Operand)
{
    switch(Operand)
    {
        case -3:
        {
            SetExpectedRegister('A', 0xFF);
        } break;
        case -2:
        {
            SetExpectedRegister('X', 0xFF);
        } break;
        case -1:
        {
            SetExpectedRegister('Y', 0xFF);
        } break;
        default:
        {
            // Sets expected memory
            SetExpectedMemory(Operand, 0xFF);
        } break;
    }
    SetMemOnExecute(0x0010, 0xFF);
    Execute(Opcode, 0x10);
}

void Testing::TestValueZPX(Byte Opcode, s32 Operand)
{
    switch(Operand)
    {
        case -3:
        {
            SetExpectedRegister('A', 0xFF);
        } break;
        case -2:
        {
            SetExpectedRegister('X', 0xFF);
        } break;
        case -1:
        {
            SetExpectedRegister('Y', 0xFF);
        } break;
        default:
        {
            // Sets expected memory
            SetExpectedMemory(Operand, 0xFF);
        } break;
    }
    SetRegisterOnExecute('X', 0x01);
    SetMemOnExecute(0x0011, 0xFF);
    Execute(Opcode, 0x10);
}

void Testing::TestValueZPY(Byte Opcode, s32 Operand)
{
    switch(Operand)
    {
        case -3:
        {
            SetExpectedRegister('A', 0xFF);
        } break;
        case -2:
        {
            SetExpectedRegister('X', 0xFF);
        } break;
        case -1:
        {
            SetExpectedRegister('Y', 0xFF);
        } break;
        default:
        {
            // Sets expected memory
            SetExpectedMemory(Operand, 0xFF);
        } break;
    }
    SetRegisterOnExecute('Y', 0x01);
    SetMemOnExecute(0x0011, 0xFF);
    Execute(Opcode, 0x10);
}

void Testing::TestValueABS(Byte Opcode, s32 Operand)
{
   switch(Operand)
    {
        case -3:
        {
            SetExpectedRegister('A', 0xFF);
        } break;
        case -2:
        {
            SetExpectedRegister('X', 0xFF);
        } break;
        case -1:
        {
            SetExpectedRegister('Y', 0xFF);
        } break;
        default:
        {
            // Sets expected memory
            SetExpectedMemory(Operand, 0xFF);
        } break;
    }
    SetMemOnExecute(0xAAAA, 0xFF);
    Execute(Opcode, 0xAAAA);
}

void Testing::TestValueABSX(Byte Opcode, s32 Operand)
{
    switch(Operand)
    {
        case -3:
        {
            SetExpectedRegister('A', 0xFF);
        } break;
        case -2:
        {
            SetExpectedRegister('X', 0xFF);
        } break;
        case -1:
        {
            SetExpectedRegister('Y', 0xFF);
        } break;
        default:
        {
            // Sets expected memory
            SetExpectedMemory(Operand, 0xFF);
        } break;
    }
    SetMemOnExecute(0xAAAB, 0xFF);
    SetRegisterOnExecute('X', 1);
    Execute(Opcode, 0xAA);
}

void Testing::TestValueABSY(Byte Opcode, s32 Operand)
{
    switch(Operand)
    {
        case -3:
        {
            SetExpectedRegister('A', 0xFF);
        } break;
        case -2:
        {
            SetExpectedRegister('X', 0xFF);
        } break;
        case -1:
        {
            SetExpectedRegister('Y', 0xFF);
        } break;
        default:
        {
            // Sets expected memory
            SetExpectedMemory(Operand, 0xFF);
        } break;
    }
    SetMemOnExecute(0xAAAB, 0xFF);
    SetRegisterOnExecute('Y', 1);
    Execute(Opcode, 0xAA);
}

void Testing::TestValueINDX(Byte Opcode, s32 Operand)
{
    /*
    SetExpectedRegister('A', 0xFF);
    SetRegisterOnExecute('X', 1);
    SetMemOnExecute(0x0011, 0xAAAA);
    SetMemOnExecute(0xAAAA, 0xFF); // Final Location
    Execute(0xA1, 0x10);
    */
    switch(Operand)
    {
        case -3:
        {
            SetExpectedRegister('A', 0xFF);
        } break;
        case -2:
        {
            SetExpectedRegister('X', 0xFF);
        } break;
        case -1:
        {
            SetExpectedRegister('Y', 0xFF);
        } break;
        default:
        {
            // Sets expected memory
            SetExpectedMemory(Operand, 0xFF);
        } break;
    }
    SetRegisterOnExecute('X', 1);
    SetMemOnExecute(0x0011, 0xAAAA);
    SetMemOnExecute(0xAAAA, 0xFF); // Location in mem A will be loaded from
    Execute(Opcode, 0x10);
}

void Testing::TestValueINDY(Byte Opcode, s32 Operand)
{
    switch(Operand)
    {
        case -3:
        {
            SetExpectedRegister('A', 0xFF);
        } break;
        case -2:
        {
            SetExpectedRegister('X', 0xFF);
        } break;
        case -1:
        {
            SetExpectedRegister('Y', 0xFF);
        } break;
        default:
        {
            // Sets expected memory
            SetExpectedMemory(Operand, 0xFF);
        } break;
    }
    SetRegisterOnExecute('Y', 1);
    SetMemOnExecute(0x0011, 0xAAAA);
    SetMemOnExecute(0xAAAA, 0xFF); // Location in mem A will be loaded from
    Execute(Opcode, 0x10);
}

void Testing::DefaultLoadOpExpectedPS()
{
    SetExpectedProcessorStatus('N', true);
    SetExpectedProcessorStatus('Z', false);
}

// Execution of tests
void Testing::ExecuteStartSequence(Byte opcode, u32 operand)
{
    cpu.Reset(memory);
    memory[cpu.PC] = opcode;
    memory[cpu.PC + 1] = operand;

    for (int i = 0; i < MemSetAmount; i++)
    {
        memory[TestMemorySet[i].Location] = TestMemorySet[i].Data;
    }

    cpu.Exit = true;
    Cycles = 0;

    cpu.A = A;
    cpu.X = X;
    cpu.Y = Y;

    ExpectedMemory = memory;

    for (int j = 0; j < ExMemSetAmount; j++)
    {
        ExpectedMemory[ExpectedMemSet[j].Location] = ExpectedMemSet[j].Data;
    }
}

void Testing::Execute(Byte opcode, u32 operand)
{
    ExecuteStartSequence(opcode, operand);

    cpu.Execute(Cycles, memory);

    bool MemValid = memory == ExpectedMemory;
    bool StatusValid = cpu.PS == ExpectedProcessorStatus;
    bool RegistersValid = (ExA == cpu.A && ExX == cpu.X && ExY == cpu.Y);

    if (MemValid && StatusValid && RegistersValid)
    {
        printf("\n\033[1;32mOpcode: 0x%X\tSuccess\033[0m\n", opcode);
    } else
    {
        printf("\n\033[1;31mOpcode: 0x%X\tMemory: %d\tProcessor Status: %d\tA: %d\tX: %d\tY: %d\tFailure\033[0m\n", opcode, MemValid, StatusValid, ExA == cpu.A, ExX == cpu.X, ExY == cpu.Y);
        printf("\033[1;31m>CPU Values:\t\tStatus: %X\tA: %X\tX: %X\tY: %X\n\033[0m", cpu.PS, cpu.A, cpu.X, cpu.Y);
        printf("\033[1;31m>Expected Values:\tStatus: %X\tA: %X\tX: %X\tY: %X\n\033[0m", ExpectedProcessorStatus, ExA, ExX, ExY);
    }

    // Activated if InspectMode is set, for actively messing with memory before tests
    while(InspectMode)
    {
        int ins_input = -1;
        int addr_input = -1;
        int value_input = -1;

        // Read input
        scanf("%i", &ins_input);

        //printf("\n\n%X\t%X\t%X\n", ins_input, addr_input, value_input);

        switch (ins_input)
        {
            case 1: // Set memory location
            {
                printf("\n[Addres] [Value]\n");
                scanf("%i %i", &addr_input, &value_input);
                memory[addr_input] = value_input;
            } break;
            case 2: // Show memory location
            {
                printf("\n[Addres]\n");
                scanf("%i", &addr_input);
                printf("\n%X\n", memory[addr_input]);
            } break;
            case 3: // Execute
            {
                Execute(opcode, operand);
            } break;
            case 4: // Reset Registers
            {
                printf("\nRegisters Reset\n");
                cpu.PS = cpu.A = cpu.X = cpu.Y = 0;
            } break;
            case 5: // Reset Memory
            {
                printf("\nMemory Reset\n");
                memory.Initialize();
            } break;
            case 6: // Exit
            {
                printf("\nInspect Mode Exited\n");
                return;
            } break;
            default: // If nothing else prints it was not recognized
            {
                printf("\nInstruction Not Recognized\n");
            }
        }
    }

    MemSetAmount = 0;
    ExMemSetAmount = 0;
    CompMem = CompRegisters = CompStatus = true;
    A = X = Y = 0;
    ExA = ExX = ExY = 0;
    ExpectedProcessorStatus = 0;
    InspectMode = false;
}

void Testing::Test()
{
    printf("\n0 = Failed, 1 = Success\n");

    // So I know what the tests are
    printf("\n\033[1;33mLDA\033[0m\n"); //"\n\033[1;31m   \033[0m\n"

    // A9 opcode, or INS_LDA_IM
    DefaultLoadOpExpectedPS();
    TestValueIM(0xA9, -3);

    // A5, or INS_LDA_ZP
    DefaultLoadOpExpectedPS();
    TestValueZP(0xA5, -3);

    // B5, or INS_LDA_ZPX
    DefaultLoadOpExpectedPS();
    TestValueZPX(0xB5, -3);

    // AD, INS_LDA_ABS
    DefaultLoadOpExpectedPS();
    TestValueABS(0xAD, -3);

    // BD, INS_LDA_ABSX
    DefaultLoadOpExpectedPS();
    TestValueABSX(0xBD, -3);
    
    // B9, INS_LDA_ABSY
    DefaultLoadOpExpectedPS();
    TestValueABSY(0xB9, -3);

    // A1, INS_LDA_INDX
    DefaultLoadOpExpectedPS();
    TestValueINDX(0xA1, -3);

    // B1, INS_LDA_INDY
    DefaultLoadOpExpectedPS();
    TestValueINDY(0xB1, -3);

    printf("\n\033[1;33mLDX\033[0m\n");

    // A2, INS_LDX_IM
    DefaultLoadOpExpectedPS();
    TestValueIM(0xA2, -2);

    // A6, INS_LDX_ZP
    DefaultLoadOpExpectedPS();
    TestValueZP(0xA6, -2);

    // B6, INS_LDX_ZPY
    DefaultLoadOpExpectedPS();
    TestValueZPY(0xB6, -2);

    // AE, INS_LDX_ABS
    DefaultLoadOpExpectedPS();
    TestValueABS(0xAE, -2);

    // BE, INS_LDX_ABSY
    DefaultLoadOpExpectedPS();
    TestValueABSY(0xBE, -2);

    printf("\n\033[1;33mLDY\033[0m\n");

    // A0, INS_LDY_IM
    DefaultLoadOpExpectedPS();
    TestValueIM(0xA0, -1);

    // A4, INS_LDY_ZP
    DefaultLoadOpExpectedPS();
    TestValueZP(0xA4, -1);

    // B4, INS_LDY_ZPX
    DefaultLoadOpExpectedPS();
    TestValueZPX(0xB4, -1);

    // AC, INS_LDY_ABS
    DefaultLoadOpExpectedPS();
    TestValueABS(0xAC, -1);

    // BC, INS_LDY_ABSX
    DefaultLoadOpExpectedPS();
    TestValueABSX(0xBC, -1);

    printf("\n\033[1;33mSTA\033[0m\n");

    // 85, INS_STA_ZP
    SetRegisterOnExecute('A', 0xFF);
    TestValueZP(0x85, -3);

    std::cout << "\n";
}
