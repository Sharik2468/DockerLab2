FROM python:3.9-slim

WORKDIR /app

# Установка компилятора и инструментов сборки
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    libomp-dev \
    file \
    binutils \
    curl \
    && rm -rf /var/lib/apt/lists/*

COPY requirements.txt .
RUN pip install -r requirements.txt

# Копируем исходники и компилируем
COPY src /app/src
RUN g++ -fPIC -shared -fopenmp -o /app/libheat_solver.so /app/src/heat_solver.cpp && \
    chmod 755 /app/libheat_solver.so && \
    ls -la /app/libheat_solver.so

# Копируем остальные файлы приложения
COPY . .

# Проверяем наличие библиотеки после копирования
RUN ls -la /app && \
    echo "Проверка библиотеки:" && \
    file /app/libheat_solver.so && \
    ldd /app/libheat_solver.so

# Добавляем путь для поиска библиотек
ENV LD_LIBRARY_PATH=/app:$LD_LIBRARY_PATH

EXPOSE 5000

# Запускаем с проверкой
CMD ["sh", "-c", "ls -la /app && echo 'LD_LIBRARY_PATH=' $LD_LIBRARY_PATH && python app/server.py"]