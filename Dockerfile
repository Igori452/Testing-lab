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
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Добавляем репозиторий Mull
RUN curl -1sLf 'https://dl.cloudsmith.io/public/mull-project/mull-stable/setup.deb.sh' | bash

# Добавляем репозиторий LLVM для Clang-15
RUN curl -sSL https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /usr/share/keyrings/llvm-archive-keyring.gpg \
    && echo "deb [signed-by=/usr/share/keyrings/llvm-archive-keyring.gpg] http://apt.llvm.org/jammy/ llvm-toolchain-jammy-15 main" \
       > /etc/apt/sources.list.d/llvm-15.list

# Обновляем apt и устанавливаем все инструменты
RUN apt-get update && apt-get install -y \
    mull-15 \
    clang-15 \
    clang-tools-15 \
    llvm-15-dev \
    libc++-15-dev \
    libc++abi-15-dev \
    wget \
    gnupg2 \
    && rm -rf /var/lib/apt/lists/*

# Добавляем репозиторий PVS-Studio
RUN wget -q -O- https://cdn.pvs-studio.ru/etc/pubkey.txt | apt-key add - && \
    wget -O /etc/apt/sources.list.d/viva64.list https://cdn.pvs-studio.ru/etc/viva64.list

# Обновляем кэш и устанавливаем
RUN apt-get update && apt-get install -y \
    cppcheck \
    pvs-studio \
    && rm -rf /var/lib/apt/lists/*

# Вспомогательные утилиты
RUN apt-get update && apt-get install -y \
    cmake \
    make \
    git \
    gdb \
    valgrind \
    xxd \
    && rm -rf /var/lib/apt/lists/*

# Рабочая директория
WORKDIR /project

# По умолчанию запускаем bash
CMD ["/bin/bash"]