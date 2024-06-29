#!/bin/bash

set -e

# Check if Docker is available
hash docker 2>/dev/null || {
  echo "ERROR: 'docker' is not available" >&2
  exit 1
}

# Run the Docker image named "wyoos" in an ephemeral container to build the ISO
docker run -it --rm -v .:/wyoos wyoos make iso
