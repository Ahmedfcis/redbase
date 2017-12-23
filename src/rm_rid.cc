#include <cstdio>
#include "rm_rid.h"

RID::RID(){                                        // Default constructor
    pageNum = BAD_PAGE_NUM;
    slotNum = BAD_SLOT_NUM;
}

RID::RID(PageNum pageNum, SlotNum slotNum){
    this->pageNum = pageNum;
    this->slotNum = slotNum;
}

RID::~RID(){                                        // Destructor

}

RC RID::GetPageNum(PageNum &pageNum) const{         // Return page number
    if(this->pageNum == BAD_PAGE_NUM)
        return RM_INVALID_RID;
    pageNum = this->pageNum;
    return OK_RC;
}

RC RID::GetSlotNum(SlotNum &slotNum) const{         // Return slot number
    if(this->slotNum == BAD_SLOT_NUM)
        return RM_INVALID_RID;
    slotNum = this->slotNum;
    return OK_RC;
}