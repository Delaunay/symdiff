#
#   If people don't build test don't add gtest
#
IF(BUILD_TESTING)
    # compile gtest
    ADD_SUBDIRECTORY(gtest)

ENDIF(BUILD_TESTING)
