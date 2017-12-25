#include <cstdio>
#include <cstring>
#include "rm.h"
#include "rm_internal.h"

RM_FileScan::RM_FileScan  (){
    value = nullptr;
}

RM_FileScan::~RM_FileScan (){
    if(value)
        delete[] value;
}

RC RM_FileScan::OpenScan  ( const RM_FileHandle &fileHandle,
                            AttrType   attrType,
                            int        attrLength,
                            int        attrOffset,
                            CompOp     compOp,
                            void       *value,
                            ClientHint pinHint){ // Initialize a file scan

    if(this->value)
        return RM_INVALID_SCAN;

    if( fileHandle.pFileHeader->fileType != RM_FILE_TYPE ||
        attrOffset + attrLength > fileHandle.pFileHeader->recordSize)
            return RM_INVALID_SCAN;

    this->fileHandle = &fileHandle;
    this->attrType = attrType;
    this->attrLength = attrLength;
    this->attrOffset = attrOffset;
    this->compOp = compOp;
    this->value = new char[attrLength];
    std::memcpy(this->value,value,attrLength);

    current = RID();
    return OK_RC;
}

RC RM_FileScan::GetNextRec(RM_Record &rec){               // Get next matching record
    if(!this->value)
        return RM_INVALID_SCAN;
    RC rc;
    if(rc = fileHandle->GetNext(current))
        return rc;
    return fileHandle->GetRec(current,rec);
}

RC RM_FileScan::CloseScan (){                             // Close the scan
    if(value)
        delete[] value;
    value == nullptr;
    return OK_RC;
}