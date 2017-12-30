#include <cstdio>
#include <memory>
#include <cstring>
#include "rm.h"
#include "rm_internal.h"

#define RM_SCAN_FLOAT_TOLERANCE     0.000001

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

    if((attrType == AttrType::INT || attrType == AttrType::FLOAT) && attrLength != 4)
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

    RM_Record tmp;
    while(rc = fileHandle->GetNext(current)){
        if(rc = fileHandle->GetRec(current,tmp))
            return rc;
        if(CheckRecord(tmp)){
            rec = std::move(tmp);
            return OK_RC;
        }
    }
}

bool RM_FileScan::CheckRecord(RM_Record& r){
    RC rc;
    char* pData;
    if(rc = r.GetData(pData))
        return false;

    pData += attrOffset;

    int cmpV;
    switch(attrType){
        case INT:
        case STRING:
            cmpV = memcmp(value,pData,attrLength);
        case FLOAT:
        {
            float fV = (*(float*)value);
            float fD = (*(float*)pData);
            cmpV = (fV - fD) / RM_SCAN_FLOAT_TOLERANCE;
        }
        default:
            return false;
    }

    switch(compOp){
        case EQ_OP:
            return cmpV == 0;
        case NE_OP:
            return cmpV != 0;
        case LT_OP:
            return cmpV < 0;
        case GT_OP:
            return cmpV > 0;
        case LE_OP:
            return cmpV <= 0;
        case GE_OP:
            return cmpV >= 0;
        default:
            return false;
    }
}

RC RM_FileScan::CloseScan (){                             // Close the scan
    if(value)
        delete[] value;
    value == nullptr;
    return OK_RC;
}