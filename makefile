tester: control.c rewrite.c
	gcc control.c -o testerControl
	gcc rewrite.c -o testerRewrite

clean:
	rm *~