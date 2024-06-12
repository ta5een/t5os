FROM debian:12.5

RUN apt-get update                      && \
    apt-get install -y build-essential  && \
    apt-get install -y xorriso

WORKDIR /wyoos
