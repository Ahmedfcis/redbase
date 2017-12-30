#include "catch.hpp"
#include "cstring"
#include "vector"
#include "rm.h"

#define TEST_RM_FILE "_rm_test.dat"
#define TEST_RM_SIZE 101

bool check_rc(RC rc);
#define REQUIRE_RC(rc)    REQUIRE(check_rc(rc))

TEST_CASE( "1000_RECORDS_WRITE_READ", "[RM]" ) {
    PF_Manager pfManager;
    RM_Manager rmManager(pfManager);
    RM_FileHandle rmFile;
    RM_Record rmRecord;
    RID rid;
    char data[TEST_RM_SIZE];
    std::vector<RID> RIDlist;

    rmManager.DestroyFile(TEST_RM_FILE);

    REQUIRE_RC(rmManager.CreateFile(TEST_RM_FILE,TEST_RM_SIZE));
    REQUIRE_RC(rmManager.OpenFile(TEST_RM_FILE,rmFile));

    //Write 10 records
    unsigned char j='a';
    for(int i=0;i<10;i++){
        std::memset(data,j,TEST_RM_SIZE);
        REQUIRE_RC(rmFile.InsertRec(data,rid));
        RIDlist.push_back(rid);
        j++;
    }

    REQUIRE(RIDlist.size()==10);

    //Reopen test file
    REQUIRE_RC(rmManager.CloseFile(rmFile));
    REQUIRE_RC(rmManager.OpenFile(TEST_RM_FILE,rmFile));

    //Write 990 records
    for(int i=10;i<1000;i++){
        std::memset(data,j,TEST_RM_SIZE);
        REQUIRE_RC(rmFile.InsertRec(data,rid));
        RIDlist.push_back(rid);
        j++;
    }


    REQUIRE(RIDlist.size()==1000);

    //Verify all records
    j='a';
    for(auto& r : RIDlist){
        REQUIRE_RC(rmFile.GetRec(r,rmRecord));
        char* d;
        REQUIRE_RC(rmRecord.GetData(d));
        for(int i=0;i<TEST_RM_SIZE;i++)
            REQUIRE(((unsigned char)d[i])==j);
        j++;
    }

    //Reopen test file
    REQUIRE_RC(rmManager.CloseFile(rmFile));
    REQUIRE_RC(rmManager.OpenFile(TEST_RM_FILE,rmFile));

    SECTION("Test Read all again"){
        //Verify all records
        j='a';
        for(auto& r : RIDlist){
            REQUIRE_RC(rmFile.GetRec(r,rmRecord));
            char* d;
            REQUIRE_RC(rmRecord.GetData(d));
            for(int i=0;i<TEST_RM_SIZE;i++)
                REQUIRE(((unsigned char)d[i])==j);
            j++;
        }
    }

    SECTION("Test delete/insert one record"){
        //Delete record 513
        REQUIRE_RC(rmFile.DeleteRec(RIDlist.at(513)));

        //Insert record
        std::memset(data,0,TEST_RM_SIZE);
        REQUIRE_RC(rmFile.InsertRec(data,rid));
        REQUIRE(RIDlist.at(513) == rid);
    }

    SECTION("Test delete/insert tow record"){
        //Delete record 513
        REQUIRE_RC(rmFile.DeleteRec(RIDlist.at(513)));
        //Delete record 111
        REQUIRE_RC(rmFile.DeleteRec(RIDlist.at(111)));

        //Insert 2 records
        std::memset(data,0,TEST_RM_SIZE);
        REQUIRE_RC(rmFile.InsertRec(data,rid));
        REQUIRE(RIDlist.at(111) == rid);

        REQUIRE_RC(rmFile.InsertRec(data,rid));
        REQUIRE(RIDlist.at(513) == rid);
    }

//    SECTION("Test Scan File"){
//        RM_FileScan rmScan;
//
//        REQUIRE_RC(rmScan.OpenScan());
//
//        REQUIRE_RC(rmScan.CloseScan());
//    }

    REQUIRE_RC(rmManager.CloseFile(rmFile));
    REQUIRE_RC(rmManager.DestroyFile(TEST_RM_FILE));
}