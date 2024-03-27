#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <omp.h>

int main(int argc, char *argv[]){


int check_;
MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &check_);

omp_set_num_threads(10);
double start_,end_;

int rows = 2000;
int cols = 2000;

double **arr1 = (double **)malloc(rows*sizeof(double *));
double **arr2 = (double **)malloc(rows*sizeof(double*));
double **result = (double **)malloc(rows*sizeof(double *));

int i=0;
int j=0;

/*The 2d matrix is allocated on the heap rather than on the main function's stack. In our heap allocation,
  only single rows are contigious rather than all the rows being contigious as in 2d arrays
  allocated on the stack. For that purpose, it's difficult to come up with an mpi custom datatype
  to transfer more than a single row in one mpi send/recv call. 
*/
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
//if(j<10 && i==0){ printf("%f \n",arr1[i][j]);}
result[i][j] = 0;
}
}
}
for(i=0;i<rows;i++)
{
MPI_Bcast(arr2[i],cols,MPI_DOUBLE,0,MPI_COMM_WORLD);
}



int rs_ = rows / size_group;

if(u_number==0)
{
start_ = MPI_Wtime();
int cnt_ = rs_;

i=1; // size_group ranks
while(i<size_group)
{
for(j=0;j<rs_;j++)
{
MPI_Send(arr1[cnt_],cols,MPI_DOUBLE,i,cnt_,MPI_COMM_WORLD);
cnt_++;
}
i++;
}
}

else
{
// receive from 0th rank

int get_ = u_number * rs_; //starting index

for(i=get_;i<get_+rs_;i++)
{
MPI_Recv(arr1[i],cols,MPI_DOUBLE,0,i,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
}

}



{
int get_ = u_number * rs_; //starting ind
int k;
#pragma omp parallel for collapse(2) private(i,j,k) shared(result,arr1,arr2)
for (i = get_; i < get_+rs_; ++i)
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

i=1; // size_group ranks
while(i<size_group)
{
for(j=0;j<rs_;j++)
{
//MPI_Send(arr1[cnt_],cols,MPI_DOUBLE,i,cnt_,MPI_COMM_WORLD);
MPI_Recv(result[cnt_],cols,MPI_DOUBLE,i,cnt_,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
cnt_++;
}
i++;
}
}
else
{
int get_ = u_number * rs_; //starting index

for(i=get_;i<get_+rs_;i++)
{
MPI_Send(result[i],cols,MPI_DOUBLE,0,i,MPI_COMM_WORLD);
//MPI_Recv(arr1[i],cols,MPI_DOUBLE,0,i,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
}
}

MPI_Barrier(MPI_COMM_WORLD);
//printf("here \n");
if(u_number==0)
{
end_ = MPI_Wtime();
sleep(3);
printf("here \n");

for(i=4;i<6;i++)
{
for(j=0;j<10;j++)
{
printf("%f \n",result[i][j]);
}
}
double ov_time = end_ - start_;
printf("%f \n",ov_time);
}

MPI_Finalize();

return 0;

}
