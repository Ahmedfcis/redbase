#include <cstdio>
#include "rm.h"
#include "rm_internal.h"

RM_FileScan::RM_FileScan  (){

}

RM_FileScan::~RM_FileScan (){

}

RC RM_FileScan::OpenScan  ( const RM_FileHandle &fileHandle,
                            AttrType   attrType,
                            int        attrLength,
                            int        attrOffset,
                            CompOp     compOp,
                            void       *value,
                            ClientHint pinHint){ // Initialize a file scan
    return END_RM_ERR;
}

RC RM_FileScan::GetNextRec(RM_Record &rec){               // Get next matching record
    return END_RM_ERR;
}

RC RM_FileScan::CloseScan (){                             // Close the scan
    return END_RM_ERR;
}