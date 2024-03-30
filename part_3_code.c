#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <omp.h>

int main(int argc, char *argv[]){


//MPI_Init(&argc, &argv);
int check_;
MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &check_);

omp_set_num_threads(100);
double start_,end_;

int rows = 4000;
int cols = 4000;

//allocating memory for rows
double **arr1 = (double **)malloc(rows*sizeof(double *));
double **arr2 = (double **)malloc(rows*sizeof(double*));
double **result = (double **)malloc(rows*sizeof(double *));

int i=0;
int j=0;

//allocating memory for columns
for(;i<rows;i++)
{
*(arr1+i) = (double *)malloc(cols*sizeof(double));
arr2[i] = (double *)malloc(cols*sizeof(double));
*(result + i) = (double *)malloc(cols*sizeof(double));
}


srand(3);
int u_number;
MPI_Comm_rank(MPI_COMM_WORLD , &u_number);
int size_group;
MPI_Comm_size(MPI_COMM_WORLD , &size_group);

for(i=0;i<rows;i++)
{
for(j=0;j<cols;j++)
{
arr1[i][j] =0; // rand() / (double) RAND_MAX;
arr2[i][j] = 0; //rand() / (double) RAND_MAX;
result[i][j] = 0;
}
}


if(u_number==0)
{
//only rank 0 generates the values
for(i=0;i<rows;i++)
{
for(j=0;j<cols;j++)
{
arr1[i][j] = rand() / (double) RAND_MAX;
arr2[i][j] = rand() / (double) RAND_MAX;
result[i][j] = 0;
}
}
}

if(u_number==0)
{
start_ = MPI_Wtime();
}
for(i=0;i<rows;i++)
{
//broadcast each row; arr2[i] holds address of initial
//element of ith row.
MPI_Bcast(arr2[i],cols,MPI_DOUBLE,0,MPI_COMM_WORLD);
}


int rs_ = rows / size_group;
int rem_ = rows % size_group;

int st_;
int ed_;

if(u_number==0)
{

int cnt_ = rs_;

if(u_number < rem_)
{cnt_++;
}


int init_ = cnt_;
int all_size;
if(u_number < rem_)
{all_size = ((size_group-1) * rs_)+rem_-1;}
else
{ all_size = ((size_group-1) * rs_); }
MPI_Request reqst[all_size];


st_ = 0;
ed_ = cnt_;

i=1; 
while(i<size_group)
{
for(j=0;j<rs_;j++)
{
MPI_Isend(arr1[cnt_],cols,MPI_DOUBLE,i,cnt_,MPI_COMM_WORLD,&reqst[cnt_-init_]);
cnt_++;
}

if(i < rem_)
{ 
MPI_Isend(arr1[cnt_],cols,MPI_DOUBLE,i,cnt_,MPI_COMM_WORLD,&reqst[cnt_-init_]); cnt_++; }

i++;
}

MPI_Waitall((all_size),reqst,MPI_STATUS_IGNORE);

}



else
{
// receive from 0th rank

ed_ = rs_;

;
if(u_number < rem_)
{st_ = (u_number*rs_) + u_number; ed_ = ed_ + 1; }
else
{st_ = (u_number * rs_) + rem_;}

for(i=st_;i<st_+ed_;i++)
{
MPI_Recv(arr1[i],cols,MPI_DOUBLE,0,i,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
}

}



{
int k;
#pragma omp parallel for collapse(2) private(i,j,k) shared(result,arr1,arr2)
for (i = st_; i < st_+ed_; ++i)
{
  for (j = 0; j < cols; ++j)
    {

       for(k=0;k<cols;k++)
        {    result[i][j] += arr1[i][k] * arr2[k][j]; }
    }

}

}

if(u_number==0)
{
int cnt_ = rs_;

if(u_number < rem_)
{cnt_++;}


i=1; 
while(i<size_group)
{
for(j=0;j<rs_;j++)
{
MPI_Recv(result[cnt_],cols,MPI_DOUBLE,i,cnt_,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
cnt_++;
}

if(i < rem_)
{ MPI_Recv(result[cnt_],cols,MPI_DOUBLE,i,cnt_,MPI_COMM_WORLD,MPI_STATUS_IGNORE); cnt_++; }
i++;
}
}
else
{

for(i=st_;i<st_+ed_;i++)
{
MPI_Send(result[i],cols,MPI_DOUBLE,0,i,MPI_COMM_WORLD);
}
}

MPI_Barrier(MPI_COMM_WORLD);
if(u_number==0)
{
end_ = MPI_Wtime();



double ov_time = end_ - start_;
printf("%f \n",ov_time);
}



MPI_Finalize();

return 0;

}

