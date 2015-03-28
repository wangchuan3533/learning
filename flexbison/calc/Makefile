all: calc
calc: calc.l calc.y
	bison -d calc.y
	flex calc.l
	cc -o $@ calc.tab.c lex.yy.c -lfl
.PHONY: clean
clean:
	rm -rf calc.tab.h calc.tab.c lex.yy.c calc
