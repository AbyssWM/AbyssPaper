<div align="center">

<img src="https://github.com/AbyssWM.png?size=200" width="160" alt="Abyss logo">

# AbyssPaper

**AbyssPaper – wallpaper daemon for Abyssland, fork of hyprpaper (BSD-3, © hyprwm/vaxerski)**

Part of [**Abyss**](https://github.com/AbyssWM) · Based on [hyprpaper](https://github.com/hyprwm/hyprpaper) by [hyprwm](https://github.com/hyprwm)

</div>

---

> [!WARNING]
> **Early development.** Abyss is being rebranded and reworked right now. Nothing is ready for daily use yet.

## About

AbyssPaper is a simple and fast wallpaper utility for [Abyssland](https://github.com/AbyssWM/Abyssland) that can change wallpapers at runtime over a socket (`abyssctl abysspaper ...`).

- Per-output wallpapers
- `cover`, `contain`, `tile` and `fill` modes
- Fractional scaling support
- IPC (`abysspaper_core`) for fast wallpaper switches

Abyssland also ships built-in wallpapers (`misc:force_default_wallpaper`). AbyssPaper is meant for your own images. Video, GIF and audio wallpapers are planned, but **not implemented yet**.

## Configuration

`~/.config/abyss/abysspaper.conf`:

```ini
wallpaper {
    monitor =
    path = ~/Pictures/wall.png
    fit_mode = cover
}
```

IPC socket: `$XDG_RUNTIME_DIR/abyss/$ABYSSLAND_INSTANCE_SIGNATURE/.abysspaper.sock`.

## Installation

Everything in the Abyss ecosystem installs to `/opt/abyss`, independent of system packages. From the Abyss workspace:

```sh
sh tools/build.sh AbyssPaper
```

The binary is `/opt/abyss/bin/abysspaper`.

### Dependencies

Development files of: Abysstoolkit, Abyssparse, Abyssutils, Abysswire, Abyss-scanner, wayland-client, pixman, libdrm.

### Manual build

```sh
export PKG_CONFIG_PATH=/opt/abyss/lib/pkgconfig:/opt/abyss/share/pkgconfig
export CMAKE_PREFIX_PATH=/opt/abyss
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/abyss
cmake --build build -j2
cmake --install build
```

## The Abyss ecosystem

| Repository | Based on |
|---|---|
| [Abyssland](https://github.com/AbyssWM/Abyssland) | [Hyprland](https://github.com/hyprwm/Hyprland) |
| [Abyssal](https://github.com/AbyssWM/Abyssal) | [aquamarine](https://github.com/hyprwm/aquamarine) |
| [Abyssutils](https://github.com/AbyssWM/Abyssutils) | [hyprutils](https://github.com/hyprwm/hyprutils) |
| [Abyssparse](https://github.com/AbyssWM/Abyssparse) | [hyprlang](https://github.com/hyprwm/hyprlang) |
| [Abysscursor](https://github.com/AbyssWM/Abysscursor) | [hyprcursor](https://github.com/hyprwm/hyprcursor) |
| [Abyssgraphics](https://github.com/AbyssWM/Abyssgraphics) | [hyprgraphics](https://github.com/hyprwm/hyprgraphics) |
| [Abysswire](https://github.com/AbyssWM/Abysswire) | [hyprwire](https://github.com/hyprwm/hyprwire) |
| [Abyss-scanner](https://github.com/AbyssWM/Abyss-scanner) | [hyprwayland-scanner](https://github.com/hyprwm/hyprwayland-scanner) |
| [Abyssland-protocols](https://github.com/AbyssWM/Abyssland-protocols) | [hyprland-protocols](https://github.com/hyprwm/hyprland-protocols) |
| [Xdg-desktop-portal-abyss](https://github.com/AbyssWM/Xdg-desktop-portal-abyss) | [xdg-desktop-portal-hyprland](https://github.com/hyprwm/xdg-desktop-portal-hyprland) |
| **AbyssPaper** | [hyprpaper](https://github.com/hyprwm/hyprpaper) |

## Credits

AbyssPaper exists thanks to **[hyprpaper](https://github.com/hyprwm/hyprpaper)**, created by **vaxerski** and **Hypr Development** at **[hyprwm](https://github.com/hyprwm)**. All credit for the original work goes to them – please support the original project.

Abyss is an independent project and is **not affiliated with or endorsed by hyprwm**.

## License

This project is licensed under the **BSD 3-Clause License**. The original copyright notice is kept in [`LICENSE`](LICENSE) as required.

## Contributors

- [bb8TheRobot](https://github.com/bb8TheRobot)
- Claude (Anthropic) – AI pair programmer
