#include <mpi.h>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    const std::vector<double> data = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 7.7, 6.6, 5.5, 4.4, 3.3, 2.2, 1.1};

    int color = world_rank % 3;

    MPI_Comm group_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &group_comm);

    int group_rank, group_size;
    MPI_Comm_rank(group_comm, &group_rank);
    MPI_Comm_size(group_comm, &group_size);

    int data_size = data.size();
    int chunk_size = data_size / group_size;
    int remainder = data_size % group_size;
    int start_idx = group_rank * chunk_size + std::min(group_rank, remainder);
    int end_idx = start_idx + chunk_size + (group_rank < remainder ? 1 : 0);
    double local_result = 0.0;
    double global_result = 0.0;

    if (color == 0) {
        for (int i = start_idx; i < end_idx; ++i) {
            local_result += data[i];
        }
        MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, group_comm);
        if (group_rank == 0) {
            std::cout << "[Group 0] Sum: " << global_result << " (" << group_size << " processes)" << std::endl;
        }
    } else if (color == 1) {
        if (start_idx < end_idx) {
            local_result = data[start_idx];
            for (int i = start_idx + 1; i < end_idx; ++i) {
                if (data[i] > local_result) local_result = data[i];
            }
        } else {
            local_result = -1e100;
        }
        MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_MAX, 0, group_comm);
        if (group_rank == 0) {
            std::cout << "[Group 1] Maximum: " << global_result << " (" << group_size << " processes)" << std::endl;
        }
    } else if (color == 2) {
        if (start_idx < end_idx) {
            local_result = data[start_idx];
            for (int i = start_idx + 1; i < end_idx; ++i) {
                if (data[i] < local_result) local_result = data[i];
            }
        } else {
            local_result = 1e100;
        }
        MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_MIN, 0, group_comm);
        if (group_rank == 0) {
            std::cout << "[Group 2] Minimum: " << global_result << " (" << group_size << " processes)" << std::endl;
        }
    }

    MPI_Comm_free(&group_comm);
    MPI_Finalize();
    return 0;
}
