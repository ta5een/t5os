# `wyoos`

`wyoos` is my implementation of a small operating system following the [Write
your own Operating System][wyoos-yt-playlist] YouTube playlist by Viktor
Engelmann.

## Getting Started

### Prerequisites

This project supports native and cross-platform compilation. To build on any
modern platform, it is recommended to use Docker. Otherwise, you will need the
following tools for your platform:

- Project tools:
  - `make`
  - `bear`
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

#### Linux

1. Clone the repository

    ```sh
    git clone https://github.com/ta5een/wyoos.git
    ```

1. Build the kernel with `bear` and `make`:

    ```sh
    bear -- make kernel
    ```

    The should generate a *non-empty* `compile_commands.json` in the project
    root. If this is not the case, refer to [this
    section](#bear-generates-empty-compile_commandsjson).

1. Build the `.iso` image:

    ```sh
    make iso
    ```

1. Run the OS in `qemu`:

    ```sh
    make qemu
    ```

#### macOS

1. Clone the repository

    ```sh
    git clone https://github.com/ta5een/wyoos.git
    ```

1. Build the kernel with Docker:

    ```sh
    docker compose up
    ```

    NOTE: You may also run `make kernel` to partially build natively and run
    the above command to build the `.iso` in a Linux environment.

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

[wyoos-yt-playlist]: https://www.youtube.com/playlist?list=PLHh55M_Kq4OApWScZyPl5HhgsTJS9MZ6M
[i386-elf-toolchain]: https://github.com/nativeos/homebrew-i386-elf-toolchain
[bear-gh-issue-comment]: https://github.com/rizsotto/Bear/issues/561#issuecomment-1921214908
