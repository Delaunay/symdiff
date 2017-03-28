#include "Nodes.h"
#include <cassert>

namespace symdiff{

void dynamic_assert(bool cond, const char* message){
    return assert(cond && message);
}

}
