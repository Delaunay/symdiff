# -*- coding: utf-8 -*-
__author__ = 'Pierre Delaunay'

from AST.Expression import *
import math


def counter(t, v):
    if isinstance(v, Multiplication):
        return div(t, v.right), v.left, True
    if isinstance(v, Addition):
        return sub(t, v.right), v.left, True
    if isinstance(v, Divide):
        return mult(t, v.right), v.left, True
    if isinstance(v, Subtraction):
        return add(t, v.right), v.left, True
    if isinstance(v, Exp):
        return log(t), v.expr, True
    if isinstance(v, Log):
        return exp(t), v.expr, True
    if isinstance(v, Pow):
        return pow(t, div(one(), v.power())), v.expr(), True

    return t, v, False


def solve(function: Expression, value=0, show_steps=False) -> Expression:
    """ Solve trivial Expression """

    f = function.copy()
    s = ScalarReal(value)
    c = True

    while c:
        if show_steps:
            print(f, '=', s)

        s, f, c = counter(s, f)

    return s


x = Unknown('x')
y = Unknown('y')

f1 = x * x * x + y
sol = solve(f1, 10)
print('Function: ', f1, '\tVariables:', f1.variables())
print('Equation: ', f1, ' = ', 10)
print('Sol     : ', x, ' = ', sol)
print('Eval    : ', f1.eval({x: sol}), '\n\n')

print('Steps:')
solve(f1, 10, show_steps=True)

print('\n\nPrimitive:')
f2 = x + y
print(f2, '\t', f2.primitive(x))

# print('Develop test')
# f2 = (x + 1) * (y + 1)
# print(f2)
# print(f2.develop())
# print(f2.develop().develop())