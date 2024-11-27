#!/bin/bash

set -e

# Check if Docker is available
hash docker 2>/dev/null || {
  echo "ERROR: 'docker' is not available" >&2
  exit 1
}

print_usage() {
  printf "Usage: %s [-rh]\n" "$(basename "$0")"
  echo "Build Docker image 't5os', using the Dockerfile in project root"
  echo ""
  echo "Options:"
  echo "  -r: Rebuild the image from scratch, as opposed to from cache"
}

rebuild=false
while getopts "rh" flag; do
  case "$flag" in
    r) rebuild=true ;;
    h) print_usage; exit 0 ;;
    *) print_usage; exit 1 ;;
  esac
done

# Build the Dockerfile (in project root) with the tag "t5os"
if [ "$rebuild" = true ]; then
  docker build --no-cache -t t5os .
else
  docker build -t t5os .
fi
