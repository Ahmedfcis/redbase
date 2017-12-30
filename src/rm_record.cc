#include <cstdio>
#include "rm.h"
#include "rm_internal.h"

RM_Record::RM_Record (){
    pData = nullptr;
}

RM_Record::~RM_Record(){
    if(pData)
        delete[] pData;
}

RM_Record::RM_Record(RM_Record&& r){
    this->pData = r.pData;
    r.pData = nullptr;

    this->rid = r.rid;
    this->size = r.size;
}

RM_Record& RM_Record::operator = (RM_Record&& r){
    if(this->pData)
        delete[] pData;

    this->pData = r.pData;
    r.pData = nullptr;

    this->rid = r.rid;
    this->size = r.size;

    return *this;
}
    // Return the data corresponding to the record.  Sets *pData to the
    // record contents.
RC RM_Record::GetData(char *&pData) const{
    if(!this->pData)
        return RM_INVALID_RECORD;
    pData = this->pData;
    return OK_RC;
}

    // Return the RID associated with the record
RC RM_Record::GetRid (RID &rid) const{
    if(!pData)
        return RM_INVALID_RECORD;
    rid = this->rid;
    return OK_RC;
}

RC  RM_Record::SetData(const RID& rid,char* pData,int size){
    if(this->pData)
        delete[] this->pData;

    this->pData = pData;
    this->rid   = rid;
    this->size  = size;
    return OK_RC;
}