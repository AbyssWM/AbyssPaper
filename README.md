<div align="center">

<img src="https://github.com/AbyssWM.png?size=200" width="140" alt="Abyss logo">

# 🖼️ AbyssPaper

**Wallpaper daemon for Abyssland.**

[![License](https://img.shields.io/badge/license-BSD--3--Clause-blue?style=flat-square)](LICENSE) [![Fedora COPR](https://img.shields.io/badge/fedora-COPR-51a2da?style=flat-square&logo=fedora&logoColor=white)](https://copr.fedorainfracloud.org/coprs/bb8therobot1717/Abyss/) [![Wayland](https://img.shields.io/badge/wayland-native-ffbc00?style=flat-square&logo=wayland&logoColor=black)](https://wayland.freedesktop.org/) [![C++](https://img.shields.io/badge/C%2B%2B-26-00599c?style=flat-square&logo=cplusplus&logoColor=white)](https://isocpp.org/) [![Stars](https://img.shields.io/github/stars/AbyssWM/AbyssPaper?style=flat-square&color=7c3aed)](https://github.com/AbyssWM/AbyssPaper/stargazers)

[🌐 Website](https://abysswm.github.io/) · [📖 Wiki](https://abysswm.github.io/wiki/getting-started) · [📦 Install](#-install) · [🌊 Ecosystem](#-the-abyss-ecosystem)

<sub>Part of <a href="https://github.com/AbyssWM"><b>Abyss</b></a> · based on <a href="https://github.com/hyprwm/hyprpaper">hyprpaper</a> by <a href="https://github.com/hyprwm">hyprwm</a></sub>

</div>

---

> [!WARNING]
> **First release.** Abyss is usable now, but this is the first version: expect bugs and please report them.

## ✨ About

AbyssPaper is a simple and fast wallpaper utility for [Abyssland](https://github.com/AbyssWM/Abyssland) that can change wallpapers at runtime over a socket (`abyssctl abysspaper ...`).

- Per-output wallpapers
- `cover`, `contain`, `tile` and `fill` modes
- Fractional scaling support
- IPC (`abysspaper_core`) for fast wallpaper switches
- Experimental media detection and libmpv prototype for MP4, WebM, MKV, GIF and APNG

**Video playback is unfinished:** the libmpv renderer is not connected to a Wayland surface or toolkit image element. OpenGL presentation and the software fallback are incomplete; video/GIF wallpapers are not currently usable. Hardware acceleration, audio output and performance have not been validated. Static PNG and WebP images use the existing image renderer.

Abyssland also ships built-in wallpapers (`misc:force_default_wallpaper`). AbyssPaper is meant for your own images and videos.

See [VIDEO_SUPPORT.md](VIDEO_SUPPORT.md) for video/GIF configuration and requirements.

## Configuration

`~/.config/abyss/abysspaper.conf`:

Runtime settings currently use the legacy format below. The build-time `#include.abyss <paper>` prototype only reports compiled config names; it does not apply wallpaper, audio or volume settings.

```ini
# Static image
wallpaper {
    monitor =
    path = ~/Pictures/wall.png
    fit_mode = cover
}

# Experimental video entry (renderer unfinished)
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

## 📦 Install

<details open>
<summary><b>Fedora 44 / 45</b> — COPR</summary>

```sh
sudo dnf copr enable bb8therobot1717/Abyss
sudo dnf install abysspaper
```
</details>

<details>
<summary><b>Arch Linux</b> — PKGBUILD</summary>

```sh
git clone https://github.com/AbyssWM/arch-builds.git
cd arch-builds/packages/abysspaper && makepkg -si
```
</details>

<details>
<summary><b>Gentoo</b> — overlay</summary>

```sh
eselect repository add abyss git https://github.com/AbyssWM/gentoo-builds.git
emaint sync -r abyss
emerge --ask gui-apps/abysspaper
```
</details>

<details>
<summary><b>Void Linux</b> — xbps-src template</summary>

Copy `srcpkgs/abysspaper` from [void-builds](https://github.com/AbyssWM/void-builds) into your `void-packages` checkout, then:

```sh
./xbps-src pkg abysspaper && sudo xbps-install -R hostdir/binpkgs abysspaper
```
</details>

<details>
<summary><b>Debian · Ubuntu · Linux Mint</b></summary>

Packaging lives in [debian-builds](https://github.com/AbyssWM/debian-builds) and [ubuntu-builds](https://github.com/AbyssWM/ubuntu-builds). A signed APT repository is on the way — see the [install guide](https://abysswm.github.io/wiki/getting-started) for the current status.
</details>

### 🔨 Build from source

Install the Abyss libraries it depends on first (see the [install guide](https://abysswm.github.io/wiki/getting-started)), then:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build
```

## 🌊 The Abyss ecosystem

<details>
<summary>All 17 repositories</summary>

| Repository | Role | Based on |
|---|---|---|
| 🪟 [Abyssland](https://github.com/AbyssWM/Abyssland) | compositor | [Hyprland](https://github.com/hyprwm/Hyprland) |
| 📚 [Abyssal](https://github.com/AbyssWM/Abyssal) | library | [aquamarine](https://github.com/hyprwm/aquamarine) |
| 📚 [Abyssutils](https://github.com/AbyssWM/Abyssutils) | library | [hyprutils](https://github.com/hyprwm/hyprutils) |
| 📚 [Abyssparse](https://github.com/AbyssWM/Abyssparse) | library | [hyprlang](https://github.com/hyprwm/hyprlang) |
| 📚 [Abysscursor](https://github.com/AbyssWM/Abysscursor) | library | [hyprcursor](https://github.com/hyprwm/hyprcursor) |
| 📚 [Abyssgraphics](https://github.com/AbyssWM/Abyssgraphics) | library | [hyprgraphics](https://github.com/hyprwm/hyprgraphics) |
| 📚 [Abysswire](https://github.com/AbyssWM/Abysswire) | library | [hyprwire](https://github.com/hyprwm/hyprwire) |
| 🛠️ [Abyss-scanner](https://github.com/AbyssWM/Abyss-scanner) | tool | [hyprwayland-scanner](https://github.com/hyprwm/hyprwayland-scanner) |
| 📜 [Abyssland-protocols](https://github.com/AbyssWM/Abyssland-protocols) | protocols | [hyprland-protocols](https://github.com/hyprwm/hyprland-protocols) |
| 🧩 [Xdg-desktop-portal-abyss](https://github.com/AbyssWM/Xdg-desktop-portal-abyss) | portal | [xdg-desktop-portal-hyprland](https://github.com/hyprwm/xdg-desktop-portal-hyprland) |
| 📚 [Abysstoolkit](https://github.com/AbyssWM/Abysstoolkit) | library | [hyprtoolkit](https://github.com/hyprwm/hyprtoolkit) |
| 🌙 [Abyss-guiutils](https://github.com/AbyssWM/Abyss-guiutils) | app | [hyprland-guiutils](https://github.com/hyprwm/hyprland-guiutils) |
| 🌙 [AbyssDusk](https://github.com/AbyssWM/AbyssDusk) | app | [hyprsunset](https://github.com/hyprwm/hyprsunset) |
| 🔒 [AbyssLock](https://github.com/AbyssWM/AbyssLock) | app | [hyprlock](https://github.com/hyprwm/hyprlock) |
| 🖼️ **AbyssPaper** | app | [hyprpaper](https://github.com/hyprwm/hyprpaper) |
| 🎨 [AbyssPicker](https://github.com/AbyssWM/AbyssPicker) | app | [hyprpicker](https://github.com/hyprwm/hyprpicker) |
| 🔐 [AbyssPolkit](https://github.com/AbyssWM/AbyssPolkit) | app | [hyprpolkitagent](https://github.com/hyprwm/hyprpolkitagent) |

</details>

## 💜 Credits

AbyssPaper exists thanks to **[hyprpaper](https://github.com/hyprwm/hyprpaper)**, created by **vaxerski** and **Hypr Development** at **[hyprwm](https://github.com/hyprwm)**. All credit for the original work goes to them – please support the original project.

Abyss is an independent project and is **not affiliated with or endorsed by hyprwm**.

## ⚖️ License

This project is licensed under the **BSD 3-Clause License**. The original copyright notice is kept in [`LICENSE`](LICENSE) as required.

## 👥 Contributors

- [bb8TheRobot](https://github.com/bb8TheRobot)
- Claude (Anthropic) – AI pair programmer

## 📦 Install

<details open>
<summary><b>Fedora 44 / 45</b> — COPR</summary>

```sh
sudo dnf copr enable bb8therobot1717/Abyss
sudo dnf install abysspaper
```
</details>

<details>
<summary><b>Arch Linux</b> — PKGBUILD</summary>

```sh
git clone https://github.com/AbyssWM/arch-builds.git
cd arch-builds/packages/abysspaper && makepkg -si
```
</details>

<details>
<summary><b>Gentoo</b> — overlay</summary>

```sh
eselect repository add abyss git https://github.com/AbyssWM/gentoo-builds.git
emaint sync -r abyss
emerge --ask gui-apps/abysspaper
```
</details>

<details>
<summary><b>Void Linux</b> — xbps-src template</summary>

Copy `srcpkgs/abysspaper` from [void-builds](https://github.com/AbyssWM/void-builds) into your `void-packages` checkout, then:

```sh
./xbps-src pkg abysspaper && sudo xbps-install -R hostdir/binpkgs abysspaper
```
</details>

<details>
<summary><b>Debian · Ubuntu</b> — .deb from Releases</summary>

Prebuilt `.deb` files for **Debian forky (testing)** and **Ubuntu 26.10** are attached to every [release](https://github.com/AbyssWM/AbyssPaper/releases/latest). Install the Abyss libraries first, then:

```sh
sudo apt install ./*.deb
```

Debian 13, Ubuntu 24.04/26.04 and Linux Mint 22 ship libraries that are too old for Abyssland — build from source there. Packaging: [debian-builds](https://github.com/AbyssWM/debian-builds) · [ubuntu-builds](https://github.com/AbyssWM/ubuntu-builds)
</details>

### 🔨 Build from source

Install the Abyss libraries it depends on first (see the [install guide](https://abysswm.github.io/wiki/getting-started)), then:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build
```

## 🌊 The Abyss ecosystem

<details>
<summary>All 17 repositories</summary>

| Repository | Role | Based on |
|---|---|---|
| 🪟 [Abyssland](https://github.com/AbyssWM/Abyssland) | compositor | [Hyprland](https://github.com/hyprwm/Hyprland) |
| 📚 [Abyssal](https://github.com/AbyssWM/Abyssal) | library | [aquamarine](https://github.com/hyprwm/aquamarine) |
| 📚 [Abyssutils](https://github.com/AbyssWM/Abyssutils) | library | [hyprutils](https://github.com/hyprwm/hyprutils) |
| 📚 [Abyssparse](https://github.com/AbyssWM/Abyssparse) | library | [hyprlang](https://github.com/hyprwm/hyprlang) |
| 📚 [Abysscursor](https://github.com/AbyssWM/Abysscursor) | library | [hyprcursor](https://github.com/hyprwm/hyprcursor) |
| 📚 [Abyssgraphics](https://github.com/AbyssWM/Abyssgraphics) | library | [hyprgraphics](https://github.com/hyprwm/hyprgraphics) |
| 📚 [Abysswire](https://github.com/AbyssWM/Abysswire) | library | [hyprwire](https://github.com/hyprwm/hyprwire) |
| 🛠️ [Abyss-scanner](https://github.com/AbyssWM/Abyss-scanner) | tool | [hyprwayland-scanner](https://github.com/hyprwm/hyprwayland-scanner) |
| 📜 [Abyssland-protocols](https://github.com/AbyssWM/Abyssland-protocols) | protocols | [hyprland-protocols](https://github.com/hyprwm/hyprland-protocols) |
| 🧩 [Xdg-desktop-portal-abyss](https://github.com/AbyssWM/Xdg-desktop-portal-abyss) | portal | [xdg-desktop-portal-hyprland](https://github.com/hyprwm/xdg-desktop-portal-hyprland) |
| 📚 [Abysstoolkit](https://github.com/AbyssWM/Abysstoolkit) | library | [hyprtoolkit](https://github.com/hyprwm/hyprtoolkit) |
| 🌙 [Abyss-guiutils](https://github.com/AbyssWM/Abyss-guiutils) | app | [hyprland-guiutils](https://github.com/hyprwm/hyprland-guiutils) |
| 🌙 [AbyssDusk](https://github.com/AbyssWM/AbyssDusk) | app | [hyprsunset](https://github.com/hyprwm/hyprsunset) |
| 🔒 [AbyssLock](https://github.com/AbyssWM/AbyssLock) | app | [hyprlock](https://github.com/hyprwm/hyprlock) |
| 🖼️ **AbyssPaper** | app | [hyprpaper](https://github.com/hyprwm/hyprpaper) |
| 🎨 [AbyssPicker](https://github.com/AbyssWM/AbyssPicker) | app | [hyprpicker](https://github.com/hyprwm/hyprpicker) |
| 🔐 [AbyssPolkit](https://github.com/AbyssWM/AbyssPolkit) | app | [hyprpolkitagent](https://github.com/hyprwm/hyprpolkitagent) |

</details>

## 💜 Credits

AbyssPaper exists thanks to **[hyprpaper](https://github.com/hyprwm/hyprpaper)**, created by **Hypr Development** at **[hyprwm](https://github.com/hyprwm)**. All credit for the original work goes to them – please support the original project.

Abyss is an independent project and is **not affiliated with or endorsed by hyprwm**.

## ⚖️ License

This project is licensed under the **BSD 3-Clause License**. The original copyright notice is kept in [`LICENSE`](LICENSE) as required.

## 👥 Contributors

- [bb8TheRobot](https://github.com/bb8TheRobot)

## 📦 Install

<details open>
<summary><b>Fedora 44 / 45</b> — COPR</summary>

```sh
sudo dnf copr enable bb8therobot1717/Abyss
sudo dnf install abysspaper
```
</details>

<details>
<summary><b>Arch Linux</b> — PKGBUILD</summary>

```sh
git clone https://github.com/AbyssWM/arch-builds.git
cd arch-builds/packages/abysspaper && makepkg -si
```
</details>

<details>
<summary><b>Gentoo</b> — overlay</summary>

```sh
eselect repository add abyss git https://github.com/AbyssWM/gentoo-builds.git
emaint sync -r abyss
emerge --ask gui-apps/abysspaper
```
</details>

<details>
<summary><b>Void Linux</b> — signed XBPS repo</summary>

```sh
echo "repository=https://abysswm.github.io/void" | sudo tee /etc/xbps.d/abyss.conf
sudo xbps-install -S abysspaper
```
</details>

<details>
<summary><b>Debian · Ubuntu</b> — .deb from Releases</summary>

Prebuilt `.deb` files for **Debian forky (testing)** and **Ubuntu 26.10** are attached to every [release](https://github.com/AbyssWM/AbyssPaper/releases/latest). Install the Abyss libraries first, then:

```sh
sudo apt install ./*.deb
```

Debian 13, Ubuntu 24.04/26.04 and Linux Mint 22 ship libraries that are too old for Abyssland — build from source there. Packaging: [debian-builds](https://github.com/AbyssWM/debian-builds) · [ubuntu-builds](https://github.com/AbyssWM/ubuntu-builds)
</details>

### 🔨 Build from source

Install the Abyss libraries it depends on first (see the [install guide](https://abysswm.github.io/wiki/getting-started)), then:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build
```

## 🌊 The Abyss ecosystem

<details>
<summary>All 17 repositories</summary>

| Repository | Role | Based on |
|---|---|---|
| 🪟 [Abyssland](https://github.com/AbyssWM/Abyssland) | compositor | [Hyprland](https://github.com/hyprwm/Hyprland) |
| 📚 [Abyssal](https://github.com/AbyssWM/Abyssal) | library | [aquamarine](https://github.com/hyprwm/aquamarine) |
| 📚 [Abyssutils](https://github.com/AbyssWM/Abyssutils) | library | [hyprutils](https://github.com/hyprwm/hyprutils) |
| 📚 [Abyssparse](https://github.com/AbyssWM/Abyssparse) | library | [hyprlang](https://github.com/hyprwm/hyprlang) |
| 📚 [Abysscursor](https://github.com/AbyssWM/Abysscursor) | library | [hyprcursor](https://github.com/hyprwm/hyprcursor) |
| 📚 [Abyssgraphics](https://github.com/AbyssWM/Abyssgraphics) | library | [hyprgraphics](https://github.com/hyprwm/hyprgraphics) |
| 📚 [Abysswire](https://github.com/AbyssWM/Abysswire) | library | [hyprwire](https://github.com/hyprwm/hyprwire) |
| 🛠️ [Abyss-scanner](https://github.com/AbyssWM/Abyss-scanner) | tool | [hyprwayland-scanner](https://github.com/hyprwm/hyprwayland-scanner) |
| 📜 [Abyssland-protocols](https://github.com/AbyssWM/Abyssland-protocols) | protocols | [hyprland-protocols](https://github.com/hyprwm/hyprland-protocols) |
| 🧩 [Xdg-desktop-portal-abyss](https://github.com/AbyssWM/Xdg-desktop-portal-abyss) | portal | [xdg-desktop-portal-hyprland](https://github.com/hyprwm/xdg-desktop-portal-hyprland) |
| 📚 [Abysstoolkit](https://github.com/AbyssWM/Abysstoolkit) | library | [hyprtoolkit](https://github.com/hyprwm/hyprtoolkit) |
| 🌙 [Abyss-guiutils](https://github.com/AbyssWM/Abyss-guiutils) | app | [hyprland-guiutils](https://github.com/hyprwm/hyprland-guiutils) |
| 🌙 [AbyssDusk](https://github.com/AbyssWM/AbyssDusk) | app | [hyprsunset](https://github.com/hyprwm/hyprsunset) |
| 🔒 [AbyssLock](https://github.com/AbyssWM/AbyssLock) | app | [hyprlock](https://github.com/hyprwm/hyprlock) |
| 🖼️ **AbyssPaper** | app | [hyprpaper](https://github.com/hyprwm/hyprpaper) |
| 🎨 [AbyssPicker](https://github.com/AbyssWM/AbyssPicker) | app | [hyprpicker](https://github.com/hyprwm/hyprpicker) |
| 🔐 [AbyssPolkit](https://github.com/AbyssWM/AbyssPolkit) | app | [hyprpolkitagent](https://github.com/hyprwm/hyprpolkitagent) |

</details>

## 💜 Credits

AbyssPaper exists thanks to **[hyprpaper](https://github.com/hyprwm/hyprpaper)**, created by **Hypr Development** at **[hyprwm](https://github.com/hyprwm)**. All credit for the original work goes to them – please support the original project.

Abyss is an independent project and is **not affiliated with or endorsed by hyprwm**.

## ⚖️ License

This project is licensed under the **BSD 3-Clause License**. The original copyright notice is kept in [`LICENSE`](LICENSE) as required.

## 👥 Contributors

- [bb8TheRobot](https://github.com/bb8TheRobot)
