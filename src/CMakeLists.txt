#AUX_SOURCE_DIRECTORY(. SRC_LIST)
#AUX_SOURCE_DIRECTORY(internal SRC_LIST)

INCLUDE_DIRECTORIES(${SFML_INCLUDE_DIR})
INCLUDE_DIRECTORIES(../dependencies/thor/include)
LINK_DIRECTORIES(../dependencies/thor/lib/)

#INCLUDE_DIRECTORIES(../dependencies/SDL2/include)
#LINK_DIRECTORIES(../dependencies/SDL2/lib/x64)

MESSAGE(STATUS ${SFML_LIBRARIES})

SET(SRC_LIST
    Nodes.h
    PrettyPrint.h
    Derivate.h
    Builder.h
    Context.h
    Context.cpp
    FullEval.h
    PartialEval.h
    DeepCopy.h
    Substitution.h
    LLVM.h
    Equiv.h
    Sym.h
    Sym.cpp
    StackGenerator.h
)

SET(${CXX_STANDARD_REQUIRED} ON)

ADD_EXECUTABLE(symdiff main.cpp ${SRC_LIST})
SET_PROPERTY(TARGET symdiff PROPERTY CXX_STANDARD 14)

ADD_EXECUTABLE(wdiff window.cpp ${SRC_LIST})
TARGET_LINK_LIBRARIES(wdiff thor-d ${SFML_LIBRARIES})

SET_PROPERTY(TARGET wdiff PROPERTY CXX_STANDARD 14)

##   IMGUI files
## ======================
#AUX_SOURCE_DIRECTORY(../dependencies/imgui IMGUI_SRC_LIST)
#INCLUDE_DIRECTORIES(../dependencies/imgui)

##   OpenGL
## ======================

#FIND_PACKAGE(OpenGL)
#INCLUDE_DIRECTORIES(${OpenGL_includes})

##   Glew files
## ======================
#INCLUDE_DIRECTORIES(../dependencies/glew/include)
#LINK_DIRECTORIES(I:/source/glew/bin/Release/Win32)

#ADD_EXECUTABLE(symdiffgui imgui_demo.cpp ${IMGUI_SRC_LIST})
#TARGET_LINK_LIBRARIES(symdiffgui gl3w ${opengl_libraries})
#SET_PROPERTY(TARGET symdiffgui PROPERTY CXX_STANDARD 14)

