package symdiff;
import symdiff.Symdiff.*;

import java.util.Map;

class Derivative extends Visitor {
    Derivative(Expression exp, String name_){
        name = name_;
        dispatch(exp);
    }

    static Expression run(Expression exp, String name){
        Derivative e = new Derivative(exp, name);
        return e.result;
    }

    void add(Expression lhs, Expression rhs){
        dispatch(lhs);
        Expression t = result;
        dispatch(rhs);
        result = Builder.add(result, t);
    }

    void mult(Expression lhs, Expression rhs){
        dispatch(lhs);
        Expression dlhs = result;
        dispatch(rhs);
        Expression drhs = result;
        result = Builder.add(Builder.mult(lhs, drhs), Builder.mult(rhs, dlhs));
    }

    void neg(Expression lhs){
        dispatch(lhs);
        result = new Neg(result);
    }

    void inv(Expression lhs){
        dispatch(lhs);
        Expression dxp = result;
        result = Builder.mult(Builder.neg(dxp), Builder.inv(Builder.mult(lhs, lhs)));
    }
    void value(double v){
        result = Builder.constant(0);
    }

    void placeholder(String n){
        if (name.equals(n))
            result = Builder.constant(1);
        else
            result = Builder.constant(0);
    }

    private Expression result;
    private String name;
}
