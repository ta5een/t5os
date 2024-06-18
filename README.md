# `wyoos`

`wyoos` is my implementation of a small operating system following the [Write
your own Operating System][wyoos-yt-playlist] YouTube playlist by Viktor
Engelmann.

## Getting Started

### Prerequisites

This project supports native and cross-platform compilation with Docker. To
build the project locally, you will need the following tools for your platform:

- Project tools:
  - `make`
  - [`bear`][bear-gh] (optional)
  - GNU 32-Bit ELF toolchain
    - macOS: [`i386-elf-toolchain`][i386-elf-toolchain] via Homebrew
    - Linux/WSL: `build-essential`
- `.iso` creation and GRUB tools (optional)[^1]:
  - `xorriso`
  - `grub-common`
  - `grub-pc-bin`
- Emulation software:
  - `qemu-system-i386`

[^1]: It is possible to run these tools with the provided Docker configuration.

### Building

#### Linux and macOS

This is the recommended method if you would like to contribute to the project.

1. Clone the repository

    ```sh
    git clone https://github.com/ta5een/wyoos.git
    ```

1. Build the kernel with `bear` and `make`:

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

    This build step requires `xorriso`, `grub-common` and `grub-pc-bin` to be
    available in your system. Alternatively, follow the [Docker instructions
    below](#docker) to build the `.iso` image without needing to install these
    tools locally.

1. Run the OS in `qemu`:

    ```sh
    make qemu
    ```

#### Docker

This is the recommended method if you would like to play around with the
project and don't want to install all the required dependencies. It is also
possible to partially [build the project natively](#linux-and-macos) and run
the rest of the build process with Docker, thanks to the power of [bind
mounts][docker-bind-mounts].

1. Clone the repository

    ```sh
    git clone https://github.com/ta5een/wyoos.git
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
    `./out/wyoos.iso` will be available in your local filesystem.

1. Run the OS in `qemu`:

    ```sh
    make qemu
    ```

## Usage

- QEMU: Run `make qemu`
- VMWare/VirtualBox/etc.: Build the `.iso` with `make iso` and boot the virtual
  machine with this disk image (via USB or CD/DVD).

## Troubleshooting

### `bear` generates empty `compile_commands.json`

This main reason why this may happen is because you are using a cross-compiler.
While `bear` works great with the platform's native toolchain, it does not
recognize cross-compilers by default (e.g. `i386-elf-g++` on macOS).

To fix this, you'll need to create a link to the wrapper with the same name as
the cross-compiler. On macOS, this can be done by running the following
(substituting `<version>` with your installed `bear` version number):

```sh
ln -s /usr/local/Cellar/bear/<version>/lib/bear/wrapper /usr/local/Cellar/bear/<version>/lib/bear/wrapper.d/i386-elf-g++
```

This fix was sourced from [this issue comment on `bear`'s GitHub repository][bear-gh-issue-comment].

[bear-gh-issue-comment]: https://github.com/rizsotto/Bear/issues/561#issuecomment-1921214908
[bear-gh]: https://github.com/rizsotto/Bear
[docker-bind-mounts]: https://docs.docker.com/storage/bind-mounts/
[i386-elf-toolchain]: https://github.com/nativeos/homebrew-i386-elf-toolchain
[wyoos-yt-playlist]: https://www.youtube.com/playlist?list=PLHh55M_Kq4OApWScZyPl5HhgsTJS9MZ6M