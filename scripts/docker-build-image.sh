#!/bin/bash

set -e

# Check if Docker is available
hash docker 2>/dev/null || {
  echo >&2 "ERROR: 'docker' is not available."
  exit 1
}

# Build the Dockerfile (in project root) with the tag "wyoos"
docker build -t wyoos .
