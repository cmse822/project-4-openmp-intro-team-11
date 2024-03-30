#include <mpi.h>
#include <omp.h>
#include <iostream>
using namespace std;

int main(int argc, char** argv) 
{
    //MPI_Init(&argc, &argv);
    int required = omp_get_max_threads();
    int provided;
    //including this or MPI_Init both worked fine, probably mpi has no idea main thread is spawning threads (main thread only does mpi calls)
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED,&provided);
    #pragma omp parallel
    {
        cout << "Hello, World!" << endl;
    }
    MPI_Finalize();
    return 0;
}
