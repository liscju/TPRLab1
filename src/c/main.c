///clean this shit right here
#include <inttypes.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

void parseOptions(int *argc, char *argv[])
{
    if(*argc==0)
    {
        printf("The all-knowing programmer predicted this error\n");
        exit(666);
    }
    if(*argc==1)
    {
        printf("No options provided, closing\n");
        exit(1);
    }else if(*argc!=3)
    {
        printf("The number of input args must be 3!\n");
        exit(2);
    }
}

int main(int argc, char* argv[])
{
    parseOptions(&argc, argv);

    int sizeOfData = atoi(argv[2]);
    int sendMode = atoi(argv[1]);

    MPI_Init(&argc, &argv);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int arrayToSend[sizeOfData];
    int arrayReceivedStd[sizeOfData];
    int arrayReceivedSynch[sizeOfData];
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
    MPI_Barrier(MPI_COMM_WORLD);
    if (world_size < 2)
    {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    double t1, t2, timeDiff = 0;
    if (world_rank == 0)
    {
        MPI_Send(&arrayToSend, sizeOfData, MPI_INT, 1, 0, MPI_COMM_WORLD);
        //MPI_Ssend(&arrayToSend, sizeOfData, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else if (world_rank == 1)
    {
        //FILE * fileOptions = fopen("D://results.txt", "w"); ///fix for Gandalf paths
        //printf("Preparing to receive data\n");
        MPI_Recv(&arrayReceivedStd, sizeOfData, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //MPI_Recv(&arrayReceivedSynch, sizeOfData, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Received\n");
        for(i = 0; i<sizeOfData; i++)
        {
            printf("%i", arrayReceivedStd[i]);
        }
        for(i = 0; i<sizeOfData; i++)
        {
            printf("%i", arrayReceivedSynch[i]);
        }
        printf("Time: %d\n", timeDiff);
    }

    MPI_Finalize();
    return 0;
}
