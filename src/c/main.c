///clean this shit right here
#include <inttypes.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char* argv[])
{
    int sizeOfData[16] = {1000, 5000, 10000,20000,30000,50000,100000,
              200000,300000,500000,1000000,2000000,3000000,
              5000000,6000000,10000000};

    MPI_Init(&argc, &argv);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int arrayToSend[sizeOfData];
    int arrayReceivedStd[sizeOfData];
    int arrayReceivedSynch[sizeOfData];
    int arrayFromSecondProcess[sizeOfData];

    int numberOfIterations = 0;

    int i = 0;
    for(i = 0; i<sizeOfData; i++)
    {
      arrayToSend[i] = 1;
    }
    for(i = 0; i<sizeOfData; i++)
    {
      arrayReceivedStd[i] = 0;
    }
    for(i = 0; i<sizeOfData; i++)
    {
      arrayReceivedSynch[i] = 0;
    }
    for(i = 0; i<sizeOfData; i++)
    {
      arrayFromSecondProcess[i] = 0;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (world_size < 2)
    {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    if (world_rank == 0)
    {

        MPI_Send(&arrayToSend, sizeOfData, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&arrayFromSecondProcess, sizeOfData, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for(i = 0; i<sizeOfData; i++)
        {
            printf("%i", arrayFromSecondProcess[i]);
        }
        printf("\n");
        MPI_Ssend(&arrayToSend, sizeOfData, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&arrayFromSecondProcess, sizeOfData, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(i = 0; i<sizeOfData; i++)
        {
            printf("%i", arrayFromSecondProcess[i]);
        }
        printf("\n");
    } else if (world_rank == 1)
    {
        /FILE * fileOptions = fopen("D://results.txt", "w"); ///fix for Gandalf paths
        //printf("Preparing to receive data\n");
        MPI_Recv(&arrayReceivedStd, sizeOfData, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&arrayReceivedStd, sizeOfData, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Recv(&arrayReceivedSynch, sizeOfData, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Ssend(&arrayToSend, sizeOfData, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("Received\n");
        for(i = 0; i<sizeOfData; i++)
        {
            printf("%i", arrayReceivedStd[i]);
        }
        printf("\n");
        for(i = 0; i<sizeOfData; i++)
        {
            printf("%i", arrayReceivedSynch[i]);
        }
        printf("\n");
        printf("Time: %d\n", timeDiff);
    }

    MPI_Finalize();
    return 0;
}
