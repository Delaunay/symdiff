#ifndef SYMDIF_SMATH_HEADER
#define SYMDIF_SMATH_HEADER
/*
 *  Description:
 *    - Define mathmatical functio
 */

#include "sym.h"

namespace symdiff
{
    inline Sym sqrt(Sym x) {   return Sym::pow(x, make_val(0.5)); }

    inline Sym gaussian_pdf(Sym m, Sym s){
        auto x = make_var("x");

        auto constant = sqrt(Sym::pi() * 2) * s;
        auto up = - (((x - m) / s) ^ 2) / 2;

        return Sym::inv(constant) * Sym::exp(up);
    }

    inline Sym gaussian_pdf(double mean, double sd){
        Sym m = make_named_val("m", mean);
        Sym s = make_named_val("s", sd);

        return gaussian_pdf(m, s);
    }




}

#endif
