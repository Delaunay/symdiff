#
#   If people don't build test don't add gtest
#
IF(BUILD_TESTING)

    ADD_SUBDIRECTORY(gtest)
    #FIND_PACKAGE(gtest)

    #IF(NOT ${GTEST_FOUND})
        # compile gtest
    #ENDIF()

ENDIF(BUILD_TESTING)


