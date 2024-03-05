#include <iostream>

#include <cstdint>
#include <string>


struct ProgramOptions
{
    std::string ImageName;
};


static ProgramOptions s_Options;


int main(int argc, char** argv)
{
    std::cout << "OSDev debugger\n";

    for (int i = 0; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
                case 'i':
                {
                    if (++i < argc)
                    {
                        s_Options.ImageName = argv[i];
                    }

                    break;
                }
            }
        }
    }


    if (s_Options.ImageName.empty())
    {
        std::cout << "No file image to load.\n";
        return 0;
    }

    


    return 0;
}
