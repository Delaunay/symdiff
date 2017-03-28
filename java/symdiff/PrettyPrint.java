package symdiff;

import symdiff.Symdiff.Expression;

class PrettyPrint extends Visitor {

	PrettyPrint(Expression exp){
		dispatch(exp);
	}
	
	static void run(Expression exp){
		new PrettyPrint(exp);
    }
	
	void add(Expression lhs, Expression rhs){
		System.out.print("(+ ");
		dispatch(lhs);
		System.out.print(" ");
		dispatch(rhs);
		System.out.print(")");
	}
	
	void mult(Expression lhs, Expression rhs){
		System.out.print("(* ");
		dispatch(lhs);
		System.out.print(" ");
		dispatch(rhs);
		System.out.print(")");
	}
	
	void neg(Expression lhs){
		System.out.print("(- ");
		dispatch(lhs);
		System.out.print(")");
	}
	
	void inv(Expression lhs){
		System.out.print("(1/ ");
		dispatch(lhs);
		System.out.print(")");
	}
	
	void value(double v){
		System.out.print(v);
	}
}
