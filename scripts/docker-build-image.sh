#!/bin/bash

set -e

# Check if Docker is available
hash docker 2>/dev/null || {
  echo "ERROR: 'docker' is not available" >&2
  exit 1
}

# Build the Dockerfile (in project root) with the tag "wyoos"
docker build -t wyoos .
