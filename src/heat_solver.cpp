#include "heat_solver.h"
#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <cstring>

static int N;  // размер сетки
static double dt, alpha;  // шаг по времени и коэффициент диффузии
static double*** tempGrid;
static double*** tempNext;

// функция для выделения памяти под 3D-массив
double*** allocate3DArray(int size) {
	double*** array = (double***)calloc(size, sizeof(double**));
	for (int i = 0; i < size; i++) {
		array[i] = (double**)calloc(size, sizeof(double*));
		for (int j = 0; j < size; j++) {
			array[i][j] = (double*)calloc(size, sizeof(double));
		}
	}
	return array;
}

// функция для освобождения памяти из под 3D-массива
void free3DArray(double*** array, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			free(array[i][j]);
		}
		free(array[i]);
	}
	free(array);
}

extern "C" {

	void initializeSolver(int gridSize, double timeStep, double diffusivity) {
		N = gridSize;
		dt = timeStep;
		alpha = diffusivity;

		// выделяем память под 3D-массивы
		tempGrid = allocate3DArray(N);
		tempNext = allocate3DArray(N);

		// инициализируем массивы начальными значениями
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				for (int k = 0; k < N; k++) {
					tempGrid[i][j][k] = 25.0;
					tempNext[i][j][k] = 25.0;
				}
			}
		}
	}

	void setBoundaryCondition(int face, double temperature) {
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				for (int k = 0; k < N; ++k) {
					if (face == 0 && i == 0) tempGrid[i][j][k] = temperature;
					if (face == 1 && i == N - 1) tempGrid[i][j][k] = temperature;
					if (face == 2 && j == 0) tempGrid[i][j][k] = temperature;
					if (face == 3 && j == N - 1) tempGrid[i][j][k] = temperature;
					if (face == 4 && k == 0) tempGrid[i][j][k] = temperature;
					if (face == 5 && k == N - 1) tempGrid[i][j][k] = temperature;
				}
			}
		}
	}

	void runSimulationSequential(int iterations) {
		for (int iter = 0; iter < iterations; ++iter) {
			for (int i = 1; i < N - 1; ++i) {
				for (int j = 1; j < N - 1; ++j) {
					for (int k = 1; k < N - 1; ++k) {
						tempNext[i][j][k] = tempGrid[i][j][k] + alpha * dt * (
							(tempGrid[i + 1][j][k] - 2 * tempGrid[i][j][k] + tempGrid[i - 1][j][k]) +
							(tempGrid[i][j + 1][k] - 2 * tempGrid[i][j][k] + tempGrid[i][j - 1][k]) +
							(tempGrid[i][j][k + 1] - 2 * tempGrid[i][j][k] + tempGrid[i][j][k - 1])
							);
						tempGrid[i][j][k] = tempNext[i][j][k];
					}
				}
			}
		}
	}

	void runSimulation(int iterations) {
		omp_set_num_threads(12);

		for (int iter = 0; iter < iterations; ++iter) {
#pragma omp parallel for schedule(static)
			for (int i = 1; i < N - 1; ++i) {
				for (int j = 1; j < N - 1; ++j) {
					for (int k = 1; k < N - 1; ++k) {
						tempNext[i][j][k] = tempGrid[i][j][k] + alpha * dt * (
							(tempGrid[i + 1][j][k] - 2 * tempGrid[i][j][k] + tempGrid[i - 1][j][k]) +
							(tempGrid[i][j + 1][k] - 2 * tempGrid[i][j][k] + tempGrid[i][j - 1][k]) +
							(tempGrid[i][j][k + 1] - 2 * tempGrid[i][j][k] + tempGrid[i][j][k - 1])
							);
						tempGrid[i][j][k] = tempNext[i][j][k];
					}
				}
			}
		}
	}

	void stepSimulation(int stepIndex) {
		if (stepIndex <= 0 || stepIndex >= N - 1) return;

#pragma omp parallel for collapse(2)
		for (int j = 1; j < N - 1; ++j) {
			for (int k = 1; k < N - 1; ++k) {
				tempNext[stepIndex][j][k] = tempGrid[stepIndex][j][k] + alpha * dt * (
					(tempGrid[stepIndex + 1][j][k] - 2 * tempGrid[stepIndex][j][k] + tempGrid[stepIndex - 1][j][k]) +
					(tempGrid[stepIndex][j + 1][k] - 2 * tempGrid[stepIndex][j][k] + tempGrid[stepIndex][j - 1][k]) +
					(tempGrid[stepIndex][j][k + 1] - 2 * tempGrid[stepIndex][j][k] + tempGrid[stepIndex][j][k - 1])
					);
			}
		}

#pragma omp parallel for collapse(2)
		for (int j = 1; j < N - 1; ++j) {
			for (int k = 1; k < N - 1; ++k) {
				tempGrid[stepIndex][j][k] = tempNext[stepIndex][j][k];
			}
		}
	}

	double* getTemperatureData() {
		double* flatArray = (double*)malloc(N * N * N * sizeof(double));

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				memcpy(&flatArray[i * (N * N) + j * N], tempGrid[i][j], N * sizeof(double));
			}
		}

		return flatArray;
	}

	void cleanupSolver() {
		free3DArray(tempGrid, N);
		free3DArray(tempNext, N);
	}

	void resetSolver() {
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				for (int k = 0; k < N; ++k) {
					tempGrid[i][j][k] = 25.0;
					tempNext[i][j][k] = 25.0;
				}
			}
		}
	}
}
