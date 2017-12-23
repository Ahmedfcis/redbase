#include <cstdio>
#include <cstring>
#include "rm.h"
#include "rm_internal.h"

RM_FileHandle::RM_FileHandle (){
    pFileHeader = new RM_FILE_HEADER();
    std::memset(pFileHeader,0,sizeof(RM_FILE_HEADER));
}

RM_FileHandle::~RM_FileHandle(){
    delete pFileHeader;
}

    // Given a RID, return the record
RC RM_FileHandle::GetRec     (const RID &rid, RM_Record &rec) const{
    RC rc;
    PageNum pageNum;
    SlotNum slotNum;
    if(rc = rid.GetPageNum(pageNum))
        return rc;

    if(rc = rid.GetSlotNum(slotNum))
        return rc;

    PF_PageHandle page;
    if(rc = pfFileHandle.GetThisPage(pageNum,page))
        return rc;

    char* pPageData;
    if(rc = page.GetData(pPageData)){
        pfFileHandle.UnpinPage(pageNum);
        return rc;
    }

    RM_PAGE_HEADER* pPageHd;
    pPageHd = (RM_PAGE_HEADER*)pPageData;

    if((pPageHd->flags[slotNum/8] & 1 << (slotNum%8)) == 0){ // if the slot not allocated
        pfFileHandle.UnpinPage(pageNum);
        return RM_INVALID_RECORD;
    }

    //Copy record data
    char* pRecordOffset = pPageData + pPageHd->headerSize + pFileHeader->recordSize * slotNum;
    char* pRecord = new char[pFileHeader->recordSize];
    std::memcpy(pRecord,pRecordOffset,pFileHeader->recordSize);

    rec.SetData(rid, pRecord, pFileHeader->recordSize);

    pfFileHandle.UnpinPage(pageNum);
    return OK_RC;
}

RC RM_FileHandle::InsertRec  (const char *pData, RID &rid){       // Insert a new record

    PageNum pageNum;
    SlotNum slotNum;
    RC rc;
    if(rc = GetFirstFreeSlot(pageNum,slotNum))
        return rc;

    if(rc = UpdateRec(pData,pageNum, slotNum))
        return rc;

    UpdateFreePageList(pageNum);

    rid = RID(pageNum,slotNum);
    return OK_RC;
}

RC RM_FileHandle::DeleteRec  (const RID &rid){                    // Delete a record
    RC rc;
    PageNum pageNum;
    SlotNum slotNum;
    if(rc = rid.GetPageNum(pageNum))
        return rc;

    if(rc = rid.GetSlotNum(slotNum))
        return rc;

    PF_PageHandle page;
    if(rc = pfFileHandle.GetThisPage(pageNum,page))
        return rc;

    char* pPageData;
    if(rc = page.GetData(pPageData)){
        pfFileHandle.UnpinPage(pageNum);
        return rc;
    }

    RM_PAGE_HEADER* pPageHd;
    pPageHd = (RM_PAGE_HEADER*)pPageData;
    pPageHd->flags[slotNum/8] &= ~ (1 << (slotNum%8)); // set 0 at flag to mark deleted page

    UpdateFreePageList(pageNum);

    pfFileHandle.MarkDirty(pageNum);
    pfFileHandle.UnpinPage(pageNum);
    return OK_RC;
}

RC RM_FileHandle::UpdateRec  (const RM_Record &rec){              // Update a record
    RC rc;
    PageNum pageNum;
    SlotNum slotNum;
    if(rc = rec.rid.GetPageNum(pageNum))
        return rc;

    if(rc = rec.rid.GetSlotNum(slotNum))
        return rc;

    return UpdateRec(rec.pData,pageNum,slotNum);
}
RC RM_FileHandle::UpdateRec  (const char* pData,PageNum pageNum, SlotNum slotNum){
    RC rc;
    PF_PageHandle page;
    if(rc = pfFileHandle.GetThisPage(pageNum,page))
        return rc;

    char* pPageData;
    if(rc = page.GetData(pPageData)){
        pfFileHandle.UnpinPage(pageNum);
        return rc;
    }

   RM_PAGE_HEADER* pPageHd;
    pPageHd = (RM_PAGE_HEADER*)pPageData;

    pPageHd->flags[slotNum/8] |= (1 << (slotNum%8));

    //Copy record data
    char* pRecordOffset = pPageData + pPageHd->headerSize + pFileHeader->recordSize * slotNum;
    std::memcpy(pRecordOffset,pData,pFileHeader->recordSize);

    pfFileHandle.MarkDirty(pageNum);
    pfFileHandle.UnpinPage(pageNum);
    return OK_RC;
}
    // Forces a page (along with any contents stored in this class)
    // from the buffer pool to disk.  Default value forces all pages.
RC RM_FileHandle::ForcePages (PageNum pageNum){
    return pfFileHandle.ForcePages(pageNum);
}

RC RM_FileHandle::GetFirstFreeSlot(PageNum& pageNum,SlotNum& slotNum){
    RC rc;
    PF_PageHandle page;
    if(pFileHeader->firstFreePage == BAD_PAGE_NUM){
        pfFileHandle.AllocatePage(page);

        char* pPageData;
        if(rc = page.GetData(pPageData)){
            pfFileHandle.UnpinPage(pageNum);
            return rc;
        }

        RM_PAGE_HEADER* pPageHd;
        pPageHd = (RM_PAGE_HEADER*)pPageData;
        int flagsSize = (pFileHeader->numOfRecordPerPage+7) / 8;
        pPageHd->headerSize = sizeof(RM_PAGE_HEADER) + flagsSize;
        std::memset(pPageHd->flags,0,flagsSize);

        page.GetPageNum(pageNum);
        pfFileHandle.MarkDirty(pageNum);
    }
    else{
        if(rc = pfFileHandle.GetThisPage(pFileHeader->firstFreePage,page))
            return rc;
        pageNum = pFileHeader->firstFreePage;
    }

    //Get First free slot
    char* pPageData;
    if(rc = page.GetData(pPageData)){
        pfFileHandle.UnpinPage(pageNum);
        return rc;
    }

    RM_PAGE_HEADER* pPageHd;
    pPageHd = (RM_PAGE_HEADER*)pPageData;

    for(int i=0;i < pFileHeader->numOfRecordPerPage;i++){
        if((pPageHd->flags[i/8] & 1 << (i%8)) == 0){
            slotNum = i;
            pfFileHandle.UnpinPage(pageNum);
            return OK_RC;
        }
    }

    pfFileHandle.UnpinPage(pageNum);
    return RM_INVALID_PAGE_FORMAT;
}

RC RM_FileHandle::UpdateFreePageList(PageNum pageNum){

    //TODO:update to reuse deleted slotes in other pages
    RC rc;
    PF_PageHandle page;
    if(rc = pfFileHandle.GetThisPage(pageNum,page))
        return rc;

   char* pPageData;
    if(rc = page.GetData(pPageData)){
        pfFileHandle.UnpinPage(pageNum);
        return rc;
    }

    RM_PAGE_HEADER* pPageHd;
    pPageHd = (RM_PAGE_HEADER*)pPageData;

    for(int i=0;i < pFileHeader->numOfRecordPerPage;i++){
        if((pPageHd->flags[i/8] & 1 << (i%8)) == 0){
            pFileHeader->firstFreePage = pageNum;
            pfFileHandle.UnpinPage(pageNum);
            return OK_RC;
        }
    }

    pFileHeader->firstFreePage = BAD_PAGE_NUM;
    pfFileHandle.UnpinPage(pageNum);
    return OK_RC;
}