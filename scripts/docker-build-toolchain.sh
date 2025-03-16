#!/bin/bash

set -e

# Check if Docker is available
hash docker 2>/dev/null || {
  echo "ERROR: 'docker' is not available" >&2
  exit 1
}

# Build the toolchain in an ephemeral container with the Docker image "t5os"
docker run -it --rm -v .:/t5os t5os bash toolchain/build.sh
