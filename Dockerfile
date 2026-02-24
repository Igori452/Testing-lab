# Dockerfile для Mull-15 + Clang-15 на Ubuntu 22.04
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Обновляем систему и ставим зависимости
RUN apt-get update && apt-get install -y \
    curl \
    apt-transport-https \
    ca-certificates \
    gnupg \
    lsb-release \
    software-properties-common \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Добавляем репозиторий Mull
RUN curl -1sLf 'https://dl.cloudsmith.io/public/mull-project/mull-stable/setup.deb.sh' | bash

# Добавляем репозиторий LLVM для Clang-15
RUN curl -sSL https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /usr/share/keyrings/llvm-archive-keyring.gpg \
    && echo "deb [signed-by=/usr/share/keyrings/llvm-archive-keyring.gpg] http://apt.llvm.org/jammy/ llvm-toolchain-jammy-15 main" \
       > /etc/apt/sources.list.d/llvm-15.list

# Обновляем apt и устанавливаем Mull-15 и Clang-15
RUN apt-get update && apt-get install -y \
    mull-15 \
    clang-15 \
    clang-tools-15 \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y \
    cmake \
    make \
    git \
    && rm -rf /var/lib/apt/lists/*

# Проверка установки Mull и Clang
RUN mull-runner-15 --version || mull-cxx-15 --version
RUN clang-15 --version

# Рабочая директория
WORKDIR /project

# По умолчанию запускаем bash
CMD ["/bin/bash"]