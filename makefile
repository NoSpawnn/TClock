exe_name := tclock

build: main.c cursor.c
	gcc main.c cursor.c -o $(exe_name)

run: build
	./$(exe_name)

clean:
	rm -f $(exe_name)
