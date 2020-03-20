/**
 * @file    parallel_sort.cpp
 * @author  Patrick Flick <patrick.flick@gmail.com>
 * @brief   Implements the parallel, distributed sorting function.
 *
 * Copyright (c) 2014 Georgia Institute of Technology. All Rights Reserved.
 */

#include "parallel_sort.h"

void spliting(int *list, int size ,MPI_Comm);

// implementation of your parallel sorting
void parallel_sort(int * begin, int * end, MPI_Comm comm) {

  //read processor information
  int rank, p;
  MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &p);

  // generate local list
  int size = end-begin;
  int list[size];
  for (int i = 0; i < size; i++) {
    list[i] = begin[i];
  }

  spliting(&list[0], size, comm);

  std::cout << rank << "/" << p <<" Final:";
  for (int i = 0; i < size; i++) {
    std::cout << list[i] << " ";
  }
  std::cout << "\n";

  /*********
  Gather or ALltoall to collect list on each processor and point it to *begin
  *********/
  return;

}


/*********************************************************************
 *             Implement your own helper functions here:             *
 *********************************************************************/
void spliting(int *list, int size, MPI_Comm comm){

  // read communicator information
  int rank, p;
  MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &p);

  // recursive spliting
  if (p == 1) {
    // run local sort when p = 1
    std::sort(list,list+size);

    // print local list
    std::cout << "rank/p:" << rank << "/" << p << ", numbers on processor:";
    for (int i = 0; i < size; i++) {
      std::cout << list[i] << " ";
    }
    std::cout << "\n";

  }else{
    // print local list
    std::cout << "rank/p:" << rank << "/" << p << ", numbers on processor:";
    for (int i = 0; i < size; i++) {
      std::cout << list[i] << " ";
    }
    std::cout << "\n";

    // communicator spliting
    MPI_Comm div_comm;

    /*********
    Select pivot
    Boradcast
    Allgather subproblem and decide how to split communicator
    Alltoall data transfer
    *********/
    int color =rank %2;

    MPI_Comm_split(comm, color, rank, &div_comm);

    // recursive call
    spliting(&list[0], size, div_comm);

    // free memo
    MPI_Comm_free(&div_comm);

    std::cout << "rank/p:" << rank << "/" << p << ", numbers on processor:";
    for (int i = 0; i < size; i++) {
      std::cout << list[i] << " ";
    }
    std::cout << "\n";
  }
}
