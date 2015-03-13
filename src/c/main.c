///clean this shit right here
#include <inttypes.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int sizeOfData[16] = {1000, 5000, 10000, 20000, 30000, 50000, 100000, 200000, 300000, 500000, 1000000, 2000000, 3000000, 5000000, 6000000, 10000000};
const int maxNumberOfIterations = 100;
const int maxSizeOfDataIteration = 15;

void normalProcess();
void calculatingProcess();

void initMPI(int * world_size, int * world_rank)
{
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, world_size);
    MPI_Barrier(MPI_COMM_WORLD);
}

double calculateTimeDifference(struct timespec * tend, struct timespec *tstart)
{
    return ((double)tend->tv_sec + 1.0e-9*tend->tv_nsec) - ((double)tstart->tv_sec + 1.0e-9*tstart->tv_nsec);
}

void checkAndLaunchProcesses(int * world_size, int * world_rank, char * argv[])
{
    if (*world_size < 2)
    {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    if (*world_rank == 0)
    {
        calculatingProcess();
    } else if (*world_rank == 1)
    {
        normalProcess();
    }
}
void calculatingProcess()
{

    struct timespec beginTime;
    struct timespec endTime;
    double timeDifference = 0.0;

    double averageBandwidth = 0;
    double bandwidth;
    double totalBandwidth;
    double averageTime = 0;
    double totalTime = 0;
    int sizeOfDataIncrement = 0;
    int numberOfIterations = 0;
    int * arrayToSend = (int*)malloc(1*sizeof(int));
    int * arrayFromSecondProcess = (int*)malloc(1*sizeof(int));
    size_t bufferSize = 0;
    if(arrayToSend!=NULL || arrayFromSecondProcess!=NULL)
    {
        FILE * resultsFile = fopen("results.txt", "w");
		FILE * delayAsyncFile = fopen("c_delayAsyn.txt", "w");
		fprintf(delayAsyncFile, "# X Y\n");
		FILE * delaySyncFile = fopen("c_delaySync.txt", "w");
		fprintf(delaySyncFile, "# X Y\n");
		FILE * bandwidthAsyncFile = fopen("c_bandwidthAsyn.txt", "w");
		fprintf(bandwidthAsyncFile, "# X Y\n");
		FILE * bandwidthSyncFile = fopen("c_bandwidthSync.txt", "w");
		fprintf(bandwidthSyncFile, "# X Y\n");
		
		// TODO ogarnac jak to jest w pythonie i zrobic analogicznie z zapisem wynikow do plikow
        for(sizeOfDataIncrement = 0; sizeOfDataIncrement<=maxSizeOfDataIteration; sizeOfDataIncrement++)
        {
            totalBandwidth = 0.0;
            bufferSize = (sizeOfData[sizeOfDataIncrement])*sizeof(int);
            arrayToSend = (int*)(realloc(arrayToSend, bufferSize));
            arrayFromSecondProcess = (int*)(realloc(arrayFromSecondProcess, bufferSize));
            for(numberOfIterations = 0; numberOfIterations < maxNumberOfIterations; numberOfIterations++)
            {
                clock_gettime(CLOCK_MONOTONIC, &beginTime);
                MPI_Send(arrayToSend, sizeOfData[sizeOfDataIncrement], MPI_INT, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(arrayFromSecondProcess, sizeOfData[sizeOfDataIncrement], MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        		clock_gettime(CLOCK_MONOTONIC, &endTime);
        		timeDifference = calculateTimeDifference(&endTime, &beginTime)/2;
                bandwidth = ((double)sizeOfData[sizeOfDataIncrement] * 2) / timeDifference;
        		totalBandwidth = totalBandwidth + bandwidth;
        		totalTime = totalTime + timeDifference;
            }
    	    averageTime = totalTime/(double)maxNumberOfIterations;
    	    averageBandwidth = (totalBandwidth*8/(1024*1024))/(double)maxNumberOfIterations;
            fprintf(resultsFile, "STD Size: %i\n", sizeOfData[sizeOfDataIncrement]);
            fprintf(resultsFile, "STD Time: %f sec\n", averageTime);
            fprintf(resultsFile, "STD Avg bw: %f Mbits/sec\n", averageBandwidth);
    	    totalBandwidth = 0;
            bandwidth = 0;
            averageBandwidth = 0;
	        averageTime = 0;
            totalTime = 0;
            for(numberOfIterations = 0; numberOfIterations < maxNumberOfIterations; numberOfIterations++)
            {
                clock_gettime(CLOCK_MONOTONIC, &beginTime);
                MPI_Ssend(arrayToSend, sizeOfData[sizeOfDataIncrement], MPI_INT, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(arrayFromSecondProcess, sizeOfData[sizeOfDataIncrement], MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                clock_gettime(CLOCK_MONOTONIC, &endTime);
		        timeDifference = calculateTimeDifference(&endTime, &beginTime)/2;
                bandwidth = ((double)sizeOfData[sizeOfDataIncrement] * 2) / timeDifference;
        		totalBandwidth = totalBandwidth + bandwidth;
        		totalTime = totalTime + timeDifference;
            }
	        averageTime = totalTime/(double)maxNumberOfIterations;
            clock_gettime(CLOCK_MONOTONIC, &endTime);
            timeDifference = calculateTimeDifference(&endTime, &beginTime)/2;
	        averageBandwidth = 0;
            averageBandwidth = (totalBandwidth*8/(1024*1024))/(double)maxNumberOfIterations;
            fprintf(resultsFile, "SYN Size: %i\n", sizeOfData[sizeOfDataIncrement]);
            fprintf(resultsFile, "SYN Time: %f sec\n", averageTime);
            fprintf(resultsFile, "SYN Avg bw: %f Mbits/sec\n", averageBandwidth);
        }
        fclose(resultsFile);
		fclose(delayAsyncFile);
		fclose(delaySyncFile);
		fclose(bandwidthAsyncFile);
		fclose(bandwidthSyncFile);
        free(arrayToSend);
        free(arrayFromSecondProcess);
    }
}

void normalProcess()
{
    int sizeOfDataIncrement = 0;
    int numberOfIterations = 0;
    int * arrayReceivedStd = (int*)malloc(1*sizeof(int));
    int * arrayReceivedSynch = (int*)malloc(1*sizeof(int));
    size_t bufferSize = 0;
    if(arrayReceivedStd!=NULL || arrayReceivedSynch!=NULL)
    {
        for(sizeOfDataIncrement = 0; sizeOfDataIncrement<=maxSizeOfDataIteration; sizeOfDataIncrement++)
        {
            bufferSize = (sizeOfData[sizeOfDataIncrement])*sizeof(int);
            arrayReceivedStd = (int*)realloc(arrayReceivedStd, bufferSize);
            arrayReceivedSynch = (int*)realloc(arrayReceivedSynch, bufferSize);
            for(numberOfIterations = 0; numberOfIterations < maxNumberOfIterations; numberOfIterations++)
            {
                MPI_Recv(arrayReceivedStd, sizeOfData[sizeOfDataIncrement], MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(arrayReceivedStd, sizeOfData[sizeOfDataIncrement], MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
            for(numberOfIterations = 0; numberOfIterations < maxNumberOfIterations; numberOfIterations++)
            {
                MPI_Recv(arrayReceivedSynch, sizeOfData[sizeOfDataIncrement], MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Ssend(arrayReceivedSynch, sizeOfData[sizeOfDataIncrement], MPI_INT, 0, 0, MPI_COMM_WORLD);
            };
        }
        free(arrayReceivedStd);
        free(arrayReceivedSynch);
    }
}

int main(int argc, char* argv[])
{
    int world_rank;
    int world_size;
    initMPI(&world_size, &world_rank);
    checkAndLaunchProcesses(&world_size, &world_rank, argv);
    MPI_Finalize();
    return 0;
}
