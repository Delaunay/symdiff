#include <stdio.h>
#include <stdlib.h>
#include <string.h>




// Symdiff start here

#define SYMDIFF_NODE_TYPE\
    X(Placeholder)\
    X(Scalar)\
    X(Add)\
    X(Mult)\
    X(Neg)\
    X(Inv)

// Runtime type information
enum __symdiff_RTTI{
#define X(name) __RTTI_##name,
    SYMDIFF_NODE_TYPE
#undef X
};

// This define common variables
#define __HEAD__ \
    enum __symdiff_RTTI __type;

#define __BINARY_OP_HEAD__ \
    __HEAD__ \
    SymExpr _lhs; \
    SymExpr _rhs;

#define __UNARY_OP_HEAD__ \
    __HEAD__ \
    SymExpr _expr;


#define SYM_NODE(name, body) \
    struct name {\
        __HEAD__ \
    \
        body;\
    };\
    typedef struct name name;

// This emulates 'inheritance'
//  - SymExpr is the abstract class
//  - SymBinary is the base for BinaryOperator etc...
struct __SymExpr       {   __HEAD__             };
typedef struct __SymExpr*   __SymExpr;

// Memory Management
struct shared_ptr
{
    int* count;
    __SymExpr ptr;
};
typedef struct shared_ptr shared_ptr;

__SymExpr shared_get(shared_ptr p) {    return p.ptr;    }


shared_ptr shared_copy(shared_ptr p){
    *p.count = *p.count + 1;
    return p;
}

shared_ptr from_ptr(__SymExpr ptr){
    shared_ptr p;
    p.count = malloc(sizeof(size_t));
    *p.count = 1;
    p.ptr = ptr;
    return p;
}

//
shared_ptr shared_owned(shared_ptr p){
   *p.count = *p.count - 1;
    return p;
}

typedef shared_ptr SymExpr;

struct __SymBinaryExpr {   __BINARY_OP_HEAD__   };
struct __SymUnaryExpr  {   __UNARY_OP_HEAD__    };
typedef struct __SymBinaryExpr* SymBinary;
typedef struct __SymUnaryExpr*  SymUnary;

void sym_print(SymExpr expr);

void shared_delete(shared_ptr p){

    *p.count = *p.count - 1;

    //printf(" : %d : ", *p.count);
    //sym_print(p);
    //printf("\n");

    if (*p.count == 0){
        free(p.count);
        free(p.ptr);
    }
}


#define DYN_CAST(x, type, target) \
    type x = (type) shared_get(target);

#define RET_CAST(expr) (__SymExpr) expr

// generate a switch over each nodes
// We expect most functions to be inlined
#define SYM_CASE(fun, type)\
    case __RTTI_##type:{\
        DYN_CAST(x, Sym##type*, expr)\
        return sym_##fun##_##type(name, x);\
    }


// define data structure
SYM_NODE(SymPlaceholder, const char* _name)
SYM_NODE(SymScalar, double _value)

SYM_NODE(SymAdd, SymExpr _lhs; SymExpr _rhs)
SYM_NODE(SymMult, SymExpr _lhs; SymExpr _rhs)

SYM_NODE(SymInv, SymExpr _expr)
SYM_NODE(SymNeg, SymExpr _expr)


/*
 *  Since constructors use shared_copy()
 *  We need to use shared_owned() on internally created nodes
 *  This is not entirely done yet. So some memory leaks might still be present
 */
SymExpr sym_placeholder(const char* name){
    SymPlaceholder* x = malloc(1 * sizeof(SymPlaceholder));
    x->__type = __RTTI_Placeholder;
    x->_name = name;
    return from_ptr(RET_CAST(x));
}

SymExpr sym_scalar(double value){
    SymScalar* x = malloc(1 * sizeof(SymScalar));
    x->__type = __RTTI_Scalar;
    x->_value = value;
    return from_ptr(RET_CAST(x));
}

SymExpr sym_add(SymExpr lhs, SymExpr rhs){
    SymAdd* x = malloc(1 * sizeof(SymAdd));
    x->__type = __RTTI_Add;
    x->_rhs = shared_copy(rhs);
    x->_lhs = shared_copy(lhs);
    return from_ptr(RET_CAST(x));
}

SymExpr sym_mult(SymExpr lhs, SymExpr rhs){
    SymMult* x = malloc(1 * sizeof(SymMult));
    x->__type = __RTTI_Mult;
    x->_rhs = shared_copy(rhs);
    x->_lhs = shared_copy(lhs);
    return from_ptr(RET_CAST(x));
}

SymExpr sym_inv(SymExpr expr){
    SymInv* x = malloc(1 * sizeof(SymInv));
    x->__type = __RTTI_Inv;
    x->_expr = shared_copy(expr);
    return from_ptr(RET_CAST(x));
}

SymExpr sym_neg(SymExpr expr){
    SymNeg* x = malloc(1 * sizeof(SymNeg));
    x->__type = __RTTI_Neg;
    x->_expr = shared_copy(expr);
    return from_ptr(RET_CAST(x));
}

SymExpr sym_sub(SymExpr lhs, SymExpr rhs){  return sym_add(lhs, sym_neg(rhs));  }
SymExpr sym_div(SymExpr lhs, SymExpr rhs){  return sym_mult(lhs, sym_inv(rhs)); }

// This implements, explicitly, virtual calls
void sym_print(SymExpr expr){

    if (shared_get(expr) == NULL){
        printf("NULL");
        return ;
    }

    switch (shared_get(expr)->__type){
        // Leafs
        case __RTTI_Placeholder:{
            DYN_CAST(x, SymPlaceholder*, expr)
            printf("%s", x->_name);
            return;
        }
        case __RTTI_Scalar:{
            DYN_CAST(x, SymScalar*, expr)
            printf("%f", x->_value);
            return;
        }
        // Binary Operators
        case __RTTI_Mult:{
            DYN_CAST(x, SymBinary, expr)
            sym_print(x->_lhs); printf(" * "); sym_print(x->_rhs);
            return;
        }
        case __RTTI_Add:{
            DYN_CAST(x, SymBinary, expr)
            sym_print(x->_lhs); printf(" + "); sym_print(x->_rhs);
            return;
        }
        // Unary Operators
        case __RTTI_Inv:{
            DYN_CAST(x, SymUnary, expr)
            printf(" 1 / "); sym_print(x->_expr);
            return;
        }
        case __RTTI_Neg:{
            DYN_CAST(x, SymUnary, expr)
            printf(" - "); sym_print(x->_expr);
            return;
        }
    }
}

SymExpr sym_deriv_Placeholder(const char* name, SymPlaceholder* x){
    if (strcmp(x->_name, name) == 0)
        return shared_owned(sym_scalar(1));
    return shared_owned(sym_scalar(0));
}

SymExpr sym_deriv_Scalar(const char* name, SymScalar* x){
    return shared_owned(sym_scalar(0));
}

SymExpr sym_deriv(const char* name, SymExpr expr);

SymExpr sym_deriv_Mult(const char* name, SymMult* x){
    SymExpr dlhs = sym_deriv(name, x->_lhs);
    SymExpr drhs = sym_deriv(name, x->_rhs);

    return sym_add(shared_owned(sym_mult(dlhs, x->_rhs)),
                   shared_owned(sym_mult(drhs, x->_lhs)));
}

SymExpr sym_deriv_Add(const char* name, SymAdd* x){
    return sym_add(sym_deriv(name, x->_lhs),
                   sym_deriv(name, x->_rhs));
}

SymExpr sym_deriv_Inv(const char* name, SymInv* x){
    SymExpr up   = sym_deriv(name, x->_expr);
    SymExpr down = sym_mult(x->_expr, x->_expr);

    return sym_neg(sym_div(up, down));
}

SymExpr sym_deriv_Neg(const char* name, SymNeg* x){
    return sym_neg(sym_deriv(name, x->_expr));
}

SymExpr sym_deriv(const char* name, SymExpr expr){

    switch (shared_get(expr)->__type){
        #define X(name) SYM_CASE(deriv, name)
            SYMDIFF_NODE_TYPE
        #undef X
    }
}

SymExpr nullptr(){
    static shared_ptr p;
    p.count = NULL;
    p.ptr = NULL;
    return p;
}

void sym_free(SymExpr expr)
{
    switch (shared_get(expr)->__type){
        // Binary Operators
        case __RTTI_Add:
        case __RTTI_Mult:{
            DYN_CAST(x, SymBinary, expr)
            sym_free(x->_rhs); x->_rhs = nullptr();
            sym_free(x->_lhs); x->_lhs = nullptr();
            break;
        }

        // Unary Operators
        case __RTTI_Inv:
        case __RTTI_Neg:{
            DYN_CAST(x, SymUnary, expr)
            sym_free(x->_expr); x->_expr = nullptr();
            break;
        }

        default:
            break;
    }

    return shared_delete(expr);
}


int main ()
{
    // Freeing memory is tricky
    // One way to solve the problem would be to hard copy everything

    // Or We could implement a shared ptr that we would have to
    // manage ourself (what a pain)

    SymExpr x = sym_placeholder("x");                           // 2 alloc
    SymExpr expr = sym_mult(x, x);                              // 2 alloc

    sym_print(expr);

    printf("\n");

    SymExpr df = sym_deriv("x", expr);                          // 10 Alloc
                                                                // 2 * 2 one()
                                                                // 2 * 2 mult
                                                                // 2   Add
    sym_print(df);

    printf("\n");

    printf("%d \n", *x.count);

    // NB: even with a shared_ptr we need to explicitly call free
    sym_free(df);
    sym_free(expr);
    sym_free(x);

    return 0;
}




