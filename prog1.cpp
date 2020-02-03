#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

double local_sum(double local_n, int seed, double local[], int rank);

int main (int argc, char * argv[]){
  if (argc != 3){
    printf("Number of arguments is not correct. Abort.\n");
    return 1;
  }
	

	int rank, size;

	MPI_Status state;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
  int root = 0;
  long int N;
  int C;
  if (rank == root){
    N = atoi(argv[1]);
    C = atoi(argv[2]);
  }
  MPI_Bcast(&N, 1, MPI_LONG, root, MPI_COMM_WORLD);
  MPI_Bcast(&C, 1, MPI_INT, root, MPI_COMM_WORLD);
  double time = MPI_Wtime();

  int seed = rank + C;
  long int local_n = N / size;
  double *local;
  local = (double *)malloc(sizeof(double)*local_n);
  if (local == NULL){
    printf("Memory allocation failed. Abort.\n");
    return 1;
  }
  for (int i = 0; i < local_n; i++){
    local[i] = 0.0;
  }


  double sum = local_sum(local_n, seed, local, rank);
  double temp;
  int target;

//summing
  int k = 1;
  while(k < size){
    if((rank & k) != 0){
      target = rank^k;

      MPI_Send(&sum, 1, MPI_DOUBLE,
        target, 0, MPI_COMM_WORLD);
    }else{
      target = rank^k;

      MPI_Recv(&temp, 1, MPI_DOUBLE,
        target, 0, MPI_COMM_WORLD, &state);
      sum = sum + temp;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    k = k*2;
  }

	if(rank == 0){
		time = MPI_Wtime() - time;
    FILE *f = fopen("output.txt", "a+");
    if (f == NULL){
      printf("Number of arguments is not correct. Abort.\n");
      return 1;
    }
    fprintf(f, "N= %ld, P = %d, C= %d, S= %f\nTime= %f\n", N, size, C, sum, time*1000.0);
    fclose(f);
	}
    
  MPI_Finalize();
  return 0;
}


double local_sum(double local_n, int seed, double local[], int rank){
  srand48(seed);
  double sum;
  for (int i = 0; i < local_n; i++){
    local[i] = drand48();
  }

  for (int i = 0; i < local_n; i++){
    sum = sum + local[i];
  }
  return sum;
}
