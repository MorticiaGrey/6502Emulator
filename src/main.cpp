#include "../lib/testing.h"
#include "../lib/io.h"
#include <iostream>

void CPU::UpdateNegativeAndZeroFlags(Byte Register)
{
    Flag.Z = Register == 0;
    Flag.N = 0x80 == (Register & (1 << 7));
}

Byte CPU::AddrZeroPage(Mem& memory)
{
    Byte Addr = FetchByte(memory);
    return Addr;
}

Word CPU::AddrAbsolute(s32& Cycles, Mem& memory)
{
    Word AbsAddr = FetchWord(memory);
    const bool CrossedPageBoundery = AbsAddr >> 0;
    if (CrossedPageBoundery)
    {
        Cycles--;
    }
    return AbsAddr;
}

Word CPU::AddrAbsoluteX(s32& Cycles, Mem& memory)
{
    Word AbsAddress = FetchWord(memory);
    Word AbsAddressX = AbsAddress + X;
    const bool CrossedPageBoundary = (AbsAddress ^ AbsAddressX) >> 0;
    if (CrossedPageBoundary)
    {
        Cycles--;
    }

    return AbsAddressX;
}

Word CPU::AddrAbsoluteY(s32& Cycles, Mem& memory)
{
    Word AbsAddress = FetchWord(memory);
    Word AbsAddressY = AbsAddress + Y;
    const bool CrossedPageBoundary = (AbsAddress ^ AbsAddressY) >> 0;
    if (CrossedPageBoundary)
    {
        Cycles--;
    }

    return AbsAddressY;
}

Word CPU::AddrIndY(s32& Cycles, Mem& memory)
{
    Byte ZPAddr = FetchByte(memory);
    ZPAddr += Y;
    Word EffectiveAddr = ReadWord(memory, ZPAddr);
    return EffectiveAddr;
}

void CPU::LoadRegister(Mem& memory, Byte& Register, Word Addr)
{
    Register = ReadByte(Addr, memory);
    UpdateNegativeAndZeroFlags(Register);
}

void CPU::TransferRegister(Byte& Receive, Byte Send)
{
    Receive = Send;
    UpdateNegativeAndZeroFlags(Receive);
}

void CPU::PushByteToStack(Mem& memory, Byte Data)
{
    memory[SPAddr()] = Data;
    SP--;
}

Byte CPU::PullByteFromStack(Mem& memory)
{
    SP++;
    return memory[SPAddr()];
}

void CPU::PushWordToStack(Mem& memory, Word Data)
{
    memory[SPAddr()] = Data;
    SP -= 2;
}

Word CPU::PullWordFromStack(Mem& memory)
{
    Word Data = PullByteFromStack(memory);
    Data |= (PullByteFromStack(memory) << 8);
    return Data;
}

Byte CPU::ROL(Byte Operand)
{
    Byte NewBit0 = Flag.C ? Flag.Z : 0;
    Flag.C = (Operand & Flag.N) > 0;
    Operand = Operand << 1;
    Operand |= NewBit0;
    UpdateNegativeAndZeroFlags(Operand);
    return Operand;
}

Byte CPU::ROR(Byte Operand)
{
    Byte NewBit0 = Flag.C ? Flag.Z : 0;
    Flag.C = (Operand & Flag.N) > 0;
    Operand = Operand >> 1;
    Operand |= NewBit0;
    UpdateNegativeAndZeroFlags(Operand);
    return Operand;
}

void CPU::LSR(Byte& Operand)
{
    Byte NewBit0 = Flag.C ? Flag.Z : 0;
    Flag.C = (Operand & Flag.N) > 0;
    Operand = Operand << 1;
    Operand |= NewBit0;
    UpdateNegativeAndZeroFlags(Operand);
}

void CPU::ADC(Byte Operand)
{
    // Decimal Mode not handled
    const bool AreSignBitsSame = !((A ^ Operand) & Flag.N);
    Word Sum = A;
    Sum += Operand;
    Sum += Flag.C;
    A = (Sum & 0xFF);
    UpdateNegativeAndZeroFlags(A);
    Flag.C = Sum > 0xFF;
    Flag.V = AreSignBitsSame && ((A ^ AreSignBitsSame) & Flag.N);
}

void CPU::SBC(Byte Operand)
{
    ADC(-Operand);
}

void CPU::AND(Byte Operand)
{
    A = A & Operand;
    UpdateNegativeAndZeroFlags(A);
}

void CPU::ASL(Byte& Operand)
{
    Flag.C = 0x80 == (Operand & (1 << 7));

    Operand << 2;

    Flag.Z = (A == 0);
    Flag.N = 0x80 == (Operand & (1 << 7));
}

void CPU::CMP(Byte& Operand)
{
    Flag.C = A >= Operand;
    Flag.Z = A == Operand;
    Flag.N = 0x80 == (Operand & (1 << 7));
}

void CPU::CPX(Byte& Operand)
{
    Flag.C = X >= Operand;
    Flag.Z = X == Operand;
    Flag.N = 0x80 == (Operand & (1 << 7));
}

void CPU::CPY(Byte& Operand)
{
    Flag.C = Y >= Operand;
    Flag.Z = Y == Operand;
    Flag.N = 0x80 == (Operand & (1 << 7));
}

void CPU::DEC(Word Addr, Mem& memory)
{
    memory[Addr]--;
    UpdateNegativeAndZeroFlags(memory[Addr]);
}

void CPU::EOR(Byte Operand)
{
    A = !A != !Operand;
    UpdateNegativeAndZeroFlags(A);
}

void CPU::INC(Word Addr, Mem& memory)
{
    memory[Addr]++;
    UpdateNegativeAndZeroFlags(memory[Addr]);
}

void CPU::ORA(Byte Operand)
{
    A |= Operand;
    UpdateNegativeAndZeroFlags(A);
}

void CPU::Execute(s32 Cycles, Mem& memory)
{
    for (;;)
    {
        Word InsLoc = PC;
        Byte Ins = FetchByte(memory);

        // Handle opcodes
        switch (Ins)
        {
            case INS_LDA_IM:
            {
                A = FetchByte(memory);
                UpdateNegativeAndZeroFlags(A);
                Cycles -= 2;
            } break;
            case INS_LDA_ZP:
            {
                Word ZeroPageAddr = AddrZeroPage(memory);
                LoadRegister(memory, A, ZeroPageAddr);
                Cycles -= 3;
            } break;
            case INS_LDA_ZPX:
            {
                Word ZeroPageAddr = AddrZeroPage(memory);
                ZeroPageAddr += X;
                LoadRegister(memory, A, ZeroPageAddr);
                Cycles -= 4;
            } break;
            case INS_LDA_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                LoadRegister(memory, A, Addr);
                Cycles -= 4;
            } break;
            case INS_LDA_ABSX:
            {
                Word Addr = AddrAbsoluteX(Cycles, memory);
                LoadRegister(memory, A, Addr);
                Cycles -= 4;
            } break;
            case INS_LDA_ABSY:
            {
                Word Addr = AddrAbsoluteY(Cycles, memory);
                LoadRegister(memory, A, Addr);
                Cycles -= 4;
            } break;
            case INS_LDA_INDX:
            {
                Byte ZPAddr = AddrZeroPage(memory);
                ZPAddr += X;
                Word EffectiveAddr = ReadWord(memory, ZPAddr);
                LoadRegister(memory, A, EffectiveAddr);
                Cycles -= 6;
            } break;
            case INS_LDA_INDY:
            {
                Word Addr = AddrIndY(Cycles, memory);
                LoadRegister(memory, A, Addr);
                Cycles -= 5;
            } break;
            case INS_LDX_IM:
            {
                LoadRegister(memory, X, PC);
                Cycles -= 2;
            } break;
            case INS_LDX_ZP:
            {
                Word ZPAddr = AddrZeroPage(memory);
                LoadRegister(memory, X, ZPAddr);
                Cycles -= 3;
            } break;
            case INS_LDX_ZPY:
            {
                Word ZPAddr = AddrZeroPage(memory);
                ZPAddr += Y;
                LoadRegister(memory, X, ZPAddr);
                Cycles -= 4;
            } break;
            case INS_LDX_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                LoadRegister(memory, X, Addr);
                Cycles -= 4;
            } break;
            case INS_LDX_ABSY:
            {
                Word Addr = AddrAbsoluteY(Cycles, memory);
                LoadRegister(memory, X, Addr);
                Cycles -= 4;
            } break;
            case INS_LDY_IM:
            {
                LoadRegister(memory, Y, PC);
                Cycles -= 2;
            } break;
            case INS_LDY_ZP:
            {
                Word Addr = AddrZeroPage(memory);
                LoadRegister(memory, Y, Addr);
                Cycles -= 3;
            } break;
            case INS_LDY_ZPX:
            {
                Word Addr = AddrZeroPage(memory);
                Addr += X;
                LoadRegister(memory, Y, Addr);
                Cycles -= 4;
            } break;
            case INS_LDY_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                LoadRegister(memory, Y, Addr);
                Cycles -= 4;
            } break;
            case INS_LDY_ABSX:
            {
                Word Addr = AddrAbsoluteX(Cycles, memory);
                LoadRegister(memory, Y, Addr);
                Cycles -= 4;
            } break;
            case INS_STA_ZP:
            {
                Word Addr = AddrZeroPage(memory);
                memory[Addr] = A;
                Cycles -= 3;
            } break;
            case INS_STA_ZPX:
            {
                Word Addr = AddrZeroPage(memory);
                Addr += X;
                memory[Addr] = A;
                Cycles -= 4;
            } break;
            case INS_STA_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                memory[Addr] = A;
                Cycles -= 4;
            } break;
            case INS_STA_ABSX:
            {
                Word Addr = AddrAbsoluteX(Cycles, memory);
                Addr += X;
                memory[Addr] = A;
                Cycles -= 5;
            } break;
            case INS_STA_ABSY:
            {
                Word Addr = AddrAbsoluteY(Cycles, memory);
                Addr += Y;
                memory[Addr] = A;
                Cycles -= 5;
            } break;
            case INS_STA_INDX:
            {
                Byte ZPAddr = FetchByte(memory);
                ZPAddr += X;
                Word EffectiveAddr = ReadWord(memory, ZPAddr);
                memory[EffectiveAddr] = A;
                Cycles -= 6;
            } break;
            case INS_STA_INDY:
            {
                Byte ZPAddr = FetchByte(memory);
                ZPAddr += X;
                Word EffectiveAddr = ReadWord(memory, ZPAddr);
                memory[EffectiveAddr] = A;
                Cycles -= 6;
            } break;
            case INS_STX_ZP:
            {
                Word Addr = AddrZeroPage(memory);
                memory[Addr] = X;
                Cycles -= 3;
            } break;
            case INS_STX_ZPY:
            {
                Word Addr = AddrZeroPage(memory);
                Addr += Y;
                memory[Addr] = X;
                Cycles -= 4;
            } break;
            case INS_STX_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                memory[Addr] = X;
                Cycles -= 4;
            } break;
            case INS_STY_ZP:
            {
                Word Addr = AddrZeroPage(memory);
                memory[Addr] = Y;
                Cycles -= 3;
            } break;
            case INS_STY_ZPX:
            {
                Word Addr = AddrZeroPage(memory);
                Addr += X;
                memory[Addr] = Y;
                Cycles -= 4;
            } break;
            case INS_STY_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                memory[Addr] = Y;
                Cycles -= 4;
            } break;
            case INS_TAX:
            {
                TransferRegister(X, A);
                Cycles -= 2;
            } break;
            case INS_TAY:
            {
                TransferRegister(Y, A);
                Cycles -= 2;
            } break;
            case INS_TSX:
            {
                TransferRegister(X, SP);
                Cycles -= 2;
            } break;
            case INS_TXA:
            {
                TransferRegister(A, X);
                Cycles -= 2;
            } break;
            case INS_TXS:
            {
                TransferRegister(SP, X);
                Cycles -= 2;
            } break;
            case INS_TYA:
            {
                TransferRegister(A, Y);
                Cycles -= 2;
            } break;
            case INS_PHA:
            {
                PushByteToStack(memory, A);
                Cycles -= 3;
            } break;
            case INS_PHP:
            {
                PushByteToStack(memory, PS);
                Cycles -= 3;
            } break;
            case INS_PLA:
            {
                A = PullByteFromStack(memory);
                
                UpdateNegativeAndZeroFlags(A);
                Cycles -= 4;
            } break;
            case INS_PLP:
            {
                PS = PullByteFromStack(memory);
                Cycles -= 4;
            } break;
            case INS_ROL_ACC:
            {
                A = ROL(A);
                Cycles -= 2;
            } break;
            case INS_ROL_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                memory[Addr] = ROL(memory[Addr]);
                Cycles -= 5;
            } break;
            case INS_ROL_ZPX:
            {
                Byte Addr = AddrZeroPage(memory);
                Addr += X;
                memory[Addr] = ROL(memory[Addr]);
                Cycles -= 6;
            } break;
            case INS_ROL_ABS:
            {
                Byte Addr = AddrAbsolute(Cycles, memory);
                memory[Addr] = ROL(memory[Addr]);
                Cycles -= 6;
            } break;
            case INS_ROL_ABSX:
            {
                Byte Addr = AddrAbsoluteX(Cycles, memory);
                memory[Addr] = ROL(memory[Addr]);
                Cycles -= 7;
            } break;
            case INS_ROR_ACC:
            {
                A = ROR(A);
                Cycles -= 2;
            } break;
            case INS_ROR_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                memory[Addr] = ROR(memory[Addr]);
                Cycles -= 5;
            } break;
            case INS_ROR_ZPX:
            {
                Byte Addr = AddrZeroPage(memory);
                Addr += X;
                memory[Addr] = ROR(memory[Addr]);
                Cycles -= 5;
            } break;
            case INS_ROR_ABS:
            {
                Byte Addr = AddrAbsolute(Cycles, memory);
                memory[Addr] = ROR(memory[Addr]);
                Cycles -= 6;
            } break;
            case INS_ROR_ABSX:
            {
                Byte Addr = AddrAbsoluteX(Cycles, memory);
                memory[Addr] = ROR(memory[Addr]);
                Cycles -= 7;
            } break;
            case INS_RTI:
            {
                PS = PullByteFromStack(memory);
                Word NewPC = PullByteFromStack(memory);
                NewPC |= (PullByteFromStack(memory) << 8);
                PC = NewPC;
                Cycles -= 6;
            } break;
            case INS_RTS:
            {
                Word NewPC = PullByteFromStack(memory);
                NewPC |= (PullByteFromStack(memory) << 8);
                NewPC++;
                PC = NewPC;
                Cycles -= 6;
            } break;
            case INS_SBC_IM:
            {
                Byte Data = FetchByte(memory);
                SBC(Data);
                Cycles -= 2;
            } break;
            case INS_SBC_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                SBC(memory[Addr]);
                Cycles -= 3;
            } break;
            case INS_SBC_ZPX:
            {
                Byte Addr = AddrZeroPage(memory);
                Addr += X;
                SBC(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_SBC_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                SBC(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_SBC_ABSX:
            {
                Word Addr = AddrAbsoluteX(Cycles, memory);
                SBC(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_SBC_ABSY:
            {
                Word Addr = AddrAbsoluteY(Cycles, memory);
                SBC(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_SBC_INDX:
            {
                Byte ZPAddr = FetchByte(memory);
                ZPAddr += X;
                Word EffectiveAddr = ReadWord(memory, ZPAddr);
                SBC(memory[EffectiveAddr]);
                Cycles -= 6;
            } break;
            case INS_SBC_INDY:
            {
                Byte ZPAddr = FetchByte(memory);
                ZPAddr += X;
                Word EffectiveAddr = ReadWord(memory, ZPAddr);
                SBC(memory[EffectiveAddr]);
                Cycles -= 5;
            } break;
            case INS_ADC_IM:
            {
                Byte Addr = FetchByte(memory);
                ADC(Addr);
                Cycles -= 2;
            } break;
            case INS_ADC_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                ADC(memory[Addr]);
                Cycles -= 3;
            } break;
            case INS_ADC_ZPX:
            {
                Byte Addr = AddrZeroPage(memory);
                Addr += X;
                ADC(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_ADC_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                ADC(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_ADC_ABSX:
            {
                Word Addr = AddrAbsoluteX(Cycles, memory);
                ADC(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_ADC_ABSY:
            {
                Word Addr = AddrAbsoluteY(Cycles, memory);
                ADC(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_ADC_INDX:
            {
                Byte ZPAddr = FetchByte(memory);
                ZPAddr += X;
                Word EffectiveAddr = ReadWord(memory, ZPAddr);
                ADC(memory[EffectiveAddr]);
                Cycles -= 6;
            } break;
            case INS_ADC_INDY:
            {
                Word EffectiveAddr = AddrIndY(Cycles, memory);
                SBC(memory[EffectiveAddr]);
                Cycles -= 5;
            } break;
            case INS_AND_IM:
            {
                Byte Data = FetchByte(memory);
                AND(Data);
                Cycles -= 2;
            } break;
            case INS_AND_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                AND(memory[Addr]);
                Cycles -= 3;
            } break;
            case INS_AND_ZPX:
            {
                Byte Addr = AddrZeroPage(memory);
                Addr += X;
                AND(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_AND_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                AND(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_AND_ABSX:
            {
                Word Addr = AddrAbsoluteX(Cycles, memory);
                AND(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_AND_ABSY:
            {
                Word Addr = AddrAbsoluteY(Cycles, memory);
                AND(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_AND_INDX:
            {
                Byte ZPAddr = FetchByte(memory);
                ZPAddr += X;
                Word EffectiveAddr = ReadWord(memory, ZPAddr);
                AND(memory[EffectiveAddr]);
                Cycles -= 6;
            } break;
            case INS_AND_INDY:
            {
                Word EffectiveAddr = AddrIndY(Cycles, memory);
                ADC(memory[EffectiveAddr]);
                Cycles -= 5;
            } break;
            case INS_ASL_ACC:
            {
                ASL(A);
                Cycles -= 2;
            } break;
            case INS_ASL_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                ASL(memory[Addr]);
                Cycles -= 5;
            } break;
            case INS_ASL_ZPX:
            {
                Byte Addr = AddrZeroPage(memory);
                Addr += X;
                ASL(memory[Addr]);
                Cycles -= 6;
            } break;
            case INS_ASL_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                ASL(memory[Addr]);
                Cycles -= 6;
            } break;
            case INS_ASL_ABSX:
            {
                Word Addr = AddrAbsoluteX(Cycles, memory);
                ASL(memory[Addr]);
                Cycles -= 7;
            } break;
            case INS_BCC:
            {
                if (!Flag.C)
                {
                    Byte Offset = FetchByte(memory);
                    PC += Offset;
                    Cycles--;
                }
                Cycles -= 2;
            } break;
            case INS_BCS:
            {
                if (Flag.C)
                {
                    Byte Offset = FetchByte(memory);
                    PC += Offset;
                    Cycles--;
                }
                Cycles -= 2;
            } break;
            case INS_BEQ:
            {
                if (Flag.Z)
                {
                    Byte Offset = FetchByte(memory);
                    PC += Offset;
                    Cycles--;
                }
                Cycles -= 2;
            } break;
            case INS_BMI:
            {
                if (Flag.N)
                {
                    Byte Offset = FetchByte(memory);
                    PC += Offset;
                    Cycles--;
                }
                Cycles -= 2;
            } break;
            case INS_BNE:
            {
                if (!Flag.Z)
                {
                    Byte Offset = FetchByte(memory);
                    PC += Offset;
                    Cycles--;
                }
                Cycles -= 2;
            } break;
            case INS_BPL:
            {
                if (!Flag.N)
                {
                    Byte Offset = FetchByte(memory);
                    PC += Offset;
                    Cycles--;
                }
                Cycles -= 2;
            } break;
            case INS_BVC:
            {
                if (!Flag.V)
                {
                    Byte Offset = FetchByte(memory);
                    PC += Offset;
                    Cycles--;
                }
                Cycles -= 2;
            } break;
            case INS_BVS:
            {
                if (Flag.V)
                {
                    Byte Offset = FetchByte(memory);
                    PC += Offset;
                    Cycles--;
                }
                Cycles -= 2;
            } break;
            case INS_BRK:
            {
                PushWordToStack(memory, PC);
                PushByteToStack(memory, PS);
                ReadWord(memory, 0xFFFE);
                Flag.B = true;
                Cycles -= 7;
            } break;
            case INS_BIT_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                Byte Data = A & memory[Addr];
                Flag.Z = (Data == 0);
                Flag.V = (Data & (1 << 6));
                Flag.N = (Data & (1 << 7));
                Cycles -= 3;
            } break;
            case INS_BIT_ABS:
            {
                Byte Addr = AddrAbsolute(Cycles, memory);
                Byte Data = A & memory[Addr];
                Flag.Z = (Data == 0);
                Flag.V = (Data & (1 << 6));
                Flag.N = (Data & (1 << 7));
                Cycles -= 4;
            } break;
            case INS_SEC:
            {
                Flag.C = true;
                Cycles -= 2;
            } break;
            case INS_SED:
            {
                Flag.D = true;
                Cycles -= 2;
            } break;
            case INS_SEI:
            {
                Flag.I = true;
                Cycles -= 2;
            } break;
            case INS_CLC:
            {
                Flag.C = false;
                Cycles -= 2;
            } break;
            case INS_CLD:
            {
                Flag.D = false;
                Cycles -= 2;
            } break;
            case INS_CLI:
            {
                Flag.I = false;
                Cycles -= 2;
            } break;
            case INS_CLV:
            {
                Flag.V = false;
                Cycles -= 2;
            } break;
            case INS_CMP_IM:
            {
                Byte Data = FetchByte(memory);
                CMP(Data);
                Cycles -= 2;
            } break;
            case INS_CMP_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                CMP(memory[Addr]);
                Cycles -= 3;
            } break;
            case INS_CMP_ZPX:
            {
                Byte Addr = AddrZeroPage(memory);
                Addr += X;
                CMP(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_CMP_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                CMP(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_CMP_ABSX:
            {
                Word Addr = AddrAbsoluteX(Cycles, memory);
                CMP(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_CMP_ABSY:
            {
                Word Addr = AddrAbsoluteY(Cycles, memory);
                CMP(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_CMP_INDX:
            {
                Byte ZPAddr = FetchByte(memory);
                ZPAddr += X;
                Word EffectiveAddr = ReadWord(memory, ZPAddr);
                CMP(memory[EffectiveAddr]);
                Cycles -= 6;
            } break;
            case INS_CMP_INDY:
            {
                Word EffectiveAddr = AddrIndY(Cycles, memory);
                CMP(memory[EffectiveAddr]);
                Cycles -= 5;
            } break;
            case INS_CPX_IM:
            {
                Byte Data = FetchByte(memory);
                CPX(Data);
                Cycles -= 2;
            } break;
            case INS_CPX_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                CPX(memory[Addr]);
                Cycles -= 3;
            } break;
            case INS_CPX_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                CPX(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_CPY_IM:
            {
                Byte Data = FetchByte(memory);
                CPY(Data);
                Cycles -= 2;
            } break;
            case INS_CPY_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                CPY(memory[Addr]);
                Cycles -= 3;
            } break;
            case INS_CPY_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                CPY(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_DEC_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                DEC(Addr, memory);
                Cycles -= 5;
            } break;
            case INS_DEC_ZPX:
            {
                Byte Addr = AddrZeroPage(memory);
                Addr += X;
                DEC(Addr, memory);
                Cycles -= 6;
            } break;
            case INS_DEC_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                DEC(Addr, memory);
                Cycles -= 6;
            } break;
            case INS_DEC_ABSX:
            {
                Word Addr = AddrAbsoluteX(Cycles, memory);
                DEC(Addr, memory);
                Cycles -= 7;
            } break;
            case INS_DEX:
            {
                X--;
                Flag.Z = X == 0;
                Flag.N = 0x80 == (X & (1 << 7));
                Cycles -= 2;
            } break;
            case INS_DEY:
            {
                Y--;
                Flag.Z = Y == 0;
                Flag.N = 0x80 == (X & (1 << 7));
                Cycles -= 2;
            } break;
            case INS_EOR_IM:
            {
                Byte Data = FetchByte(memory);
                EOR(Data);
                Cycles -= 2;
            } break;
            case INS_EOR_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                EOR(memory[Addr]);
                Cycles -= 3;
            } break;
            case INS_EOR_ZPX:
            {
                Byte Addr = AddrZeroPage(memory);
                Addr += X;
                EOR(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_EOR_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                EOR(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_EOR_ABSX:
            {
                Word Addr = AddrAbsoluteX(Cycles, memory);
                EOR(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_EOR_INDX:
            {
                Byte ZPAddr = FetchByte(memory);
                ZPAddr += X;
                Word EffectiveAddr = ReadWord(memory, ZPAddr);
                EOR(memory[EffectiveAddr]);
                Cycles -= 6;
            } break;
            case INS_EOR_INDY:
            {
                Word EffectiveAddr = AddrIndY(Cycles, memory);
                CMP(memory[EffectiveAddr]);
                Cycles -= 5;
            } break;
            case INS_INC_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                INC(Addr, memory);
                Cycles -= 5;
            } break;
            case INS_INC_ZPX:
            {
                Byte Addr = AddrZeroPage(memory);
                Addr += X;
                INC(Addr, memory);
                Cycles -= 6;
            } break;
            case INS_INC_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                INC(Addr, memory);
                Cycles -= 6;
            } break;
            case INS_INC_ABSX:
            {
                Word Addr = AddrAbsoluteX(Cycles, memory);
                INC(Addr, memory);
                Cycles -= 7;
            } break;
            case INS_INX:
            {
                X++;
                UpdateNegativeAndZeroFlags(X);
                Cycles -= 2;
            } break;
            case INS_INY:
            {
                Y++;
                UpdateNegativeAndZeroFlags(Y);
                Cycles -= 2;
            } break;
            case INS_JMP_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                PC = ReadWord(memory, Addr);
                Cycles -= 3;
            } break;
            case INS_JMP_IND:
            {
                Word Addr = FetchWord(memory);
                Addr = ReadWord(memory, Addr);
                Word Data = ReadWord(memory, Addr);
                PC = Data;
                Cycles -= 5;
            } break;
            case INS_JSR:
            {
                PushWordToStack(memory, PC--);
                PC = FetchWord(memory);
                Cycles -= 6;
            } break;
            case INS_NOP:
            {
                PC++;
                Cycles -= 2;
            } break;
            case INS_ORA_IM:
            {
                Byte Data = FetchByte(memory);
                ORA(Data);
                Cycles -= 2;
            } break;
            case INS_ORA_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                ORA(memory[Addr]);
                Cycles -= 3;
            } break;
            case INS_ORA_ZPX:
            {
                Byte Addr = AddrZeroPage(memory);
                Addr += X;
                ORA(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_ORA_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                ORA(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_ORA_ABSX:
            {
                Word Addr = AddrAbsoluteX(Cycles, memory);
                ORA(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_ORA_ABSY:
            {
                Word Addr = AddrAbsoluteY(Cycles, memory);
                ORA(memory[Addr]);
                Cycles -= 4;
            } break;
            case INS_ORA_INDX:
            {
                Byte ZPAddr = FetchByte(memory);
                ZPAddr += X;
                Word EffectiveAddr = ReadWord(memory, ZPAddr);
                ORA(memory[EffectiveAddr]);
                Cycles -= 6;
            } break;
            case INS_ORA_INDY:
            {
                Byte ZPAddr = FetchByte(memory);
                ZPAddr += Y;
                Word EffectiveAddr = ReadWord(memory, ZPAddr);
                ORA(memory[EffectiveAddr]);
                Cycles -= 5;
            } break;
            case INS_LSR_ACC:
            {
                LSR(A);
                Cycles -= 2;
            } break;
            case INS_LSR_ZP:
            {
                Byte Addr = AddrZeroPage(memory);
                LSR(memory[Addr]);
                Cycles -= 5;
            } break;
            case INS_LSR_ZPX:
            {
                Byte Addr = AddrZeroPage(memory);
                Addr += X;
                LSR(memory[Addr]);
                Cycles -= 6;
            } break;
            case INS_LSR_ABS:
            {
                Word Addr = AddrAbsolute(Cycles, memory);
                LSR(memory[Addr]);
                Cycles -= 6;
            } break;
            case INS_LSR_ABSX:
            {
                Word Addr = AddrAbsoluteX(Cycles, memory);
                LSR(memory[Addr]);
                Cycles -= 7;
            } break;
            default:
            {
                printf("Opcode (%X) In Memory Location (%X) Not Recognized\n", Ins, InsLoc);
                throw -1;
            } break;
        }

        // Handles Interrupts and other cyclic tasks
        if (Cycles <= 0)
        {
            if (!Flag.I) // Checks if interrupt disable flag is set, doesn't handle interrupt if so
            {

            }

            // Adds to cycles and starts from top of loop if not needing to exit
            Cycles += InterruptPeriod;
            if (Exit) break;
        }
    }
}

int main()
{
    /*
    Testing testing;
    testing.Init();
    testing.Test();
    */
}