#ifndef HEAT_SOLVER_H
#define HEAT_SOLVER_H

// Заменяем Windows-специфичные макросы на Linux-совместимые
#define HEAT_API __attribute__((visibility("default")))

extern "C" {
    HEAT_API void initializeSolver(int gridSize, double timeStep, double diffusivity);
    HEAT_API void setBoundaryCondition(int face, double temperature);
    HEAT_API void runSimulation(int iterations);
    HEAT_API void runSimulationSequential(int iterations);
    HEAT_API double* getTemperatureData();
    HEAT_API void cleanupSolver();
    HEAT_API void resetSolver();
    HEAT_API void stepSimulation(int stepIndex);
}

#endif // HEAT_SOLVER_H