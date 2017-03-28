
SET(BENCH_SRC
    BenchTimer.h
    BenchDummy.cpp)

ADD_EXECUTABLE(bench_dummy BenchDummy.cpp ${BENCH_SRC})
TARGET_LINK_LIBRARIES(bench_dummy symdiff -lrt)
SET_PROPERTY(TARGET bench_dummy PROPERTY CXX_STANDARD 14)
