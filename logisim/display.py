#!/usr/bin/env python3

import turtle
import fileinput
 
A, B, C, D, E, F, G = 0, 0, 0, 0, 0, 0, 0

turtle.setup(width=240, height=320)
turtle.pen(shown=False, pendown=False, pencolor='#aa0000', pensize=5)
turtle.tracer(0)

stdin = fileinput.input()
while True:
  turtle.clear()
  turtle.home()
  turtle.setpos(-20, 0)
  for i, draw in enumerate([G, C, D, E, F, A, B]):
    turtle.penup(); turtle.forward(5)
    turtle.pendown() if draw else turtle.penup()
    turtle.forward(40)
    turtle.penup(); turtle.forward(5)
    turtle.right(90)
    if i == 3: turtle.left(90)
  turtle.update()
  exec(stdin.readline())
