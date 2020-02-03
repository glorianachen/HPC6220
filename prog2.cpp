#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MPI_CHK(err) if (err != MPI_SUCCESS) return err

double local_sum(long int local_n,long int local_c,double local_nums[],double* p);

int main(int argc, char *argv[]) {

    if(argc!=3){
        fprintf(stderr,"argc invalid, must have N, C specified. aborting the program ... \n");
        exit(EXIT_FAILURE);
    }

    
    // int n;
    // int c;
    // n = atoi( argv[1] );
    // c = atoi( argv[2] );
    // int local_n = 
    // printf("local_n:%d,seed on master process c:%d\n",n,c);
    // printf("arg count: %d:\n" ,argc);
    // int i;
    // for (i=0; i<argc; i++){

    //     printf("arg num %d",i);
    //     printf("arg val: %s\n", argv[i]);
    // }
    int rank,size;
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int p = size;
    int err;
    // master process:
    const int root = 0;
    // int n,c;
    long int arg_buff[2];
    if (rank == root){
        arg_buff[0]= atoi(argv[1]);
        arg_buff[1] = atoi(argv[2]);

    }

    //printf("[%d]: before Bcast, number size is %d,master seeding number is %d\n", rank, arg_buff[0],arg_buff[1]);

    // all ranks start calling MPI_Bcast, root to all other ranks, assign a buff to send the argv, buff = [n,c], already converted to int type
    MPI_Bcast(&arg_buff, 2, MPI_DOUBLE, root, MPI_COMM_WORLD);
    // printf("done with MPI_Bcast");

    //start the timer
    double t_start = MPI_Wtime();
    // printf("check time start  %f", t_start);
    //after Bcast, cast values to variable n, c , and calculate each rank's new seed number 
    long int n;
    long int c;
    n = arg_buff[0];
    c = arg_buff[1];
    long int local_c;
    local_c = c+rank;

    // printf("local_c: %li, seed: %li \n", local_c,c);


    // printf("[%d]: After Bcast, number size is %li, new seed number is %li\n",rank, n, local_c);
    int d = log2(p);
    //printf("[%d]: is having %d dimensions \n",rank,d);
    // consider the N/P, N is not divisible by P cases, we need to assign the maximum numbers for a rank, so some may have one more number than the other.
    int local_n;
    int remainder = n%size;
    if (remainder != 0 && remainder >= rank+1){
        local_n = n/size + 1;
        //printf("[%d]: assigned %d size numbers to this processor\n",rank, local_n);
    }
    else{
        local_n = n/size;
        //printf("[%d]: assigned %d size numbers to this processor\n",rank, local_n);

    }
    
 

    // after calculated the local_c, the seed for each processor, and the local_n, the number size assigned to each processor. 
    // we need to generate the random numbers based on the local_c, local_n;
    //specify the request array size, based on local_n assigned to each processor
    double local_nums[local_n+1];
    //malloc the memory needed
    double* ptr = (double *) malloc ( sizeof(double) * local_n );
    if (ptr == NULL){
        printf("[%d]: Error! memory not allocated.\n",rank);
        exit(EXIT_FAILURE);

    }
    double local_s = local_sum(local_n,local_c,local_nums,ptr);
    //printf("[%d]: local sum is:%f\n",rank,local_s); 
    //free the ptr pointed malloc memory
    free(ptr);
    ptr = NULL;
    
    // double sum;
    //start send local sum in pairs 
    for( int j= 0; j < d; j++ ){
 
        int bit = pow(2, j);
     
        if ((rank & bit) !=0){
            // printf("j = %d with rank %d, send to %d\n",j,rank,(rank ^ bit));
            MPI_Send(&local_s, 1, MPI_DOUBLE,(rank ^ bit), 111, MPI_COMM_WORLD);
            MPI_Finalize();
            return 0;
        }
        else{
            MPI_Status stat;
            double local_s_received;
            MPI_Recv (&local_s_received, 1, MPI_DOUBLE, (rank ^ bit), 111, MPI_COMM_WORLD, &stat);
            local_s = local_s+local_s_received;
        }
    }
    


    // Get the name of the processor
    // char processor_name[MPI_MAX_PROCESSOR_NAME];
    // int name_len;
    // MPI_Get_processor_name(processor_name, &name_len);

    // // Print off a hello world message
    // printf("From machine %s: rank %d out of %d processors\n",
    //        processor_name, rank, p);
    
    if (rank==0){
    double sum = local_s;
    double t_end = MPI_Wtime();
    double t_running = t_end - t_start;
    printf("sum is: %f",sum);
    FILE *f = fopen("output.txt", "a+");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(EXIT_FAILURE);
    }

    // print results to output.txt
   
    fprintf(f, "N= %ld, P = %d, C= %ld, S= %f\nTime= %f\n", n,p,c,sum,t_running);

    fclose(f);
    }
    // Finalize the MPI environment.
    MPI_Finalize();

return 0;

}

double local_sum(long int local_n,long int local_c, double local_nums[],double* p){
    double local_s;
    srand48(local_c);
    int i;
    // double local_nums[local_n+1];
    // double* p = (double *) malloc ( sizeof(double) * local_n ); 

    p = local_nums;
    for (i=0; i<local_n; i++){
        local_nums[i] = drand48();
        // printf(" generating:%f \n",local_nums[i]);
        local_s = local_s + local_nums[i];
    }
    // free(p);
    // if (p != NULL){
    //     printf("check p address for array: %p\n",p);
    //     for (int i = 0; i < 5; ++i)
    //         printf("%f\n", *(p + i));

    // }
    return local_s;
}