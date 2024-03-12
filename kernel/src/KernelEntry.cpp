


extern "C"
{


void ClearScreen()
{
    unsigned short* videoMemory = (unsigned short*)0xB8000;

    for (unsigned int y = 0; y < 25; y++)
    {
        for (unsigned int x = 0; x < 80; x++)
        {
            videoMemory[y * 80 + x] = 0x209F;
        }
    }
}

int _kernel_entry()
{
    ClearScreen();

    return 0;
}


}
