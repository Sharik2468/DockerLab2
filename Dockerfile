FROM python:3.9-slim

WORKDIR /app

# Устанавливаем только необходимые пакеты и сразу очищаем кэш apt
RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ \
    libomp-dev \
    curl \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

# Копируем только requirements.txt сначала
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Копируем только исходники C++ и компилируем
COPY src /app/src
RUN g++ -fPIC -shared -fopenmp -o /app/libheat_solver.so /app/src/heat_solver.cpp && \
    chmod 755 /app/libheat_solver.so

# Копируем только необходимые файлы приложения
COPY app /app/app

# Устанавливаем переменную окружения для библиотеки
ENV LD_LIBRARY_PATH=/app:$LD_LIBRARY_PATH

EXPOSE 5000

CMD ["python", "app/server.py"]