#ifndef __REVISION_DATA_H__
#define __REVISION_DATA_H__
    #define _HASH                      "@rev_hash@"
    #define _DATE                      "@rev_date@"
    #define _BRANCH                    "@rev_branch@"
    #define _SOURCE_DIRECTORY          "@CMAKE_SOURCE_DIR@"
    #define VER_COMPANYNAME_STR        "Whatever Developers"
    #define VER_LEGALCOPYRIGHT_STR     "(c) Whatever"
    #define VER_FILEVERSION            0,0,0
    #define VER_FILEVERSION_STR        "@rev_hash@ @rev_date@ (@rev_branch@ branch)"
    #define VER_PRODUCTVERSION         VER_FILEVERSION
    #define VER_PRODUCTVERSION_STR     VER_FILEVERSION_STR
    #define COMPILER_C_FLAGS           "@CMAKE_C_FLAGS@"
    #define COMPILER_CXX_FLAGS         "@CMAKE_CXX_FLAGS@"
    #cmakedefine LLVM_FOUND
    #cmakedefine USE_LLVM_IR
#endif // __REVISION_DATA_H__
