# -*- coding: utf-8 -*-
__author__ = 'Pierre Delaunay'
import math


class Expression:

    def __init__(self):
        pass

    def __repr__(self):
        return 'FAIL'

    def __str__(self):
        pass

    def __eq__(self, other) -> bool:
        if self is other:
            return True
        return False

    def derivate(self, x):
        """ derivate in respect of x"""
        return self

    def is_scalar(self):
        return False

    def is_one(self):
        return False

    def is_nul(self):
        return False

    def is_leaf(self):
        return False

    def eval(self, variables):
        """ partially evaluate the expression"""
        pass

    def full_eval(self, variables):
        """ fully evaluate the expression, every unknown must be specified"""
        pass

    def __mul__(self, other):
        return apply_operator(self, other, mult)

    def __add__(self, other):
        return apply_operator(self, other, add)

    def __truediv__(self, other):
        return apply_operator(self, other, div)

    def __pow__(self, other):
        return apply_operator(self, other, pow)

    def __sub__(self, other):
        return apply_operator(self, other, sub)

    def __neg__(self):
        """ return - self"""
        return mult(minus_one(), self)

    def variables(self):
        """ Return a set of unknown the expression depend of"""
        return set()

    def get_tree(self):
        """ Return Operation tree"""
        return []

    def apply_function(self, function):
        """ apply a function to the graph """
        return self

    def copy(self):
        """ return a copy of the expression"""
        return self.apply_function('copy')

    def simplify(self):
        return self.apply_function('simplify')

    def develop(self):
        return self.apply_function('develop')

    def factorize(self):
        return self.apply_function('factorize')

    def cancel(self):
        """ return the expression cancelling the current expression
            i.e x => 1/x     exp => log    x ** 2 => x ** 0.5 """
        return self

    def primitive(self, x):
        return self

    def _print(self):
        if self.is_leaf():
            return self.__str__()
        return '(' + self.__str__() + ')'

    def _id(self):
        raise NotImplementedError()

    def __lt__(self, other):
        return self._id() < other._id()


def reorder(a, b):
    ia = a._id()
    ib = b._id()

    if ia < ib:
        return a, b

    if ia != ib:
        return b, a

    if a.is_scalar() and b.is_scalar() and a.value > b.value:
        return b, a
    return a, b


class UnaryOperator(Expression):

    def __init__(self, expr: Expression):
        Expression.__init__(self)
        self.expr = expr

    def __eq__(self, other: Expression) -> bool:
        if self is other:
            return True

        if isinstance(other, type(self)):
            if self.expr == other.expr:
                return True

        return False

    def variables(self):
        return self.expr.variables()

    def get_tree(self):
        return [self] + self.expr.get_tree()


class BinaryOperator(Expression):

    def __init__(self, left: Expression, right: Expression):
        Expression.__init__(self)
        self.left = left
        self.right = right

    def __eq__(self, other: Expression) -> bool:
        if self is other:
            return True

        if isinstance(other, type(self)):
            if self.left == other.left and self.right == other.right:
                return True

        return False

    def variables(self):
        return self.left.variables().union(self.right.variables())

    def get_tree(self):
        return [self] + self.left.get_tree() + self.right.get_tree()


class ScalarReal(Expression):

    def __init__(self, value: float):
        Expression.__init__(self)

        # check if it is a true scalar
        # x = 1 - value

        self.value = value

    def __str__(self):
        return str(self.value)

    def __repr__(self):
        return 'Scalar<' + str(self.value) + '>'  #

    def __neg__(self):
        return scalar(- self.value)

    def __eq__(self, other):
        if self is other:
            return True

        if isinstance(other, ScalarReal) and other.value == self.value:
            return True
        return False

    def is_scalar(self):
        return True

    def is_one(self):
        return self.value == 1

    def is_nul(self):
        return self.value == 0

    def is_leaf(self):
        return True

    def derivate(self, x: Expression) -> Expression:
        return zero()

    def eval(self, variables):
        if self.is_one():
            return one()
        if self.is_nul():
            return zero()

        return self

    def full_eval(self, variables) -> Expression:
        return self.value

    def apply_function(self, function):
        return scalar(self.value)

    def primitive(self, x):
        return mult(self, x)

    def _id(self):
        return 0

# pre allocate common values
__one = ScalarReal(1)
__zero = ScalarReal(0)
__minus_one = ScalarReal(-1)
__two = ScalarReal(2)


def one():
    return __one


def zero():
    return __zero


def minus_one():
    return __minus_one


def two():
    return __two


class Unknown(Expression):

    def __init__(self, name: str, size: tuple=(1,)):
        Expression.__init__(self)
        self.name = name
        self.size = size

    def __repr__(self):
        return self.name + str(self.size)

    def __hash__(self):
        return str.__hash__(self.name)

    def __str__(self):
        return self.name

    def _id(self):
        return 1

    # def __eq__(self, other):
    #     if self is other:
    #         return True
    #
    #     if isinstance(other, Unknown) and self.name == other.name:
    #         return True
    #
    #     return False

    def is_leaf(self):
        return True

    def derivate(self, x: Expression) -> Expression:
        if x is self:
            return one()
        return zero()

    def eval(self, variables) -> Expression:
        if self in variables:
            return variables[self]
        return self

    def full_eval(self, variables) -> Expression:
        return variables[self].full_eval(variables)

    def variables(self):
        return {self}

    def primitive(self, x):
        if x is self:
            return pow(mult(div(one(), two()), self), two())
        return mult(self, x)


class Addition(BinaryOperator):

    def __init__(self, left: Expression, right: Expression):
        BinaryOperator.__init__(self, left, right)

    def __str__(self):
        return str(self.left) + ' + ' + str(self.right)

    def __repr__(self):
        return '+'  # '(' + str(self.left) + ' + ' + str(self.right) + ')'

    def derivate(self, x: Expression) -> Expression:
        return add(self.left.derivate(x), self.right.derivate(x))

    def eval(self, variables) -> Expression:
        l = self.left.eval(variables)
        r = self.right.eval(variables)

        if l.is_scalar() and r.is_scalar():
            return scalar(l.value + r.value)
        return add(l, r)

    def full_eval(self, variables) -> Expression:
        return self.left.full_eval(variables) + self.right.full_eval(variables)

    def apply_function(self, function):
        return add(getattr(self.left, function)(), getattr(self.right, function)())

    def primitive(self, x):
        return add(self.left.primitive(x), self.right.primitive(x))

    def _id(self):
        return 2

class Subtraction(BinaryOperator):

    def __init__(self, left: Expression, right: Expression):
        BinaryOperator.__init__(self, left, right)

    def __str__(self):
        return str(self.left) + ' - ' + str(self.right)

    def __repr__(self):
        return '-'  # '(' + str(self.left) + ' - ' + str(self.right) + ')'

    def derivate(self, x: Expression) -> Expression:
        return sub(self.left.derivate(x), self.right.derivate(x))

    def eval(self, variables) -> Expression:
        l = self.left.eval(variables)
        r = self.right.eval(variables)

        if l.is_scalar() and r.is_scalar():
            return ScalarReal(l.value - r.value)
        return sub(l, r)

    def full_eval(self, variables) -> Expression:
        return self.left.full_eval(variables) - self.right.full_eval(variables)

    def apply_function(self, function):
        return sub(getattr(self.left, function)(), getattr(self.right, function)())

    def primitive(self, x):
        return sub(self.left.primitive(x), self.right.primitive(x))

    def _id(self):
        return 3

class Multiplication(BinaryOperator):

    def __init__(self, left: Expression, right: Expression):
        BinaryOperator.__init__(self, left, right)

    def __str__(self):
        return self.left._print() + ' * ' + self.right._print()

    def __repr__(self):
        return '*'  # '(' + str(self.left) + ') * (' + str(self.right) + ')'

    def derivate(self, x: Expression) -> Expression:
        return add(mult(self.left, self.right.derivate(x)), mult(self.right, self.left.derivate(x)))

    def eval(self, variables) -> Expression:
        l = self.left.eval(variables)
        r = self.right.eval(variables)

        if l.is_scalar() and r.is_scalar():
            return scalar(l.value * r.value)
        return mult(l, r)

    def full_eval(self, variables) -> Expression:
        return self.left.full_eval(variables) * self.right.full_eval(variables)

    def apply_function(self, function):
        return mult(getattr(self.left, function)(), getattr(self.right, function)())

    def copy(self):
        return self.apply_function('copy')

    def simplify(self):
        return self.apply_function('simplify')

    def develop(self):
        if isinstance(self.right, Addition):
            return self.left * self.right.left + self.left * self.right.right

        if isinstance(self.left, Addition):
            return self.right * self.left.left + self.right * self.left.right

    def primitive(self, x):
        # integration by part
        # return mult() + mult()
        pass

    def _id(self):
        return 4


class Exp(UnaryOperator):

    def __init__(self, expr: Expression):
        UnaryOperator.__init__(self, expr)

    def __str__(self):
        return 'exp(' + str(self.expr) + ')'

    def __repr__(self):
        return 'exp'   #(' + str(self.expr) + ')'

    def derivate(self, x: Expression) -> Expression:
        return mult(self.expr.derivate(x), self)

    def eval(self, variables) -> Expression:
        l = self.expr.eval(variables)

        if l.is_scalar():
            return scalar(math.exp(l.value))
        return exp(l)

    def full_eval(self, variables) -> Expression:
        return math.exp(self.expr.full_eval(variables))

    def apply_function(self, function):
        return exp(getattr(self.expr, function)())

    def cancel(self):
        return log(self.expr)

    def primitive(self, x):
        return self

    def _id(self):
        return 5


class Log(UnaryOperator):

    def __init__(self, expr: Expression):
        UnaryOperator.__init__(self, expr)

    def __str__(self):
        return 'log(' + str(self.expr) + ')'

    def __repr__(self):
        return 'log'   # (' + str(self.expr) + ')'

    def derivate(self, x: Expression) -> Expression:
        return div(self.expr.derivate(x), self.expr)

    def eval(self, variables) -> Expression:
        l = self.expr.eval(variables)

        if l.is_scalar():
            return scalar(math.log(l.value))

        return log(l.value)

    def full_eval(self, variables) -> Expression:
        return math.log(self.expr.full_eval(variables))

    def apply_function(self, function):
        return log(getattr(self.expr, function)())

    def cancel(self):
        return exp(self.expr)

    def primitive(self, x):
        raise ArithmeticError('Log does not have a primitive')

    def _id(self):
        return 6

class Divide(BinaryOperator):

    def __init__(self, up: Expression, down: Expression):
        BinaryOperator.__init__(self, up, down)

    def __str__(self):
        return self.left._print() + ' / ' + self.right._print()
        # return '(' + str(self.left) + ') / (' + str(self.right) + ')'

    def __repr__(self):
        return '/'  # '(' + str(self.left) + ') / (' + str(self.right) + ')'

    def up(self):
        return self.left

    def down(self):
        return self.right

    def derivate(self, x: Expression) -> Expression:
        a = mult(self.right, self.left.derivate(x))
        b = mult(self.left, self.right.derivate(x))
        return div(sub(a, b), pow(self.right, scalar(2)))

    def eval(self, variables) -> Expression:
        l = self.left.eval(variables)
        r = self.right.eval(variables)

        if l.is_scalar() and r.is_scalar():
            return scalar(l.value / r.value)

        return div(l, r)

    def full_eval(self, variables) -> Expression:
        return self.left.full_eval(variables) / self.right.full_eval(variables)

    def apply_function(self, function):
        return div(getattr(self.left, function)(), getattr(self.right, function)())

    def _id(self):
        return 7

class Pow(BinaryOperator):

    def __init__(self, expr: Expression, power: Expression):
        BinaryOperator.__init__(self, expr, power)

    def power(self):
        return self.right

    def expr(self):
        return self.left

    def __str__(self):
        return self.left._print() + ' ^ ' + self.right._print()

    def __repr__(self):
        return '^'  # '(' + str(self.left) + ') ^ (' + str(self.right) + ')'

    def derivate(self, x: Expression) -> Expression:
        return mult(mult(self.power(), self.expr().derivate(x)), pow(self.expr(), add(self.power(), minus_one())))

    def eval(self, variables) -> Expression:
        l = self.left.eval(variables)
        r = self.right.eval(variables)

        if l.is_scalar() and r.is_scalar():
            return scalar(l.value ** r.value)
        return pow(l, r)

    def full_eval(self, variables) -> Expression:
        return self.left.full_eval(variables) ** self.right.full_eval(variables)

    def apply_function(self, function):
        return pow(getattr(self.left, function)(), getattr(self.right, function)())

    def primitive(self, x):
        v = self.power() + one()
        return mult(div(one(), v), pow(self.expr(), v))

    def _id(self):
        return 8


class MathConstant(ScalarReal):

    def __init__(self, name, value):
        ScalarReal.__init__(self, value)
        self.name = name

    def __str__(self):
        return self.name

    def __repr__(self):
        return self.name

    def copy(self):
        return self

    def _id(self):
        return 9

# define math constant
__euler = MathConstant('e', 2.718281828459045)
__pi = MathConstant('pi', 3.141592653589793)


def pi():
    return __pi


def e():
    return __euler


#
#   Helper function
#       Those make trivial simplification
def add(l: Expression, r: Expression) -> Expression:
    l, r = reorder(l, r)

    if l.is_nul():
        return r

    if r.is_scalar() and l.is_scalar():
        return scalar(r.value + l.value)

    if l is r:
        return mult(l, scalar(2))

    if isinstance(l, Subtraction):
        if l.right == r:
            return l.left

    if isinstance(r, Subtraction):
        if r.right == l:
            return r.left

    if isinstance(l, Multiplication):
        if l.right == r and l.left.is_scalar():
            return mult(r, scalar(l.left.value + 1))

        if l.left == r and l.right.is_scalar():
            return mult(r, scalar(l.right.value + 1))

    if isinstance(r, Multiplication):
        if r.right == l and r.left.is_scalar():
            return mult(l, scalar(r.left.value + 1))

        if r.left == l and r.right.is_scalar():
            return mult(l, scalar(r.right.value + 1))

    return Addition(l, r)


def mult(l: Expression, r: Expression) -> Expression:
    l, r = reorder(l, r)

    if l.is_nul() or r.is_nul():
        return zero()

    if r.is_one():
        return l

    if l.is_one():
        return r

    if l.is_scalar() and r.is_scalar():
        return scalar(l.value * r.value)

    if l is r:
        return pow(l, scalar(2))

    if l.is_scalar() and isinstance(r, Multiplication):
        if r.left.is_scalar():
            return mult(scalar(l.value * r.left.value), r.right)

    if isinstance(l, Divide):
        if l.down() == r:
            return l.up()

    if isinstance(r, Divide):
        if r.down() == l:
            return r.up()

    if isinstance(l, Pow):
        if l.left == r and l.right.is_scalar():
            return pow(r, scalar(l.right.value + 1))

    if isinstance(r, Pow):
        if r.left == l and r.right.is_scalar():
            return pow(l, scalar(r.right.value + 1))

    return Multiplication(l, r)


def exp(expr: Expression) -> Expression:

    if expr.is_nul():
        return one()

    if expr.is_one():
        return e()

    if isinstance(expr, Log):
        return expr.expr

    return Exp(expr)


def pow(expr: Expression, power: Expression) -> Expression:

    if power.is_nul():
        return one()

    if expr.is_nul():
        return zero()

    if power.is_one():
        return expr

    if isinstance(expr, Pow):
        return pow(expr.expr(), expr.power() * power)

    return Pow(expr, power)


def log(expr: Expression) -> Expression:

    if expr.is_one():
        return zero()

    if expr is e():
        return one()

    if isinstance(expr, Exp):
        return expr.expr

    return Log(expr)


def div(up: Expression, down: Expression) -> Expression:
    if down.is_one():
        return up

    if up == down:
        return one()

    if up.is_nul():
        return zero()

    if up.is_scalar() and down.is_scalar():
        rv = down.value
        lv = up.value

        if rv - int(rv) == 0 and lv - int(lv) == 0:
            # Simplify the expression
            gcd = math.gcd(int(rv), int(lv))
            rv /= gcd
            lv /= gcd
            up = scalar(lv)
            down = scalar(rv)
        else:
            return scalar(lv / rv)

    if isinstance(up, Multiplication):
        if up.left == down:
            return up.right

        if up.right == down:
            return up.left

    if isinstance(down, Multiplication):
        if down.left == up:
            return down.right

        if down.right == up:
            return down.left

    return Divide(up, down)


def scalar(v):
    if v == 0:
        return zero()
    if v == 1:
        return one()
    if v == -1:
        return minus_one()
    if v == 2:
        return two()

    return ScalarReal(v)


def sub(l: Expression, r: Expression) -> Expression:
    l, r = reorder(l, r)

    if l == r:
        return zero()

    if l.is_nul():
        return - r

    if r.is_nul():
        return l

    if r.is_scalar() and l.is_scalar():
        return scalar(l.value - r.value)

    if isinstance(l, Addition):
        if r == l.right:
            return l.left

        if r == l.left:
            return l.right

    return Subtraction(l, r)


#
#
#
def apply_operator(l, r, f):
    if isinstance(r, Expression):
        return f(l, r)

    return f(l, scalar(r))


if __name__ == '__main__':

    x = Unknown('x')
    y = Unknown('y')

    val = {x: scalar(5)}

    f = x ** 3 - y ** 2    # add(pow(x, 3), mult(y, 2))
    dfdx = f.derivate(x)

    print(' f   : ', f,    '\tEval: ', f.eval(val))
    print('dfdx : ', dfdx, '\tEval: ', dfdx.eval(val))

    val = {x: scalar(5), y: scalar(2)}
    print(' f   : ', f,    '\tEval: ', f.full_eval(val))
    print('dfdx : ', dfdx, '\tEval: ', dfdx.full_eval(val))

