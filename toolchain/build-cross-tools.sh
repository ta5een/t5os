#!/usr/bin/env bash

set -e
set -o pipefail

ARCH=${ARCH:-i686}
OS_NAME="$(uname -s)"
MAKE_JOBS=${MAKE_JOBS:-$(nproc)}
DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

TARBALLS_DIR="$DIR/tarballs"
BUILD_DIR_PREFIX="$DIR/build"
LOCAL_DIR_PREFIX="$DIR/local"

GCC_VERSION="14.1.0"
GCC_NAME="gcc-${GCC_VERSION}"
GCC_TARBALL="${GCC_NAME}.tar.xz"
GCC_BASE_URL="https://ftpmirror.gnu.org/gnu/gcc/${GCC_NAME}"
GCC_MD5SUM="24195dca80ded5e0551b533f46a4481d"

BINUTILS_VERSION="2.42"
BINUTILS_NAME="binutils-${BINUTILS_VERSION}"
BINUTILS_TARBALL="${BINUTILS_NAME}.tar.xz"
BINUTILS_BASE_URL="https://ftp.gnu.org/gnu/binutils"
BINUTILS_MD5SUM="a075178a9646551379bfb64040487715"

STEP_DEPENDENCIES="dependencies"
STEP_BINUTILS="binutils"
STEP_GCC="gcc"
STEP_LIBGCC="libgcc"

# Exported for the newly-built cross tools to use
export PREFIX="$LOCAL_DIR_PREFIX/$ARCH"
export TARGET="$ARCH-elf"
export PATH="$PREFIX/bin:$PATH"

function log {
  step=$1
  shift
  "$@" 2>&1 | sed $'s|^|\e[35m['"${step}"$']\e[0m |'
}

# Check if toolchain has been built previously
# TODO: Expose this as a "clean" or "rebuild" subcommand instead
if [[ -d "$PREFIX" && -n $(ls -A "$PREFIX") ]]; then
  printf \
    $'You already have a toolchain built for \e[33m%s\e[0m in the following directory:\n' \
    "$ARCH" \
    >&2
  printf "\n  $PREFIX\n\n" >&2
  echo "Do you want to remove it and rebuild again?" >&2
  select yn in "Yes" "No"; do
    case $yn in
      Yes ) echo "Okay, build will continue..."; break;;
      No ) echo "Cancelled"; exit 1;;
    esac
  done

  rm -rf "$BUILD_DIR_PREFIX/$ARCH"
  rm -rf "$LOCAL_DIR_PREFIX/$ARCH"
fi

# Download binutils and gcc tarballs
mkdir -p "$TARBALLS_DIR"
pushd "$TARBALLS_DIR" > /dev/null
  # Does the binutils tarball already exist?
  if [ -f "$BINUTILS_TARBALL" ]; then
    log $STEP_DEPENDENCIES echo "Binutils already downloaded, validating MD5 checksum..."
    # Does the MD5 checksum match? If not, it may have been corrupted.
    if ! md5sum -c <<< "$BINUTILS_MD5SUM $BINUTILS_TARBALL"; then
      log $STEP_DEPENDENCIES echo "'$BINUTILS_TARBALL' MD5 does not match. Removing..."
      rm -f "$BINUTILS_TARBALL"
      # In case the extracted directory is present, remove it too
      rm -f "$BINUTILS_NAME"
    else
      log $STEP_DEPENDENCIES echo "Checksum passed, continuing..."
    fi
  fi

  # Download binutils if it isn't downloaded (or has been recently removed due
  # to a checksum failure in the previous if-statement)
  if [ ! -f "$BINUTILS_TARBALL" ]; then
    log $STEP_DEPENDENCIES echo "Downloading Binutils $BINUTILS_VERSION..."
    curl -LO "$BINUTILS_BASE_URL/$BINUTILS_TARBALL"
  fi

  # Extract binutils into the same directory
  if [ ! -d "$BINUTILS_NAME" ]; then
    log $STEP_DEPENDENCIES echo "Extracting Binutils $BINUTILS_VERSION..."
    tar -xjf "$BINUTILS_TARBALL"
  fi
popd > /dev/null # "$TARBALLS_DIR"

# Build and install binutils and gcc
mkdir -p "$BUILD_DIR_PREFIX/$ARCH"
mkdir -p "$PREFIX" # aka: "$LOCAL_DIR_PREFIX/$ARCH"
pushd "$BUILD_DIR_PREFIX/$ARCH" > /dev/null
  mkdir -p binutils
  pushd binutils > /dev/null
    log "$STEP_BINUTILS/configure" echo "Configuring Binutils $BINUTILS_VERSION..."
    log "$STEP_BINUTILS/configure" "$TARBALLS_DIR/$BINUTILS_NAME/configure" \
      --target="$TARGET" \
      --prefix="$PREFIX" \
      --with-sysroot \
      --disable-nls \
      --disable-werror || exit 1

    log "$STEP_BINUTILS/build" echo "Building binutils $BINUTILS_VERSION..."
    log "$STEP_BINUTILS/build" make -j "$MAKE_JOBS" || exit 1

    log "$STEP_BINUTILS/install" echo "Installing binutils $BINUTILS_VERSION..."
    log "$STEP_BINUTILS/install" make install || exit 1
  popd > /dev/null # binutils

  mkdir -p gcc
  pushd gcc > /dev/null
    log "$STEP_GCC/configure" echo "Configuring GCC $GCC_VERSION..."
    log "$STEP_GCC/configure" "$TARBALLS_DIR/$GCC_NAME/configure" \
      --target="$TARGET" \
      --prefix="$PREFIX" \
      --disable-nls \
      --enable-languages=c,c++ \
      --without-headers || exit 1

    log "$STEP_GCC/build" echo "Building gcc $GCC_VERSION..."
    log "$STEP_GCC/build" make -j "$MAKE_JOBS" all-gcc || exit 1

    log "$STEP_GCC/install" echo "Installing gcc $GCC_VERSION..."
    log "$STEP_GCC/install" make install-gcc || exit 1

    log "$STEP_LIBGCC/build" echo "Building libgcc $GCC_VERSION..."
    log "$STEP_LIBGCC/build" make -j "$MAKE_JOBS" all-target-libgcc || exit 1

    log "$STEP_LIBGCC/install" echo "Installing libgcc $GCC_VERSION..."
    log "$STEP_LIBGCC/install" make install-target-libgcc || exit 1
  popd > /dev/null # gcc
popd > /dev/null # "$BUILD_DIR_PREFIX/$ARCH"
