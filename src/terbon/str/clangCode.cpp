#include <fstream>
#include <string>
#include <iterator>
#include <iostream>

#include "str/clangCode.h"

namespace terbon {

void clangCpp(std::string path)
{
    std::ifstream file(path);

    std::string code(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );
    
    size_t i = 0;

    while (i < code.size())
    {
        if (code[i] == ',')
        {
            while (code[i - 1] == ' ')
            {
                code.erase(i - 1 , 1);
            }

            while (code[i + 1] == ' ')
            {
                code.erase(i + 1 , 1);
            }

            code.replace(i , 1 , " " + std::string(1 , code[i]) + " ");
            i ++;
        }

        if (code[i] == '+')
        {
            if (code[i + 1] == '+')
            {
                if (code[i - 1] == ' ')
                {
                    code.insert(i + 2 , " ");
                }
                else if (code[i + 1] == '+')
                {
                    code.insert(i , " ");
                }

                i ++;
            }
        }

        i ++;
    }
}

void clangPy(std::string path)
{
    std::ifstream file(path);

    std::string code(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    size_t i = 0;

    while (i < code.size())
    {
        if (code[i] == ',')
        {
            while (code[i - 1] == ' ')
            {
                code.erase(i - 1 , 1);
            }

            while (code[i + 1] == ' ')
            {
                code.erase(i + 1 , 1);
            }

            code.insert(i , " ");
            code.insert(i + 2 , " ");
            i ++;
        }
        // else if (code[i] == '=')
        // {
        //     while (code[i - 1] == ' ')
        //     {
        //         code.erase(i - 1 , 1);
        //     }

        //     while (code[i + 1] == ' ')
        //     {
        //         code.erase(i + 1 , 1);
        //     }

        //     code.insert(i , " ");
        //     code.insert(i + 2 , " ");
        //     i ++;
        // }

        i ++;
    }

    std::ofstream output(path);

    output << code;

    output.close();
}

}
