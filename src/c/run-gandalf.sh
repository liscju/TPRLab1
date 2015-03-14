mpicc -o main.out main.c -lrt
mpiexec -n 2 ./main.out -f ../gandalf-mpihosts