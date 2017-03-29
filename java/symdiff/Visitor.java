package symdiff;

import symdiff.Symdiff.Expression;

abstract class Visitor {
	
	void dispatch(Expression exp){ exp.visit(this);}
	
	abstract void add (Expression lhs, Expression rhs);
	abstract void mult(Expression lhs, Expression rhs);
	abstract void neg (Expression lhs);
	abstract void inv (Expression lhs);
	abstract void placeholder(String name);
	abstract void value(double v);
}
