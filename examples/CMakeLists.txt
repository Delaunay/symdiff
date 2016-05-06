INCLUDE_DIRECTORIES(../src)

ADD_EXECUTABLE(example1 example1.cpp)
TARGET_LINK_LIBRARIES(example1 ${LLVM_LIBS} symdiff)
SET_PROPERTY(TARGET example1 PROPERTY CXX_STANDARD 11)

IF(${LLVM_FOUND})
    llvm_map_components_to_libnames(EX2_LLVM_LIBS
        Support
        Core
        nativecodegen
        ExecutionEngine
        Interpreter
        MC)

    ADD_EXECUTABLE(example2 example2.cpp)
    TARGET_LINK_LIBRARIES(example2 ${EX2_LLVM_LIBS} symdiff)
    SET_PROPERTY(TARGET example2 PROPERTY CXX_STANDARD 11)
ENDIF()

ADD_EXECUTABLE(pattern pattern.cpp)
TARGET_LINK_LIBRARIES(pattern ${LLVM_LIBS} symdiff)
SET_PROPERTY(TARGET pattern PROPERTY CXX_STANDARD 11)
