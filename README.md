# `t5os`

`t5os` is a small hobby operating system written in C.

## Getting Started

### Prerequisites

This project supports native and cross-platform compilation with Docker. To
build the project locally, you will need the following tools for your platform:

- Project compilation tools:
  - [GNU GCC and Binutils dependencies][gnu-gcc-binutils-deps]
  - [`bear`][bear-gh] (optional)
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
    target. By default, `$ARCH` is set to `i686` (i.e., x86 32-bit).

    > **NOTE**: This process will take a while to complete, depending on your
    > machine's specifications. Once complete, the resulting build artefacts
    > will take up around 3GB of disk space, give or take half a GB. Make sure
    > you have enough disk space (and patience) for this step :)

1. Build the kernel with `bear` and `make`:

    > **NOTE:** I am in the process of migrating from `bear` to `cmake`, mostly
    > due to the fact that `cmake` supports cross-compilers with
    > project-relative paths. This step is subject to change once the migration
    > is complete.

    ```sh
    bear -- make kernel
    ```

    Running with `bear` provides IDE/LSP support by generating a compilation
    database every time you build the project. This is beneficial if you use
    Clang as your LSP provider in your editor.

    By running the above command, `bear` should have generated a *non-empty*
    `compile_commands.json` file in the project root. If this is not the case,
    please refer to [this section](#bear-generates-empty-compile_commandsjson).

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
> Since integrating a custom GCC cross-compiler into the project, I've found
> that building with Docker fails if I previously compiled the toolchain on my
> host system. This is an issue if, for example, I compiled the toolchain on
> macOS, as the resulting binaries would be in the Mach-O format. This would
> cause Docker to fail as Debian doesn't (at least natively) support Mach-O
> executables.

This is the recommended method if you would like to play around with the
project and don't want to install all the required dependencies. It is also
possible to partially [build the project natively](#linux-and-macos) and run
the rest of the build process with Docker, thanks to the power of [bind
mounts][docker-bind-mounts].

1. Clone the repository:

    ```sh
    git clone https://github.com/ta5een/t5os.git
    ```

1. Build the Docker image:

    ```sh
    bash ./scripts/docker-build-image.sh
    ```

1. Build the `.iso` image:

    ```sh
    bash ./scripts/docker-make-iso.sh
    ```

    This will bind the current working directory as a volume in the Docker
    image and build the `.iso` image in a Linux environment. Once completed,
    `./build/t5os.iso` will be available in your local filesystem.

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
in this project:

- [OSDev Wiki](https://wiki.osdev.org/Expanded_Main_Page)
- [The little book about OS development](https://littleosbook.github.io)
- [Write Your Own Operating System (YouTube Playlist by Viktor Engelmann)](https://www.youtube.com/playlist?list=PLHh55M_Kq4OApWScZyPl5HhgsTJS9MZ6)

## License

This project is licensed under the GNU General Public License v3.0. See
[License](./LICENSE) for more details.

Additionally, many parts of this project, from its implementation to its
behaviour, have been derived from multiple open-source projects. Wherever
possible, I've added an attribution comment on top of the relevant
file/function/line in the source code. For reference, here is a list of
projects I've taken inspiration from (I highly recommend checking them out):

- [SerenityOS](https://github.com/SerenityOS/serenity)
- [RetrOS-32](https://github.com/joexbayer/RetrOS-32)
- [SkiftOS](https://github.com/skift-org/skift)
- [banan-os](https://git.bananymous.com/Bananymous/banan-os)
- [opuntiaOS](https://github.com/opuntiaOS-Project/opuntiaOS/)

If for any reason you believe I have used your work and haven't credited you
and/or abided by your license(s), please feel free to reach out to me :)

## Troubleshooting

### `bear` generates empty `compile_commands.json`

The main reason why this may happen is because you are using a cross-compiler.
While `bear` works great with the platform's native toolchain, it does not
recognise cross-compilers by default (e.g., `i686-elf-g++` on macOS).

To fix this, you'll need to create a link to the wrapper with the same name as
the cross-compiler. On macOS, this can be done by running the following
(substituting `<version>` with your installed `bear` version number):

```sh
ln -s /usr/local/Cellar/bear/<version>/lib/bear/wrapper /usr/local/Cellar/bear/<version>/lib/bear/wrapper.d/i686-elf-g++
```

This fix was sourced from [this issue comment on `bear`'s GitHub
repository][bear-gh-issue-comment].

[bear-gh-issue-comment]: https://github.com/rizsotto/Bear/issues/561#issuecomment-1921214908
[bear-gh]: https://github.com/rizsotto/Bear
[docker-bind-mounts]: https://docs.docker.com/storage/bind-mounts/
[gnu-gcc-binutils-deps]: https://wiki.osdev.org/GCC_Cross-Compiler#Installing_Dependencies
