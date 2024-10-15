#include "6502.cpp"

int main()
{
    CPU cpu;
    Memory mem;
    cpu.Reset(mem);
    // inline program - load accumulator with 69
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = -69;
    // mem[0x0042] = 69;
    // inline program - load accumulator with 69

    // inline program - jmp test
    // mem[0xFFFC] = CPU::INS_JMP_IND;
    // mem[0xFFFD] = 0x42;
    // mem[0x0042] = 0x69;
    // mem[0x0069] = CPU::INS_LDA_ABS;
    // mem[0x0070] = 0x66;
    // mem[0x0066] = 69;

    cpu.Execute(10, mem);
    return 0;
}