package symdiff;

import symdiff.Symdiff.Expression;

class FullEval extends Visitor {

	FullEval(Expression exp){
		dispatch(exp);
	}
	
	static double run(Expression exp){
		FullEval e = new FullEval(exp);
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
	
	private double result;
}
