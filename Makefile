build:
	gcc -std=c99 -Wall p.c -o quadtree -lm
clean:
	rm quadtree