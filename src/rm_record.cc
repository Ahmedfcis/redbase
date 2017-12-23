#include <cstdio>
#include "rm.h"
#include "rm_internal.h"

RM_Record::RM_Record (){

}

RM_Record::~RM_Record(){

}

    // Return the data corresponding to the record.  Sets *pData to the
    // record contents.
RC RM_Record::GetData(char *&pData) const{
    return END_RM_ERR;
}

    // Return the RID associated with the record
RC RM_Record::GetRid (RID &rid) const{
    return END_RM_ERR;
}