#include <cstdio>
#include "rm.h"
#include "rm_internal.h"

RM_FileHandle::RM_FileHandle (){
    pFileHeader = new RM_FILE_HEADER();
}

RM_FileHandle::~RM_FileHandle(){
    delete pFileHeader;
}

    // Given a RID, return the record
RC RM_FileHandle::GetRec     (const RID &rid, RM_Record &rec) const{
    return -1;
}

RC RM_FileHandle::InsertRec  (const char *pData, RID &rid){       // Insert a new record
    return END_RM_ERR;
}

RC RM_FileHandle::DeleteRec  (const RID &rid){                    // Delete a record
    return END_RM_ERR;
}

RC RM_FileHandle::UpdateRec  (const RM_Record &rec){              // Update a record
    return END_RM_ERR;
}
    // Forces a page (along with any contents stored in this class)
    // from the buffer pool to disk.  Default value forces all pages.
RC RM_FileHandle::ForcePages (PageNum pageNum){
    return END_RM_ERR;
}
