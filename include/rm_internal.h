
#ifndef _RM_INTERNAL_H_
#define _RM_INTERNAL_H_
#include "redbase.h"
#include "rm_rid.h"

#define RM_FILE_TYPE    0x0A

struct RM_FILE_HEADER{
    int fileType;
    int recordSize;
    PageNum firstFreePage;
};

struct RM_PAGE_HEADER{
    int headerSize;
    unsigned char flags[];
};

#endif //_RM_INTERNAL_H_