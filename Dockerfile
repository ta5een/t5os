FROM debian:12.5-slim

RUN apt-get update && apt-get install -y \
    bison \
    build-essential \
    flex \
    libgmp3-dev \
    libisl-dev \
    libmpc-dev \
    libmpfr-dev \
    python3-pip \
    texinfo \
    && rm -rf /var/lib/apt/lists/*

# We don't mind installing these Python packages as root
# NOTE: ninja 1.12.0+ is currently not available in PyPi
RUN python3 -m pip install --break-system-packages \
    'meson>=1.5.0,<1.6.0' \
    'ninja>=1.11.0,<1.12.0'

WORKDIR /t5os
