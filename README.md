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
- **🎬 Video wallpapers** (MP4, WebM, MKV, GIF, APNG) with hardware acceleration
- **🔊 Optional audio** via PipeWire/PulseAudio (disabled by default)
- **⚡ Zero-copy rendering** via EGL/GLES3 (OpenGL) with vaapi/nvdec fallback

Abyssland also ships built-in wallpapers (`misc:force_default_wallpaper`). AbyssPaper is meant for your own images and videos.

See [VIDEO_SUPPORT.md](VIDEO_SUPPORT.md) for video/GIF configuration and requirements.

## Configuration

`~/.config/abyss/abysspaper.conf`:

```ini
# Static image
wallpaper {
    monitor =
    path = ~/Pictures/wall.png
    fit_mode = cover
}

# Video wallpaper (NEW!)
wallpaper {
    monitor = DP-1
    path = ~/Videos/animated.mp4
    fit_mode = cover
}

# GIF wallpaper
wallpaper {
    monitor = eDP-1
    path = ~/Pictures/loop.gif
    fit_mode = contain
}
```

IPC socket: `$XDG_RUNTIME_DIR/abyss/$ABYSSLAND_INSTANCE_SIGNATURE/.abysspaper.sock`.

**For video/GIF configuration and requirements, see [VIDEO_SUPPORT.md](VIDEO_SUPPORT.md).**

## Installation

Everything in the Abyss ecosystem installs to `/opt/abyss`, independent of system packages. From the Abyss workspace:

```sh
sh tools/build.sh AbyssPaper
```

The binary is `/opt/abyss/bin/abysspaper`.

### Dependencies

**Runtime:**
- Abysstoolkit, Abyssparse, Abyssutils, Abysswire
- wayland-client, pixman, libdrm
- **libmpv** (2.5.0+) – for video/GIF support
- **EGL/GLES3** (Mesa or proprietary drivers) – for hardware-accelerated video
- **libmagic** – for media type detection

**Build:**
Development files of all runtime dependencies plus: Abyss-scanner, mpv-devel, libva-devel, libglvnd-devel, libgbm-devel

```bash
# Void Linux
sudo xbps-install -Sy mpv-devel libva-devel libglvnd-devel libgbm-devel libdrm-devel
```

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
