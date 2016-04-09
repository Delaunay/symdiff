#ifndef SYMDIF_SMATH_HEADER
#define SYMDIF_SMATH_HEADER
/*
 *  Description:
 *    - Define mathmatical functio
 */

#include "sym.h"

namespace symdiff
{
    inline Sym sqrt(Sym x) {   return pow(x, 0.5); }

    inline Sym gaussian_pdf(Sym m, Sym s){
        auto x = make_var("x");

        auto constant = sqrt(pi() * 2) * s;
        auto up = - (((x - m) / s) ^ 2) / 2;

        return inv(constant) * exp(up);
    }

    inline Sym gaussian_pdf(double mean, double sd){
        Sym m = make_val("m", mean);
        Sym s = make_val("s", sd);

        return gaussian_pdf(m, s);
    }




}

#endif
