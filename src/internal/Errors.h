#ifndef SYMDIF_ERROR_HEADER
#define SYMDIF_ERROR_HEADER

namespace symdiff{

class Error{
public:

    Error(const std::string& str):
        _msg(str)
    {}

    const char* what() const noexcept{  return _msg.c_str(); }

private:
    std::string _msg;
};

class FullEvalError: public Error{
public:
    FullEvalError(const std::string& str): Error(str) {}
};

class CastError: public Error{
public:
    CastError(const std::string& str): Error(str) {}
};

class EvalError: public Error{
public:
    EvalError(const std::string& str): Error(str) {}
};


}

#endif
