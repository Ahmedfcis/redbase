//
// rm.h
//
//   Record Manager component interface
//
// This file does not include the interface for the RID class.  This is
// found in rm_rid.h
//

#ifndef RM_H
#define RM_H

// Please DO NOT include any files other than redbase.h and pf.h in this
// file.  When you submit your code, the test program will be compiled
// with your rm.h and your redbase.h, along with the standard pf.h that
// was given to you.  Your rm.h, your redbase.h, and the standard pf.h
// should therefore be self-contained (i.e., should not depend upon
// declarations in any other file).

// Do not change the following includes
#include "redbase.h"
#include "rm_rid.h"
#include "pf.h"

struct RM_FILE_HEADER;

//
// RM_Record: RM Record interface
//
class RM_Record {
public:
    RM_Record ();
    ~RM_Record();

    RM_Record(const RM_Record& r)=delete;
    RM_Record(RM_Record&& r);

    RM_Record& operator = (const RM_Record& r)=delete;
    RM_Record& operator = (RM_Record&& r);
    // Return the data corresponding to the record.  Sets *pData to the
    // record contents.
    RC GetData(char *&pData) const;

    // Return the RID associated with the record
    RC GetRid (RID &rid) const;
private:
    friend class RM_FileHandle;
    RC  SetData(const RID& rid,char* pData,int size);

    RID     rid;
    char*   pData;
    int     size;
};

//
// RM_FileHandle: RM File interface
//
class RM_FileHandle {
public:
    RM_FileHandle ();
    ~RM_FileHandle();

    // Given a RID, return the record
    RC GetRec     (const RID &rid, RM_Record &rec) const;

    RC InsertRec  (const char *pData, RID &rid);       // Insert a new record

    RC DeleteRec  (const RID &rid);                    // Delete a record
    RC UpdateRec  (const RM_Record &rec);              // Update a record

    // Forces a page (along with any contents stored in this class)
    // from the buffer pool to disk.  Default value forces all pages.
    RC ForcePages (PageNum pageNum = ALL_PAGES);

    RC GetNext(RID& rid) const;

private:
    friend class RM_Manager;
    friend class RM_FileScan;
    PF_FileHandle   pfFileHandle;
    RM_FILE_HEADER*  pFileHeader;

    RC UpdateRec  (const char* pData,PageNum pageNum, SlotNum slotNum);
    RC GetFirstFreeSlot(PageNum& pageNum,SlotNum& slotNum);
    RC UpdateFreePageList(PageNum pageNum);
};

//
// RM_FileScan: condition-based scan of records in the file
//
class RM_FileScan {
public:
    RM_FileScan  ();
    ~RM_FileScan ();

    RC OpenScan  (const RM_FileHandle &fileHandle,
                  AttrType   attrType,
                  int        attrLength,
                  int        attrOffset,
                  CompOp     compOp,
                  void       *value,
                  ClientHint pinHint = NO_HINT); // Initialize a file scan
    RC GetNextRec(RM_Record &rec);               // Get next matching record
    RC CloseScan ();                             // Close the scan

private:
    const RM_FileHandle *fileHandle;
    AttrType   attrType;
    int        attrLength;
    int        attrOffset;
    CompOp     compOp;
    char       *value;
    ClientHint pinHint;
    RID        current;

    bool CheckRecord(RM_Record& r);
};

//
// RM_Manager: provides RM file management
//
class RM_Manager {
public:
    RM_Manager    (PF_Manager &pfm);
    ~RM_Manager   ();

    RC CreateFile (const char *fileName, int recordSize);
    RC DestroyFile(const char *fileName);
    RC OpenFile   (const char *fileName, RM_FileHandle &fileHandle);

    RC CloseFile  (RM_FileHandle &fileHandle);
private:
    PF_Manager& pfm;
};

//
// Print-error function
//
void RM_PrintError(RC rc);

#define RM_EOF                  START_RM_WARN + 10 //first 10 reserved for RID
#define RM_INVALID_RECORD_SIZE  START_RM_WARN + 11
#define RM_INVALID_SCAN         START_RM_WARN + 12

#define RM_INVALID_FILE_FORMAT  START_RM_ERR - 10 //first 10 reserved for RID
#define RM_INVALID_RECORD       START_RM_ERR - 11
#define RM_INVALID_PAGE_FORMAT  START_RM_ERR - 12

#endif
