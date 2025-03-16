# `t5os`

`t5os` is a small hobby operating system written in C.

## Getting Started

### Prerequisites

This project supports native and cross-platform compilation with Docker. To
build the project on your system (the *build machine*), you will need the
following tools for your platform:

  - [`meson v1.7.x`][meson-website]
  - [`ninja v1.12.x`][ninja-website]
  - [`qemu v9.2.x`][qemu-website]
  - [GNU GCC and Binutils dependencies][gnu-gcc-binutils-deps]

### Building

#### Linux and macOS

This is the recommended method if you would like to contribute to the project.

1. Clone the repository:

   ```sh
   git clone https://github.com/ta5een/t5os.git
   ```

1. `cd` into the cloned directory and build the GCC Cross-Compiler toolchain:

   ```sh
   bash ./toolchain/build-cross-tools.sh
   ```

   This build step will download the source code for GNU GCC 14.1.0 and GNU
   Binutils 2.42. It will then compile these tools, with customized settings so
   that it can build programs for the `$ARCH` target from the *build machine*.
   By default, `$ARCH` is set to `i686` (i.e., x86 32-bit).

   The resulting binaries, headers, and archives will be placed inside the
   `toolchain` directory.

   > **NOTE**: This process will take a while to complete, depending on your
   > machine's specifications. Once complete, the resulting build artifacts
   > will take up around 3 GB of disk space. Make sure you have enough disk
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
   provided cross build definition file informs Meson of the compiler and tools
   to be used when building for the selected architecture. Currently, only the
   i686 architecture is supported.

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

1. Run the OS in `qemu`:

   ```sh
   meson compile -C build qemu-run-image
   ```

#### Docker

> [!NOTE]
> If you previously compiled the toolchain on a non-Linux system that doesn't
> natively produce ELF binaries, the toolchain will *NOT* be compatible with
> the Debian environment in Docker. This would certainly be the case if you ran
> the build step on macOS or Windows. If you would like to primarily build the
> kernel in a Docker container, please rebuild the toolchain as instructed in
> the steps below.

This is the recommended method if you would like to play around with the
project and don't want to install all the required dependencies. Alternatively,
you may want to [build the kernel natively](#linux-and-macos).

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
   bash ./scripts/docker-build-toolchain.sh
   ```

   This command will spawn an ephemeral Docker container, bind the current
   working directory as a volume, and run the toolchain build process. By
   binding the current working directory, the container will have direct access
   to only this directory in your system and can make changes to it in a way
   that will be visible to you.

   This is important to note because if your build system is *NOT* a Linux
   environment compatible with Debian 12.5, the resulting toolchain binaries
   will *NOT* be compatible with your system. This may not be a concern if you
   don't plan on using the toolchain outside the container, but if you do,
   consider [building the toolchain natively](#linux-and-macos) instead of
   through Docker.

   In any case, the toolchain binaries, headers, and archives will be placed
   inside the `toolchain` directory.

   > **NOTE**: This process will take a while to complete, depending on your
   > machine's specifications. Once complete, the resulting build artifacts
   > will take up around 3GB of disk space. Make sure you have enough disk
   > space (and patience) for this step :)

1. Run the OS in `qemu`:

   ```sh
   meson compile -C build qemu-run-image
   ```

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
[qemu-website]: https://www.qemu.org/
