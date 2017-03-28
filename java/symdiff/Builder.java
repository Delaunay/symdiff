package symdiff;
import symdiff.Symdiff.*;

class Builder {
    private static double epsilon = 1e-6;

    static Expression add(Expression lhs, Expression rhs){
        if (lhs instanceof Const){
            Const v = (Const) lhs;
            if (Math.abs(v.value()) < epsilon)
                return rhs;
        }

        if (rhs instanceof Const){
            Const v = (Const) rhs;
            if (Math.abs(v.value()) < epsilon)
                return lhs;
        }

        if (rhs instanceof Const && lhs instanceof Const){
            Const r = (Const) rhs;
            Const l = (Const) lhs;
            return Builder.constant(r.value() + l.value());
        }

        return new Add(lhs, rhs);
    }

    static Expression mult(Expression lhs, Expression rhs){
        if (lhs instanceof Const){
            Const v = (Const) lhs;
            if (Math.abs(v.value()) < epsilon)
                return Builder.constant(0);
            if (Math.abs(v.value() - 1) < epsilon)
                return rhs;
        }

        if (rhs instanceof Const){
            Const v = (Const) rhs;
            if (Math.abs(v.value()) < epsilon)
                return Builder.constant(0);
            if (Math.abs(v.value() - 1) < epsilon)
                return lhs;
        }

        if (rhs instanceof Const && lhs instanceof  Const){
            Const r = (Const) rhs;
            Const l = (Const) lhs;
            return Builder.constant(r.value() * l.value());
        }

        return new Mult(lhs, rhs);
    }

    static Expression inv(Expression exp){
        if (exp instanceof  Inv){
            Inv v = (Inv) exp;
            return v.exp();
        }

        if (exp instanceof Const){
            Const v = (Const) exp;
            if (Math.abs(v.value() - 1) < epsilon)
                return v;
        }

        return new Inv(exp);
    }

    static Expression neg(Expression exp){
        if (exp instanceof  Neg){
            Neg v = (Neg) exp;
            return v.exp();
        }

        if (exp instanceof Const){
            Const v = (Const) exp;
            if (Math.abs(v.value()) < epsilon)
                return v;
        }

        return new Neg(exp);
    }

    static Expression constant(double n){
        return new Const(n);
    }
}
