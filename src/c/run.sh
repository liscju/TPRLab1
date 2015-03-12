mpicc -o main.out main.c -lrt
lamboot
mpiexec -n 2 ./main.out