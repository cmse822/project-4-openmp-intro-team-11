#include <mpi.h>
#include <omp.h>
#include <iostream>
using namespace std;

int main(int argc, char** argv) 
{
    MPI_Init(&argc, &argv);
    int required = omp_get_max_threads();
    int provided;
    //MPI_Init_thread(&argc, &argv, required, &provided);
    #pragma omp parallel
    {
        cout << "Hello, World!" << endl;
    }
    MPI_Finalize();
    return 0;
}
