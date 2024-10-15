#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;

struct Memory
{
public:
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    // Processes

    void Initialize()
    {
        for (u32 i = 0; i < MAX_MEM; i++)
        {
            Data[i] = 0;
        }
    }

    // Operations

    // Read 1 byte
    Byte operator[](u32 _address) const
    {
        if (_address >= MAX_MEM)
        {
            throw std::out_of_range("Bad memory access");
        }
        return Data[_address];
    }

    // Write 1 byte
    Byte &operator[](u32 _address)
    {
        if (_address >= MAX_MEM)
        {
            throw std::out_of_range("Bad memory access");
        }
        return Data[_address];
    }

    // Write 2 bytes
    void WriteWord(u32 &_cycles, Word _data, u32 _address)
    {
        Data[_address] = _data & 0xFF;
        Data[_address + 1] = (_data >> 8);
        _cycles -= 2;
    }

    Word ReadWord(u32 &_cycles, u32 _address)
    {
    }
};

struct CPU
{
    Word PC; // Program counter
    Word SP; // Stack pointer

    Byte A; // Register A, accumulator
    Byte X; // Register X
    Byte Y; // Register Y

    // Processor Status
    Byte C : 1; // Carry flag bit
    Byte Z : 1; // Zero flag bit
    Byte I : 1; // Interrupt Disable flag bit
    Byte D : 1; // Decimal mode flag bit
    Byte B : 1; // Break command flag bit
    Byte V : 1; // Overflow flag bit
    Byte N : 1; // Negative flag bit

    // Processes
    void Reset(Memory &_memory)
    {
        PC = 0xFFFC;                   // Set program counter
        SP = 0x0100;                   // Set Stack pointer
        C = Z = I = D = B = V = N = 0; // Reset all flags
        A = X = Y = 0;                 // Reset all registers

        _memory.Initialize();
    }

    Byte FetchByte(u32 &_cycles, Byte _address, Memory &_memory)
    {
        Byte data = _memory[_address];
        _cycles--;
        return data;
    }

    Word FetchWord(u32 &_cycles, Word _address, Memory &_memory)
    {
        Word data = _memory[_address];
        data |= (_memory[_address + 1] << 8);
        _cycles -= 2;
        return data;
    }

    Byte ReadByte(u32 &_cycles, Memory &_memory)
    {
        Byte data = _memory[PC];
        PC++;
        _cycles--;
        return data;
    }

    Word ReadWord(u32 &_cycles, Memory &_memory)
    {
        Word data = _memory[PC];
        PC++;
        data |= (_memory[PC] << 8);
        _cycles -= 2;
        return data;
    }

    void LDASetStatus()
    {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;
    }

    static constexpr Byte INS_LDA_IM = 0xA9;
    static constexpr Byte INS_LDA_ZP = 0xA5;
    static constexpr Byte INS_LDA_ZPX = 0xB5;
    static constexpr Byte INS_LDA_ABS = 0xAD;
    static constexpr Byte INS_JSR = 0x20;

    void Execute(u32 _cycles, Memory &_memory)
    {
        while (_cycles > 0)
        {
            Byte instruction = ReadByte(_cycles, _memory);
            switch (instruction)
            {
            case INS_LDA_IM:
            {
                Byte value = ReadByte(_cycles, _memory);
                A = value;
                LDASetStatus();
            }
            break;
            case INS_LDA_ZP:
            {
                Byte zeroPageAdress = ReadByte(_cycles, _memory);
                A = FetchByte(_cycles, zeroPageAdress, _memory);
                LDASetStatus();
            }
            break;
            case INS_LDA_ZPX:
            {
                Byte zeroPageAdress = ReadByte(_cycles, _memory);
                zeroPageAdress += X;
                zeroPageAdress %= 0xFF;
                A = FetchByte(_cycles, zeroPageAdress, _memory);
                LDASetStatus();
            }
            break;
            case INS_LDA_ABS:
            {
                Word zeroPageAdress = ReadWord(_cycles, _memory);
                A = FetchWord(_cycles, zeroPageAdress, _memory);
                LDASetStatus();
            }
            break;
            case INS_JSR:
            {
                Word jmpToAddress = ReadWord(_cycles, _memory);
                _memory.WriteWord(_cycles, PC, SP);
                PC = jmpToAddress;
                _cycles -= 2;
            }
            break;

            default:
            {
                printf("Instruction not handled %d", instruction);
                return;
            }
            break;
            }
        }
    }
};

int main()
{
    CPU cpu;
    Memory mem;
    cpu.Reset(mem);
    // inline program - load accumulator with 69
    mem[0xFFFC] = CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 69;
    // inline program - load accumulator with 69
    cpu.Execute(3, mem);
    return 0;
}