#include "../lib/io.h"

void GraphicsModule::Init(Word GraphicsModeAddr, GMem& gmemory)
{
    GModeAddr = GraphicsModeAddr;
    gmemory.Initialize();
}

void GraphicsModule::tick(Mem memory)
{
    Byte GMode = memory[GModeAddr];

    switch (GMode)
    {
        case 0:
        {
            printf("%c", memory[PC]);
        } break;
    };
}