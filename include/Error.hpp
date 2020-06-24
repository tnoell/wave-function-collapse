#ifndef ERROR_H
#define ERROR_H

#include <string>

struct Error
{
    enum Code {
        contradiction = 0,
        noElement
    };
    Code code;
    std::string message;
};

const static Error errors[] = {
    {Error::Code::contradiction, "A contradiction was encountered"},
    {Error::Code::noElement, "No element found."}
    };

#endif // ERROR_H