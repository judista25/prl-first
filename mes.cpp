#include <iostream>
#include <fstream>
#include <mpi.h>
#include <climits>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = (size + 1) / 2;
    int infinity = INT_MAX;

    // 1. VÝPIS VSTUPNÍ POSLOUPNOSTI (pouze rank 0)
    if (rank == 0) {
        std::ifstream file("numbers", std::ios::binary);
        if (file.is_open()) {
            for (int i = 0; i < n; ++i) {
                unsigned char b;
                file.read(reinterpret_cast<char*>(&b), 1);
                std::cout << (int)b << (i == n - 1 ? "" : " ");
            }
            std::cout << std::endl;
            file.close();
        } else {
            std::cerr << "Chyba: Nelze otevrit soubor numbers" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // 2. Extrakce pro každý prvek vzestupně
    for (int i = 0; i < n; ++i) {
        // --- LIST ---
        if (rank >= n - 1) {
            static int my_value = -1;
            if (my_value == -1) {
                std::ifstream file("numbers", std::ios::binary);
                file.seekg(rank - (n - 1));
                unsigned char buffer;
                file.read(reinterpret_cast<char *>(&buffer), 1);
                my_value = static_cast<int>(buffer);
                file.close();
            }


            MPI_Send(&my_value, 1, MPI_INT, (rank - 1) / 2, 0, MPI_COMM_WORLD);
            // 4ek8m ne6 budu minimální hodnota
            int winner_flag;
            MPI_Recv(&winner_flag, 1, MPI_INT, (rank - 1) / 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Pokud jsem byl vybrán, příště posílám nekonečno aby neovlivňoval další kola
            if (winner_flag == 1) my_value = infinity;

        } else {
            // --- porovnávací uzly ---
            int left_child = 2 * rank + 1;
            int right_child = 2 * rank + 2;
            int left_val, right_val;

            MPI_Recv(&left_val, 1, MPI_INT, left_child, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&right_val, 1, MPI_INT, right_child, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            int win_val = (left_val <= right_val) ? left_val : right_val;

            // Pošli minimum nahoru
            if (rank > 0) {
                MPI_Send(&win_val, 1, MPI_INT, (rank - 1) / 2, 0, MPI_COMM_WORLD);
            } else {
                // Kořen vypíše aktuální minimum 
                std::cout << win_val << std::endl;
            }

            
            int winner_from_above = 1; 
            if (rank > 0) {
                MPI_Recv(&winner_from_above, 1, MPI_INT, (rank - 1) / 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            // Propaguj informaci o minimu správnému childu
            int left_winner = 0, right_winner = 0;
            if (winner_from_above == 1) {
                if (left_val <= right_val) left_winner = 1;
                else right_winner = 1;
            }
            MPI_Send(&left_winner, 1, MPI_INT, left_child, 0, MPI_COMM_WORLD);
            MPI_Send(&right_winner, 1, MPI_INT, right_child, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}