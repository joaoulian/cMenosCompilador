flex cmenos.l
bison -d cmenos.y
gcc -c *.c
gcc -o cmenos *.o -ly -lfl

