#include <string.h>
#include <stdio.h>

#include "ExceptionHandler.h"
#include "TestResult.h"
#include "Failure.h"
#include "TestException.h"



namespace UnitExceptionHandler {


namespace {
    bool g_bHandleExceptions = true;
}


bool IsOn ()
{
    return g_bHandleExceptions;
}

void TurnOn (bool bOn)
{
    g_bHandleExceptions = bOn;
}


void Handle (TestResult& result, const TestException& exception, 
             const char* testname, const char* filename, int linenumber )
{
    char msg[4096];
    sprintf( msg, "Raised exception %s from:\n  %s(%i)", exception.message, exception.file, exception.line );
    result.AddFailure (Failure (msg, testname, filename, linenumber));
}

void Handle (TestResult& result, const char* condition, 
             const char* testname, const char* filename, int linenumber)
{
    if (!g_bHandleExceptions) 
        throw;
        
    char msg[1024] = "Unhandled exception ";
    strcat(msg, condition);
    result.AddFailure (Failure (msg, testname, filename, linenumber));
}



}
