#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "iostream"
#include "redbase.h"
#include "pf.h"
#include "rm.h"

bool check_rc(RC rc){
    if(rc == OK_RC)
        return true;

    if (abs(rc) <= END_PF_WARN)
        PF_PrintError(rc);
    else if (abs(rc) <= END_RM_WARN)
        RM_PrintError(rc);
    else
        std::cerr << "Error code out of range: " << rc << "\n";

    return false;
}