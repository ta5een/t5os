# `t5os`

`t5os` is a small hobby operating system written in C.

## Getting Started

### Prerequisites

This project supports native and cross-platform compilation with Docker. To
build the project on your system (the *build machine*), you will need the
following tools for your platform:

- Project compilation tools:
  - [`meson`][meson-website]
  - [`ninja`][ninja-website]
  - [GNU GCC and Binutils dependencies][gnu-gcc-binutils-deps]
- GRUB tools (optional)[^1]:
  - `xorriso`
  - `grub-common`
  - `grub-pc-bin`
- Emulation software:
  - `qemu-system-i386`

[^1]: It is possible to run these tools with the provided Docker configuration.

### Building

#### Linux and macOS

This is the recommended method if you would like to contribute to the project.

1. Clone the repository:

    ```sh
    git clone https://github.com/ta5een/t5os.git
    ```

1. Build the GCC Cross-Compiler toolchain:

    ```sh
    make toolchain
    ```

    This build step will download the source code for the GNU GCC 14.1.0
    compiler and the GNU Binutils 2.42 binary tools. It will then compile these
    tools, with settings tweaked so that it can build programs for the `$ARCH`
    target from the *build machine*. By default, `$ARCH` is set to `i686`
    (i.e., x86 32-bit).

    The resulting binaries, headers, and archives will be placed inside the
    `toolchain` directory.

    > **NOTE**: This process will take a while to complete, depending on your
    > machine's specifications. Once complete, the resulting build artifacts
    > will take up around 3GB of disk space. Make sure you have enough disk
    > space (and patience) for this step :)

1. Set up a `build` directory with `meson`:

    ```sh
    meson setup build --cross-file ./meson/cross/i686-elf.ini
    ```

    Meson works by building projects out-of-source. This means that all files
    generated during the build are placed in a separate directory. It is thus
    possible to have multiple build directories, each with their own
    configurations.

    With the above command, we request Meson to set up a build directory (aptly
    named `build`) that is configured to target an i686 *host machine*. The
    provided cross build definition file informs Meson of the compiler and
    tools to be used when building for the selected architecture. Currently,
    only the i686 architecture is supported.

1. Build the kernel with `meson`:

    ```sh
    meson compile -C build
    ```

    Running the command above will start the build process with the new `build`
    directory we created in the previous step. If you named the build directory
    differently, make sure to change the name after the `-C` flag.

    To provide editor support with `clangd`, you must have built the kernel at
    least once. This is because Meson will generate a `compile_commands.json`
    file in the provided build folder, which is essential for `clangd` to work
    properly. Note that `clangd` searches for this file in [specific
    locations][clangd-compile-commands], so you may need to configure `clangd`
    if it can't find the file.

1. Build the `.iso` image:

    ```sh
    make iso
    ```

    This build step requires `xorriso`, `grub-common`, and `grub-pc-bin` to be
    available on your system. Alternatively, follow the [Docker instructions
    below](#docker) to build the `.iso` image without needing to install these
    tools locally.

1. Run the OS in `qemu`:

    ```sh
    make qemu
    ```

#### Docker

> [!NOTE]
> If you previously compiled the toolchain on a non-Linux system that doesn't
> natively produce ELF binaries, the toolchain will *NOT* be compatible with
> the Debian environment in Docker. This would certainly be the case if you ran
> `make toolchain` on macOS or Windows. If you would like to primarily build
> the kernel in a Docker container, please rebuild the toolchain as instructed
> in the steps below.

This is the recommended method if you would like to play around with the
project and don't want to install all the required dependencies. It is also
possible to first [build the kernel natively](#linux-and-macos) and then create
the ISO with Docker (which has access to the legacy GRUB tools), thanks to the
power of [bind mounts][docker-bind-mounts].

1. Clone the repository:

    ```sh
    git clone https://github.com/ta5een/t5os.git
    ```

1. Build the Docker image:

    ```sh
    bash ./scripts/docker-build-image.sh
    ```

1. Build the toolchain in a Debian environment:

    ```sh
    bash ./scripts/docker-make-toolchain.sh
    ```

    This command will spawn an ephemeral Docker container, bind the current
    working directory as a volume, and run the toolchain build process. By
    binding the current working directory, the container will have direct
    access to only this directory in your system and can make changes to it in
    a way that will be visible to you.

    This is important to note as if your system is *NOT* a Linux environment
    similar to Debian 12.5, the toolchain binaries that will be built will
    *NOT* be compatible with your system. This may not be a concern if you
    don't plan on using the toolchain outside the container, but if you do,
    consider [building the toolchain natively](#linux-and-macos) by following
    steps 1, 2, and 3.

    In any case, the resulting binaries, headers, and archives will be placed
    inside the `toolchain` directory.

    > **NOTE**: This process will take a while to complete, depending on your
    > machine's specifications. Once complete, the resulting build artifacts
    > will take up around 3GB of disk space. Make sure you have enough disk
    > space (and patience) for this step :)

1. Build the `.iso` image:

    ```sh
    bash ./scripts/docker-make-iso.sh
    ```

    Like the step before, this will build the `.iso` image inside (a different
    instance of) an ephemeral Docker container. Once completed,
    `./build/kernel/t5os.iso` will be available in your local filesystem.

1. Run the OS in `qemu`:

    ```sh
    make qemu
    ```

## Usage

- QEMU: Run `make qemu`
- VMware/VirtualBox/etc.: Build the `.iso` with `make iso` and boot the virtual
  machine with this disk image (via USB or CD/DVD).

## Online Resources

Here is a non-exhaustive list of online resources that have helped me immensely
in this project (in no particular order):

- [OSDev Wiki](https://wiki.osdev.org/Expanded_Main_Page)
- [The little book about OS development](https://littleosbook.github.io)
- [Building an OS (YouTube playlist by nanobyte)](https://youtube.com/playlist?list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN)

## License

This project is licensed under the GNU General Public License v3.0. See
[License](./LICENSE) for more details.

Additionally, many parts of this project, from its implementation to its
behavior, have been derived from multiple open-source projects. Wherever
possible, I've added an attribution comment on top of the relevant
file/function/line in the source code. For reference, here is a list of
projects I've taken inspiration from (in alphabetical order):

- [banan-os](https://git.bananymous.com/Bananymous/banan-os)
- [HelenOS](https://github.com/HelenOS/helenos)
- [nanobyte_os](https://github.com/nanobyte-dev/nanobyte_os)
- [opuntiaOS](https://github.com/opuntiaOS-Project/opuntiaOS)
- [RetrOS-32](https://github.com/joexbayer/RetrOS-32)
- [SerenityOS](https://github.com/SerenityOS/serenity)
- [SkiftOS](https://github.com/skift-org/skift)

If for any reason you believe I have used your work and haven't credited you
and/or abided by your license(s), please feel free to reach out to me :)

[clangd-compile-commands]: https://clangd.llvm.org/installation.html#compile_commandsjson
[docker-bind-mounts]: https://docs.docker.com/storage/bind-mounts/
[gnu-gcc-binutils-deps]: https://wiki.osdev.org/GCC_Cross-Compiler#Installing_Dependencies
[meson-website]: https://mesonbuild.com/
[ninja-website]: https://ninja-build.org/
