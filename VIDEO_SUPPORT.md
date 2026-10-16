# AbyssPaper Video/GIF Support

AbyssPaper now supports video wallpapers (MP4, WebM, MKV, GIF, APNG) with hardware acceleration and optional audio.

## Features

- **Video Formats**: MP4, WebM, MKV, AVI, MOV, GIF, APNG, and more
- **Hardware Decoding**: Automatic detection of vaapi (AMD/Intel) and nvdec (NVIDIA)
- **Rendering Modes**:
  - **Stufe B (OpenGL)**: Zero-copy rendering via EGL/GLES3 with dmabuf (default)
  - **Stufe A (Software)**: CPU-based fallback via wl_shm (when OpenGL fails)
- **Audio Support**: Optional audio playback via PipeWire/PulseAudio (disabled by default)
- **Performance**: Low CPU usage with hardware decoding (~3% for 1080p30 H.264)
- **Auto-detection**: Automatically detects video files via libmagic

## Configuration

### Basic Video Wallpaper

```hyprlang
wallpaper {
    monitor = DP-1
    path = ~/Videos/wallpaper.mp4
    fit_mode = cover
}
```

### Video with Audio

```hyprlang
wallpaper {
    monitor = DP-1
    path = ~/Videos/animated.mp4
    audio = true  # Enable audio (default: false)
}
```

### Animated GIF

```hyprlang
wallpaper {
    monitor = eDP-1
    path = ~/Pictures/cool.gif
    fit_mode = contain
}
```

### Mixed Image/Video Slideshow

```hyprlang
wallpaper {
    monitor = HDMI-A-1
    path = ~/Wallpapers/  # Mix of images and videos
    timeout = 30
    order = random-shuffle
}
```

## Requirements

### Build Dependencies

```bash
sudo xbps-install -Sy mpv-devel libva-devel libglvnd-devel libgbm-devel libdrm-devel
```

### Runtime

- **libmpv** (2.5.0+)
- **EGL/GLES3** (Mesa or proprietary drivers)
- **Hardware decoder**:
  - AMD/Intel: Mesa with libva
  - NVIDIA: nvidia-drm.modeset=1, libnvidia-egl-wayland, nvdec/vaapi
- **Audio** (optional): PipeWire or PulseAudio

## Hardware Acceleration

### AMD/Intel (VAAPI)

Works out-of-the-box with Mesa:

```bash
# Check if vaapi is available
vainfo
```

### NVIDIA (RTX 40/50 Series)

Ensure driver ≥545 with modeset enabled:

```bash
# Add to kernel parameters
nvidia-drm.modeset=1

# Install EGL Wayland
sudo xbps-install -Sy libnvidia-egl-wayland

# Check nvdec support
nvidia-smi --query-gpu=decoder.max_video_capacity --format=csv
```

## Performance Tuning

AbyssPaper automatically applies these optimizations (from `plan-abysspaper-video.md`):

- `hwdec=auto-safe`: Auto-detect hardware decoder
- `demuxer-max-bytes=16MiB`: Limit buffer size
- `cache=no`: Disable disk cache
- `vd-lavc-threads=2`: Limit decode threads
- `framedrop=vo`: Drop frames if needed

## Troubleshooting

### Video not playing

Check logs:

```bash
abysspaper --verbose 2>&1 | grep -E "(mpv|video|Media)"
```

### Fallback to software rendering

If you see `"falling back to software rendering"`, OpenGL init failed. Check:

```bash
# EGL availability
eglinfo

# Wayland EGL
wlinfo
```

### No hardware acceleration

```bash
# Check MPV hwdec support
mpv --hwdec=auto-safe --vo=gpu your_video.mp4

# AMD/Intel: Install libva
sudo xbps-install -Sy libva-utils mesa-vaapi

# NVIDIA: Check driver
nvidia-smi
```

## Implementation Details

See `/mnt/Claudes-playground/vault/abyss/docs/plan-abysspaper-video.md` for technical details:

- **Video detection**: libmagic for MIME type detection
- **Decoder**: libmpv with Render API
- **Rendering**:
  - Stufe B: `MPV_RENDER_API_TYPE_OPENGL` → EGL context → wl_egl_window
  - Stufe A: `sw` render API → BGRA buffer → wl_shm
- **Audio**: `ao=pipewire` when enabled
- **Future**: Pause on fullscreen/covered, IPC control, shader effects

## Examples

### 4K Video on RTX 4090

```hyprlang
wallpaper {
    monitor = DP-1
    path = ~/Videos/4k_nature.mp4
    fit_mode = cover
}
```

Expected: <3% CPU, GPU decoder active, zero-copy rendering

### Looping GIF

```hyprlang
wallpaper {
    monitor = eDP-1
    path = ~/Pictures/chill_lofi.gif
    fit_mode = contain
}
```

### Multi-Monitor Setup

```hyprlang
wallpaper {
    monitor = DP-1
    path = ~/Videos/left_screen.webm
}

wallpaper {
    monitor = DP-2
    path = ~/Pictures/right_screen.jpg
}
```

## Known Limitations

- No pause-on-fullscreen yet (planned for v2)
- Audio only on one monitor at a time
- No IPC controls yet (pause/resume/mute/volume coming soon)
- Static images still use CImageElement (no mpv overhead)

## Contributing

Report issues or improvements to: https://github.com/AbyssWM/AbyssPaper
