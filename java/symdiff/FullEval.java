package symdiff;

import symdiff.Symdiff.Expression;
import java.util.Map;


class FullEval extends Visitor {

	FullEval(Expression exp, Map<String, Expression> context_){
        context = context_;
	    dispatch(exp);
	}
	
	static double run(Expression exp, Map<String, Expression> context_){
		FullEval e = new FullEval(exp, context_);
		return e.result;
    }
	
	void add(Expression lhs, Expression rhs){
		dispatch(lhs);
		double t = result;
		dispatch(rhs);
		result = result + t;
	}
	
	void mult(Expression lhs, Expression rhs){
		dispatch(lhs);
		double t = result;
		dispatch(rhs);
		result = result * t;
	}
	
	void neg(Expression lhs){
		dispatch(lhs);
		result = - result;
	}
	
	void inv(Expression lhs){
		dispatch(lhs);
		result = 1.0 / result;
	}
	void value(double v){
		result = v;
	}

	void placeholder(String name){
		Expression n = context.get(name);

		if (n == null)
			throw new RuntimeException("No placeholder allowed");
		else
			dispatch(n);
	}
	
	private double result;
	private Map<String, Expression> context;
}
