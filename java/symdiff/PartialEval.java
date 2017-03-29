package symdiff;

import symdiff.Symdiff.Expression;
import symdiff.Symdiff.Const;
import java.util.Map;

public class PartialEval extends Visitor {
    PartialEval(Expression exp, Map<String, Expression> ctx){
        context = ctx;
        dispatch(exp);
    }

    static Expression run(Expression exp, Map<String, Expression> ctx){
        PartialEval e = new PartialEval(exp, ctx);
        return e.result;
    }

    void add (Expression lhs, Expression rhs){
        dispatch(lhs);
        Expression rlhs = result;
        dispatch(rhs);
        Expression rrhs = result;


        if (rlhs instanceof Const && rrhs instanceof Const){
            Const vlhs = (Const) rlhs;
            Const vrhs = (Const) rrhs;
            result = Builder.constant(vlhs.value() + vrhs.value());
        }
        else
            result = Builder.add(rlhs, rrhs);
    }
    void mult(Expression lhs, Expression rhs){
        dispatch(lhs);
        Expression rlhs = result;
        dispatch(rhs);
        Expression rrhs = result;


        if (rlhs instanceof Const && rrhs instanceof Const){
            Const vlhs = (Const) rlhs;
            Const vrhs = (Const) rrhs;
            result = Builder.constant(vlhs.value() * vrhs.value());
        }
        else
            result = Builder.mult(rlhs, rrhs);
    }
    void neg (Expression lhs){
        dispatch(lhs);

        if (result instanceof Const){
            Const v = (Const) result;
            result = Builder.constant(- v.value());
        }
        else
            result = Builder.neg(lhs);
    }
    void inv (Expression lhs){
        dispatch(lhs);

        if (result instanceof Const){
            Const v = (Const) result;
            result = Builder.constant(1.0 / v.value());
        }
        else
            result = Builder.inv(lhs);
    }
    void placeholder(String name){
        Expression n = context.get(name);
        if (n == null)
            result = Builder.placeholder(name);
        else
            result = n;
    }
    void value(double v){
        result = Builder.constant(v);
    }

    private Expression result;
    private Map<String, Expression> context;
}
