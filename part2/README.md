Part 2 Report (Placeholder to avoid git conflicts)

5. Let "N" be the number of ranks for the call to mpiexec and let "n" be the number set by the environment variable OMP_NUM_THREADS. The output for the hybrid OpenMP-MPI "Hello World" program prints the statement "Hello World!" n * N times. However, unlike with pure MPI, the endline characters are inconsistent and often have multiple "Hello World!"s on one line with multiple new line characters in between them. This can be explained by the fact that each thread in OpenMP is using the same output buffer (stdout) and is competing for that resource. Therefore, since the "Hello World!" and the endline character are put into the output buffer (std::cout) separately, they can get separated in execution. Interestingly, this does not apply if the newline character is added at the end of the string literal instead of as "std::endl".