
a.out : ast.cpp ex.tab.c lex.yy.c main.cpp
	g++ ast.cpp ex.tab.c lex.yy.c main.cpp -std=c++11 -lfl

ex.tab.c: ex.y
	bison -d ex.y

lex.yy.c: ex.l
	flex ex.l

.PHONY: clean
clean:
	rm -f ex.tab.c ex.tab.h lex.yy.c a.out