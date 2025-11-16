/**************************************************************************
--          PICO2 RISC-V multicore matrix multiplication 2D arrays 
-- 
--           Copyright (C) 2025 By Ulrik Hørlyk Hjort
--
--  This Program is Free Software; You Can Redistribute It and/or
--  Modify It Under The Terms of The GNU General Public License
--  As Published By The Free Software Foundation; Either Version 2
--  of The License, or (at Your Option) Any Later Version.
--
--  This Program is Distributed in The Hope That It Will Be Useful,
--  But WITHOUT ANY WARRANTY; Without Even The Implied Warranty of
--  MERCHANTABILITY or FITNESS for A PARTICULAR PURPOSE.  See The
--  GNU General Public License for More Details.
--
-- You Should Have Received A Copy of The GNU General Public License
-- Along with This Program; if not, See <Http://Www.Gnu.Org/Licenses/>.
***************************************************************************/
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>
#include "pico/multicore.h"
#include "hardware/structs/sio.h"
#include "uart_pio.h"

#define UART_TX_PIN 2

#define M 6   // rows of A
#define N 4   // cols of A / rows of B
#define P 5   // cols of B

int A[M][N];
int B[N][P];
int C[M][P];  


void print_matrix(const char *name,  int rows, int cols, int matrix[rows][cols]) {
    printf("%s (%d x %d):\r\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%4d ", matrix[i][j]);
        }
        printf("\r\n");
    }
    printf("\r\n");
}



// Core 1 computes the second half of rows
void core1_task() {
    for (int i = M/2; i < M; i++) {
        for (int j = 0; j < P; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    // Signal core 0 that computation is done
    multicore_fifo_push_blocking(1);
}

int main() {
	init_uart(19200,UART_TX_PIN);	

    // Initialize matrices A and B with example values
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            A[i][j] = i + j + 1;

    for (int i = 0; i < N; i++)
        for (int j = 0; j < P; j++)
            B[i][j] = i * j + 1;

    // Launch core 1
    multicore_launch_core1(core1_task);

    // Core 0 computes first half of rows
    for (int i = 0; i < M/2; i++) {
        for (int j = 0; j < P; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // Wait for core 1 to finish
    multicore_fifo_pop_blocking();

	print_matrix("A", M, N, A);
	print_matrix("B", N, P, B);
	print_matrix("C", M, P, C);
    return 0;
}

