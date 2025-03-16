#!/usr/bin/env bash

# This script was heavily derived from the Serenity OS project:
# https://github.com/SerenityOS/serenity/blob/311af9ad0590970e3e3c7384feed67c63aed08f4/Toolchain/BuildGNU.sh

set -e
set -o pipefail

OS_NAME="$(uname -s)"

# Before doing anything, first check if we're on macOS with GNU coreutils
# installed. If so, we'll modify PATH to prioritize GNU coreutils over native
# utilities for platform consistency.
if [ "$OS_NAME" = "Darwin" ]; then
  if command -v brew > /dev/null; then
    BREW_COREUTILS_PREFIX=$(brew --prefix coreutils 2>/dev/null)
    if [ -n "$BREW_COREUTILS_PREFIX" ]; then
      export PATH="$BREW_COREUTILS_PREFIX/libexec/gnubin:$PATH"
    fi
  fi
fi

ARCH=${ARCH:-i686}
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

# Exported for the newly-built toolchain to use
export PREFIX="$LOCAL_DIR_PREFIX/$ARCH"
export TARGET="$ARCH-elf"
export PATH="$PREFIX/bin:$PATH"

function log {
  step=$1
  shift
  "$@" 2>&1 | sed $'s|^|\e[35m['"${step}"$']\e[0m |'
}

function ask_yes_no {
  select yn in "Yes" "No"; do
    case $yn in
      Yes ) echo "Okay, build will continue..."; break;;
      No ) echo "Cancelled"; exit 1;;
    esac
  done
}

# Check if toolchain has been built previously
# TODO: Expose this as a "clean" or "rebuild" subcommand instead
if [[ -d "$PREFIX" && -n $(ls -A "$PREFIX") ]]; then
  printf \
    $'You already have a toolchain built for \e[33m%s\e[0m in the following directory:\n' \
    "$ARCH" \
    >&2
  printf "\n  $PREFIX\n\n" >&2
  echo "Do you want to remove it and rebuild again? Enter '1' or '2':" >&2
  ask_yes_no

  rm -rf "$BUILD_DIR_PREFIX/$ARCH"
  rm -rf "$LOCAL_DIR_PREFIX/$ARCH"
fi

disk_space_available=$(df -h "$DIR" | tail -n1 | awk '{ print $4 }')
echo "\
This build step will download the source code for the GNU GCC $GCC_VERSION \
compiler and the GNU Binutils $BINUTILS_VERSION binary tools. It will then \
compile these tools, specified to target $ARCH-elf.

This process will take a while to complete, depending on your machine's \
specifications. Once complete, the resulting toolchain (downloaded tarballs, \
source code, and build artifacts) will take up around 3GB of disk space. \

You are on a(n) $OS_NAME system with $MAKE_JOBS processing unit(s) available \
and $disk_space_available of disk space available.

Are you sure you want to continue? Enter '1' or '2':" | fold -s -w 80
ask_yes_no

if command -v md5 > /dev/null; then
  MD5="md5"
  MD5_AWK_STR='{ print $NF }'
elif command -v md5sum > /dev/null; then
  MD5="md5sum"
  MD5_AWK_STR='{ print $1 }'
else
  echo "ERR: Neither md5 nor md5sum is available on this system."
  exit 1
fi

# Download binutils and gcc tarballs
mkdir -p "$TARBALLS_DIR"
pushd "$TARBALLS_DIR" > /dev/null
  # Does the binutils tarball already exist?
  if [ -f "$BINUTILS_TARBALL" ]; then
    log $STEP_DEPENDENCIES echo "Binutils already downloaded, validating MD5 checksum..."
    # Does the MD5 checksum match? If not, it may have been corrupted.
    if [ "$($MD5 "$BINUTILS_TARBALL" | awk "$MD5_AWK_STR")" != "$BINUTILS_MD5SUM" ]; then
      log $STEP_DEPENDENCIES echo "'$BINUTILS_TARBALL' MD5 does not match. Removing..."
      rm -f "$BINUTILS_TARBALL"
      # In case the extracted directory is present, remove it too
      rm -rf "$BINUTILS_NAME"
    else
      log $STEP_DEPENDENCIES echo "Checksum passed, continuing..."
    fi
  fi

  # Download binutils if it isn't available
  if [ ! -f "$BINUTILS_TARBALL" ]; then
    log $STEP_DEPENDENCIES echo "Downloading Binutils $BINUTILS_VERSION..."
    curl -LO "$BINUTILS_BASE_URL/$BINUTILS_TARBALL"
  else
    log $STEP_DEPENDENCIES echo "Binutils $BINUTILS_VERSION already downloaded, continuing..."
  fi

  # Extract binutils into the same directory
  if [ ! -d "$BINUTILS_NAME" ]; then
    log $STEP_DEPENDENCIES echo "Extracting Binutils $BINUTILS_VERSION..."
    tar -xJf "$BINUTILS_TARBALL"
  fi

  # Does the gcc tarball already exist?
  if [ -f "$GCC_TARBALL" ]; then
    log $STEP_DEPENDENCIES echo "GCC already downloaded, validating MD5 checksum..."
    # Does the MD5 checksum match? If not, it may have been corrupted.
    if [ "$($MD5 "$GCC_TARBALL" | awk "$MD5_AWK_STR")" != "$GCC_MD5SUM" ]; then
      log $STEP_DEPENDENCIES echo "'$GCC_TARBALL' MD5 does not match. Removing..."
      rm -f "$GCC_TARBALL"
      # In case the extracted directory is present, remove it too
      rm -rf "$GCC_NAME"
    else
      log $STEP_DEPENDENCIES echo "Checksum passed, continuing..."
    fi
  fi

  # Download gcc if it isn't available
  if [ ! -f "$GCC_TARBALL" ]; then
    log $STEP_DEPENDENCIES echo "Downloading GCC $GCC_VERSION..."
    curl -LO "$GCC_BASE_URL/$GCC_TARBALL"
  else
    log $STEP_DEPENDENCIES echo "GCC $GCC_VERSION already downloaded, continuing..."
  fi

  # Extract gcc into the same directory
  if [ ! -d "$GCC_NAME" ]; then
    log $STEP_DEPENDENCIES echo "Extracting GCC $GCC_VERSION..."
    tar -xJf "$GCC_TARBALL"
  fi

  # Download gcc prerequisites
  pushd $GCC_NAME > /dev/null
    log $STEP_DEPENDENCIES echo "Downloading GCC $GCC_VERSION prerequisites..."
    sh ./contrib/download_prerequisites
  popd > /dev/null # "$GCC_TARBALL"
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

echo "Toolchain built and installed successfully"
