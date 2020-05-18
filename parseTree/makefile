frontend: y.tab.c lex.yy.c
	gcc -o frontend y.tab.c

y.tab.c:
	yacc -vdty parser.y

lex.yy.c:
	flex scanner.l

clean:
	rm y.tab.c y.tab.h lex.yy.c