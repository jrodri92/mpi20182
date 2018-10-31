/*
 *  mpi-example2.c
 *  
 *  Created by Edwin Montoya on 10/16/18.
 *  Copyright 2018 __Universidad EAFIT__. All rights reserved.
 *
 */

#include <stdio.h>
#include <math.h>

#include "mpi.h"

#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

#define LX 30000
#define LY 10000

long vec_sum[LY];
long matrix[LX][LY];

int taskId,
	numTasks,
	numWorkers,
	sourceId,
	destId,
	currentWorker = 0;

MPI_Status status;

void initMPI(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
	MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
	numWorkers = numTasks - 1;
}

void sendRows()
{
	long count = LX;
	long index;
	long i;
	long w;
	for (i = 0; i < LY; i++)
	{
		w = nextWorker();
		MPI_Send(&i, 1, MPI_LONG, w, FROM_MASTER, MPI_COMM_WORLD);
		MPI_Send(&matrix[0][i], count, MPI_LONG, w, FROM_MASTER, MPI_COMM_WORLD);
	}
	long fin = -1;
	for (i = 1; i <= numWorkers; i++)
	{
		w = nextWorker();
		MPI_Send(&fin, 1, MPI_LONG, w, FROM_MASTER, MPI_COMM_WORLD);
	}
}

void recvRows()
{
	long count = LX;
	long index = 0;
	long result;
	while (index != -1)
	{
		MPI_Recv(&index, 1, MPI_LONG, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		if (index != -1)
		{
			MPI_Recv(&matrix[0][index], count, MPI_LONG, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
			result = processRow(index);
			MPI_Send(&index, 1, MPI_LONG, MASTER, FROM_MASTER, MPI_COMM_WORLD);
			MPI_Send(&result, 1, MPI_LONG, MASTER, FROM_MASTER, MPI_COMM_WORLD);
		}
	}
}

int processRow(int index)
{
	long i;
	long result = 0;
	for (i = 0; i < LX; i++)
		result = result + matrix[i][index];
	return result;
}

int nextWorker()
{
	if (currentWorker >= numWorkers)
		currentWorker = 0;
	currentWorker++;
	return currentWorker;
}

void recvResults()
{
	long count = LX;
	long i, index, w;
	currentWorker = 0;
	for (i = 0; i < LY; i++)
	{
		w = nextWorker();
		MPI_Recv(&index, 1, MPI_LONG, w, FROM_MASTER, MPI_COMM_WORLD, &status);
		if (index != -1)
		{
			MPI_Recv(&vec_sum[index], 1, MPI_LONG, w, FROM_MASTER, MPI_COMM_WORLD, &status);
		}
	}
}

void fillMatrix()
{
	long i, j;
	long val;
	int prob;
	for (i = 0; i < LX; i++)
		for (j = 0; j < LY; j++)
		{
			val = rand() % 256;
			matrix[i][j] = val;
		}
}

void main(int argc, char **argv)
{
	double start;
	initMPI(argc, argv);
	start = MPI_Wtime();
	if (taskId == MASTER)
	{
		fillMatrix();
		sendRows();
		recvResults();
		printf("TOTAL time: %lf\n", MPI_Wtime() - start);
	}
	else
	{
		recvRows();
	}
	MPI_Finalize();
}
