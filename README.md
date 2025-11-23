# PICO 2 Multicore matrix multiplication using 2D arrays

Embarrassingly parallel matrix multiplication using the PICO2's two RISCV cores
(should work on the M33 cores as well,but not tested there). The rows are split
50-50 on the two cores. 2D arrays are used.

NOTE: The matrix multiplication here is not fully efficient across two cores because both cores share the same system bus and SRAM. 
During the heavy memory access here , the bus arbitration logic  causing contention and stalls. 
This reduces effective memory bandwidth and limits the expected speed-up.

	   
