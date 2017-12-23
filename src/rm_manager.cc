#include <cstdio>
#include <cstring>
#include "rm.h"
#include "rm_internal.h"

RM_Manager::RM_Manager(PF_Manager &pfm):pfm(pfm){

}

RM_Manager::~RM_Manager(){

}

RC RM_Manager::CreateFile (const char *fileName, int recordSize){

    if(recordSize <= 0 || recordSize + sizeof(RM_PAGE_HEADER) > PF_PAGE_SIZE )
        return RM_INVALID_RECORD_SIZE;

    RC rc;
    if(rc = pfm.CreateFile(fileName))
        return rc;

    RM_FILE_HEADER fh;
    fh.fileType = RM_FILE_TYPE;
    fh.firstFreePage = BAD_PAGE_NUM;
    fh.recordSize = recordSize;

    PF_FileHandle fileHandle;

    if(rc = pfm.OpenFile(fileName,fileHandle)){
        DestroyFile(fileName);
        return rc;
    }

    PF_PageHandle pageHandle;
    if(rc = fileHandle.AllocatePage(pageHandle)){
        DestroyFile(fileName);
        return rc;
    }

    char* pData;
    if(rc = pageHandle.GetData(pData)){
        DestroyFile(fileName);
        return rc;
    }

    std::memcpy(pData,&fh,sizeof(fh));

    PageNum pageNum;
    pageHandle.GetPageNum(pageNum);

    if(rc = fileHandle.MarkDirty(pageNum)){
        DestroyFile(fileName);
        return rc;
    }

    fileHandle.UnpinPage(pageNum);
    pfm.CloseFile(fileHandle);
    return OK_RC;
}

RC RM_Manager::DestroyFile(const char *fileName){
    RC rc;
    if(rc = pfm.DestroyFile(fileName))
        return rc;

    return OK_RC;

}

RC RM_Manager::OpenFile(const char *fileName, RM_FileHandle &fileHandle){
    RC rc;
    PF_FileHandle pfFileHandle;
    PF_PageHandle pageHandle;

    if(rc = pfm.OpenFile(fileName,pfFileHandle))
        return rc;

    if(rc = pfFileHandle.GetFirstPage(pageHandle)){
        pfm.CloseFile(pfFileHandle);
        return rc;
    }

    char* pData;
    if(rc = pageHandle.GetData(pData)){
        pfm.CloseFile(pfFileHandle);
        return rc;
    }

    std::memcpy(fileHandle.pFileHeader,pData,sizeof(RM_FILE_HEADER));

    if(fileHandle.pFileHeader->fileType != RM_FILE_TYPE){
        pfm.CloseFile(pfFileHandle);
        return RM_INVALID_FILE_FORMAT;
    }

    fileHandle.pfFileHandle = pfFileHandle;

    PageNum pageNum;
    pageHandle.GetPageNum(pageNum);
    pfFileHandle.UnpinPage(pageNum);

    return OK_RC;
}

RC RM_Manager::CloseFile(RM_FileHandle &fileHandle){
    RC rc;
    PF_PageHandle pageHandle;

    if(rc = fileHandle.pfFileHandle.GetFirstPage(pageHandle)){
        pfm.CloseFile(fileHandle.pfFileHandle);
        return rc;
    }

    char* pData;
    if(rc = pageHandle.GetData(pData)){
        pfm.CloseFile(fileHandle.pfFileHandle);
        return rc;
    }

    std::memcpy(pData,fileHandle.pFileHeader,sizeof(RM_FILE_HEADER));

    PageNum pageNum;
    pageHandle.GetPageNum(pageNum);

    if(rc = fileHandle.pfFileHandle.MarkDirty(pageNum)){
        pfm.CloseFile(fileHandle.pfFileHandle);
        return rc;
    }

    fileHandle.pfFileHandle.UnpinPage(pageNum);
    pfm.CloseFile(fileHandle.pfFileHandle);
    return OK_RC;
}