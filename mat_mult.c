#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "get_walltime.h"

#include <omp.h>



void in_serial_multiplyMatrices(double **mat1, double **mat2, double **result, int rows, int cols)
{
int i,j,k;
for (i = 0; i < rows; ++i)
   {
      for (j = 0; j < cols; ++j)
      {

        for(k=0;k<cols;k++)
        {    result[i][j] += mat1[i][k] * mat2[k][j]; }
      }

   }


}


void in_parallel_multiplyMatrices(double **mat1, double **mat2, double **result, int rows, int cols)
{

//result, mat1, and mat2 are by default shared ; explicitly listed them for readability
int i,j,k; //need to list them as private since they are shared by default ; defined outside openmp's parallel loop
#pragma omp parallel for collapse(2) private(i,j,k) shared(result,mat1,mat2)
   for (i = 0; i < rows; ++i)
   {
      for (j = 0; j < cols; ++j)
      {

        for(k=0;k<cols;k++)
        {    result[i][j] += mat1[i][k] * mat2[k][j]; }
      }

   }


}



int main(int argc, char *argv[])
{

omp_set_num_threads(100);

int rows = 1000;
int cols = 1000;

double **arr1 = (double **)malloc(rows*sizeof(double *));
double **arr2 = (double **)malloc(rows*sizeof(double *));
double **result = (double **)malloc(rows*sizeof(double *));

int i=0;
int j = 0;

double **temp1 = arr1;
double **temp2 = arr2;
double **temp3 = result;


for(;i<rows;++i)
{
    *(arr1+i) = (double *)malloc(cols*sizeof(double));
    *(arr2+i) = (double *)malloc(cols*sizeof(double));
    *(result+i)  = (double *)malloc(cols*sizeof(double));
}


arr1 = temp1;
arr2 = temp2;
result = temp3;



//srand(time(NULL));
srand(3); // fix seed for verification test
  
for (i=0;i<rows;i++)
{
for (j=0;j<cols;j++)
{
   arr1[i][j] = rand() / (double) RAND_MAX;
   arr2[i][j] = rand() / (double) RAND_MAX;
   result[i][j] = 0;
}

}




double start_time, end_time;
get_walltime(&start_time);
in_parallel_multiplyMatrices(arr1,arr2,result, rows,cols);
get_walltime(&end_time);

printf("%f \n", end_time-start_time);



for(i=0;i<rows;++i,++arr1,++arr2,++result)
{ free(*arr1); free(*arr2); free(*result); }
arr1 = temp1;
arr2 = temp2;
result = temp3;

free(arr1);
free(arr2);
free(result);

return 0;

}
