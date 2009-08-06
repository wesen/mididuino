#include "Failure.h"

#include <string>
#include <iostream>


Failure::Failure (const char* condition, const char* testName, 
                  const char* fileName, int lineNumber) 
    : m_condition (condition)
    , m_testName (testName)
    , m_fileName (fileName)
    , m_lineNumber (lineNumber)
{
}

const char* Failure::Condition() const
{
    return m_condition;
}


std::ostream& operator<< (std::ostream& stream, const Failure& failure)
{
    stream 
        << failure.m_fileName
        << "(" << failure.m_lineNumber << "): "
        << "Failure in " << failure.m_testName << ": \"" << failure.m_condition << "\" " 
        << std::endl;

    return stream;
}

