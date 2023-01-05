all : 
	gcc -c -g ./src/main.c
	gcc -c -g ./src/simulator.c
	gcc -c -g ./src/step.c
	gcc -o lc3simulator main.o simulator.o step.o

clear :
	rm *.o
	rm *.exe

# ./lc3simulator -b 12288 -d -f ./test/test1.bin -r ./test/register1.txt
# ./lc3simulator -b 12288 -d -f ./test/test2.bin -r ./test/register2.txt
# ./lc3simulator -b 12288 -d -f ./test/test3.bin -r ./test/register3.txt
