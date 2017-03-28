package symdiff;

public class Symdiff {

	static abstract class Expression {
		abstract void visit(Visitor v);
	}
	
	static abstract class UnaryNode extends Expression{
		UnaryNode(Expression expr){
			_expr = expr;
		}
		
		Expression exp() {	return _expr; }
		
		private Expression _expr;
	}

	static abstract class BinaryNode extends Expression {
		
		BinaryNode(Expression lhs, Expression rhs){
			_lhs = lhs;
			_rhs = rhs;
		}
		
		private Expression _rhs;
		private Expression _lhs;
		
		Expression rhs() { return _rhs; }
		Expression lhs() { return _lhs; }
	}
	
	static public class Add extends BinaryNode{
		Add(Expression lhs, Expression rhs){
			super(lhs, rhs);
		}
		
		void visit(Visitor v){
			v.add(lhs(), rhs());
		}
	}
	
	static class Mult extends BinaryNode{
		Mult(Expression lhs, Expression rhs){
			super(lhs, rhs);
		}
		
		void visit(Visitor v){
			v.mult(lhs(), rhs());
		}
	}
	
	static class Inv extends UnaryNode{
		Inv(Expression exp){
			super(exp);
		}
		
		void visit(Visitor v){
			v.inv(exp());
		}
	}
	
	static class Neg extends UnaryNode{
		Neg(Expression exp){
			super(exp);
		}
		
		void visit(Visitor v){
			v.neg(exp());
		}
	}
	
	static class Const extends Expression{
		Const(double v){
			_value = v;
		}
		
		void visit(Visitor v) {
			v.value(value());
		}
		
		double value() {	return _value; }
		private double _value;
	}
	
	
	// ------------------------------------------------------------------------
	
	public Symdiff(Expression expr){
		_expr = expr;
	}
	
	public Symdiff(double expr){
		_expr = new Const(expr);
	}
	
	public Symdiff add (Symdiff b) { return new Symdiff(Builder.add(_expr, b._expr)); }
	public Symdiff mult(Symdiff b) { return new Symdiff(Builder.mult(_expr, b._expr)); }
	public Symdiff inv()           { return new Symdiff(Builder.inv(_expr)); }
	public Symdiff neg()           { return new Symdiff(Builder.neg(_expr)); }
	public Symdiff derivative()    { return new Symdiff(Derivative.run(_expr)); }
	
	public double full_eval()    {	return FullEval.run(_expr); }
	public void   pretty_print() {	PrettyPrint.run(_expr);     }
	
	private Expression _expr;
}
