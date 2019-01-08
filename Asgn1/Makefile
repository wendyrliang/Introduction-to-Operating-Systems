# Ruiwen Liang
# rliang4
# cs111-asgn1
# Makefile

all : argshell

argshell : lex.yy.c
	cc -o argshell argshell.c lex.yy.c -lfl -Wno-int-to-pointer-cast #-Wall

lex.yy.c : shell.l
	flex shell.l

clean :
	rm argshell *.o lex.yy.c argshell.core
