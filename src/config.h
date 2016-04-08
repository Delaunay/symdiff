#ifndef SYMDIF_CONFIG_HEADER
#define SYMDIF_CONFIG_HEADER

#include <unordered_map>

namespace symdiff{

//
//  Global configuration
//
class config
{
public:

    config(){
        _config["eval_scalar"] = 1;
        // _config["laziness"] = 0;
    }

    config& instance(){
        static config cfg;
        return cfg;
    }

    int& operator[] (const std::string& m){ return _config[m];  }

private:

    std::unordered_map<std::string, int> _config;
};


}

#endif
