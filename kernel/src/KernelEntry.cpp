#include "include/types.h"

#include "include/io.h"

#include "include/console.h"
#include "include/memory.h"


extern "C"
{


class Test
{
public:
    Test()
    {
        PrintString("Class: Test\n");
    }

    virtual ~Test() {}
};

class InheritedTest : public Test
{
public:
    InheritedTest()
    {
        PrintString("Class: InheritedTest\n");
    }

    virtual ~InheritedTest() {}
};


int _kernel_entry()
{
    InitConsole((unsigned short*)0xB8000, ConsoleColor::Blue, ConsoleColor::LightGray);

    ClearScreen();
    PrintString("RexxKaos operating system.\nSupported features: NONE\n", 0x10);

    GetMemoryInformation();

    PrintString("5 * 7 + 2 = %d\n", 5 * 7 + 2);

    PrintString("Console address: %x\n", 0x80085deadbeef717); // 0xB8000);

    Test testClass;
    InheritedTest inheritedTestClass;

    return 0;
}




}
