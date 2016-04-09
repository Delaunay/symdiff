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
        _config["eval_scalar"] = 1;         // evaluation is done if all args are scalar
        // Example:
        //          ln(2) => ......

        _config["no_approx"] = 0;           // only eval if result is rational/finite
        // Example
        //          ln(2) => ln(2)        but ln(1) => 0

        _config["mathcons_are_scalar"] = 1; // e/pi are treated as numbers instead of symbolic quantities

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
