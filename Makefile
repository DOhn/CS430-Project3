all: main.c raycast.c
	gcc main.c raycast.c -o raycaster

clean:
	rm -rf project1 *~
