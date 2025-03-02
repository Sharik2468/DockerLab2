import ctypes
import numpy as np
import os
import sys

# Загружаем .so файл вместо .dll
def load_library():
    lib_path = os.path.join(os.path.dirname(os.path.dirname(__file__)), "libheat_solver.so")
    try:
        print(f"Пытаемся загрузить библиотеку из: {lib_path}")
        print(f"Текущая директория: {os.getcwd()}")
        print(f"Содержимое директории: {os.listdir(os.path.dirname(lib_path))}")
        print(f"LD_LIBRARY_PATH: {os.environ.get('LD_LIBRARY_PATH', 'не установлен')}")
        
        # Пробуем использовать абсолютный путь
        abs_path = os.path.abspath(lib_path)
        print(f"Абсолютный путь к библиотеке: {abs_path}")
        
        return ctypes.CDLL(abs_path)
    except Exception as e:
        print(f"Ошибка загрузки библиотеки: {e}")
        print(f"Системная информация:")
        print(f"Python версия: {sys.version}")
        print(f"OS: {os.name}")
        raise

heat_lib = load_library()

# Инициализация функций
heat_lib.initializeSolver.argtypes = [ctypes.c_int, ctypes.c_double, ctypes.c_double]
heat_lib.setBoundaryCondition.argtypes = [ctypes.c_int, ctypes.c_double]
heat_lib.runSimulation.argtypes = [ctypes.c_int]
heat_lib.getTemperatureData.restype = ctypes.POINTER(ctypes.c_double)

# Функции Python → C++
def initialize_solver(grid_size, time_step, diffusivity):
    heat_lib.initializeSolver(grid_size, time_step, diffusivity)

def set_boundary(face, temperature):
    heat_lib.setBoundaryCondition(face, temperature)

def run_simulation(iterations):
    heat_lib.runSimulation(iterations)

def get_temperature(grid_size):
    data_ptr = heat_lib.getTemperatureData()
    size = grid_size ** 3
    array = np.ctypeslib.as_array(data_ptr, shape=(size,))
    return array.reshape((grid_size, grid_size, grid_size))
