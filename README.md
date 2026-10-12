# abysspaper

Abysspaper is a simple and fast wallpaper utility for Abyssland with the ability to dynamically change wallpapers through sockets.

# Features
 - Per-output wallpapers
 - fill, tile, cover or contain modes
 - fractional scaling support
 - IPC for fast wallpaper switches

# Installation

[Arch Linux](https://archlinux.org/packages/extra/x86_64/abysspaper/): `pacman -S abysspaper`

[OpenSuse Linux](https://software.opensuse.org/package/abysspaper): `zypper install abysspaper`

## Manual:

### Dependencies
The development files of these packages need to be installed on the system for `abysspaper` to build correctly.
(Development packages are usually suffixed with `-dev` or `-devel` in most distros' repos).
- abysstoolkit
- abyssparse
- abyssutils
- abysswire

### Building

Building is done via CMake:

```sh
cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_INSTALL_PREFIX:PATH=/usr -S . -B ./build
cmake --build ./build --config Release --target abysspaper -j`nproc 2>/dev/null || getconf _NPROCESSORS_CONF`
```

Install with:

```sh
cmake --install ./build
```