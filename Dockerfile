FROM debian:12.5-slim

RUN apt-get update && apt-get install -y \
    bison \
    build-essential \
    flex \
    grub-common \
    grub-pc-bin \
    libgmp3-dev \
    libisl-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    xorriso \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /t5os
