#!/bin/bash

set -e

# Check if Docker is available
hash docker 2>/dev/null || {
  echo "ERROR: 'docker' is not available" >&2
  exit 1
}

# With the Docker image "t5os", in an ephemeral container, build the toolchain
docker run -it --rm -v .:/t5os t5os bash toolchain/build-cross-tools.sh
