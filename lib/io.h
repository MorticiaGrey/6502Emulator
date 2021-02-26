#include "../lib/main.h"

struct GMem // Graphics mem
{
    static constexpr u32 MAX_MEM = 1024 * 16;
    Byte Data[MAX_MEM] = {};

    void Initialize()
    {
        for (u32 i = 0; i < MAX_MEM; i++)
        {
            Data[i] = 0;
        }
    }

    Byte operator[](u32 Address) const
    {
        return Data[Address];
    }

    Byte& operator[](u32 Address)
    {
        return Data[Address];
    }
};

// Modeled in part after the VIC-II chip, though not meant to truly emulate graphics
struct GraphicsModule
{
    // Where the graphics mode bits are stored
    // 0 = Default character sheet (predefined), other is address to custom character sheet
    Word GModeAddr = 0;

    // The range of instructions for the graphics sim
    int Memstart, Memend;

    Word PC; // internal Program counter

    void Init(Word GraphicsModeAddr, GMem& gmemory);

    void tick(Mem memory);
};