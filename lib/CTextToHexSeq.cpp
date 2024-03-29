#include <iostream>
#include <sstream>
#include <iomanip>

#include <CTextToHexSeq.h>

std::string CTextToHexSeq::convert(std::string &inputString, const int &varLine, const std::string &inputFile, const std::string &nl)

{

    std::stringstream stream;
    stream << std::hex << std::setfill('0');
    unsigned int charPos = 0;

    checkNewLine(inputString, nl);

    for (unsigned char c : inputString)
    {
        checkASCII(c, varLine, charPos, inputFile);

        stream << "\\x" << std::setw(2) << static_cast<int>(c);
        charPos++;
    }
    return stream.str();
}

// constructor to initialize an instance of the CTextToEscSeq class
CTextToHexSeq::CTextToHexSeq(const VariableStruct &variable, const ParamStruct &parameter) : CTextToCPP(variable, parameter)
{
    // constructor implementation
}

// destructor for the CTextToHexSeq class
CTextToHexSeq::~CTextToHexSeq()
{
    // destructor implementation
}
