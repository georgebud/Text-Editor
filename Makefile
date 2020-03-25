build: editor

editor: editor.o
	gcc -Wall -g editor.o -o editor

editor.o: editor.c
	gcc -c -g editor.c

run: editor
	./editor

clean:
	rm -f *.o *~ editor
