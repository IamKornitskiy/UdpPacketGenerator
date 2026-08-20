## En [Ru](README_RU.md)

<h1 align="center">UDP Packet Generator</h1>
<div align="center">
    <a href="https://github.com/UdpPacketGenerator/UdpPacketGenerator/releases"><img src="https://img.shields.io/github/v/release/UdpPacketGenerator/UdpPacketGenerator" alt="Latest Release"/></a>
    <a href="https://github.com/UdpPacketGenerator/UdpPacketGenerator/releases"><img src="https://img.shields.io/github/downloads/UdpPacketGenerator/UdpPacketGenerator/total" alt="Downloads"/></a>
    <a href="https://github.com/UdpPacketGenerator/UdpPacketGenerator/stargazers"><img src="https://img.shields.io/github/stars/UdpPacketGenerator/UdpPacketGenerator" alt="Stars"/></a>
    <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="License"/></a>
    <a href="https://www.qt.io/"><img src="https://img.shields.io/badge/Qt-6.8%2B-lightgray?logo=Qt&logoColor=white&labelColor=brightgreen" alt="Qt"/></a>
    <a href="#"><img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg?logo=c%2B%2B" alt="C++ Standard"/></a>
    <a href="https://github.com/UdpPacketGenerator/UdpPacketGenerator/actions/workflows/test.yaml"><img src="https://github.com/UdpPacketGenerator/UdpPacketGenerator/actions/workflows/test.yaml/badge.svg" alt="CI"/></a>
    <a href="#"><img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey" alt="Platform"/></a>
</div>
<p align="center">
    <img src="screenshots/UdpPacketGenerator.gif" width="600" alt="Demo"/>
    <br><b>A flexible UDP packet generator configured entirely through JSON templates.</b></br>
</p>

## About

**A tool for network protocol developers, testers, and engineers.**  
> Configure UDP packet generation using simple JSON templates — no recompilation required.

UDP Packet Generator creates and sends custom UDP packets based on a JSON template.  
Define packet fields with different types, set them as constants, counters, or user inputs.

### ✨ Why UDP Packet Generator?

- **🚀 Actively developing project** — regular releases, clear roadmap, community feedback
- **📦 Ready-to-use JSON templates** — check [`templates/`](templates/) folder for examples to get started quickly
- **⚡ No coding required** — define packets in JSON, no recompilation needed
- **🔄 Real-time editing** — change values while packets are being sent
- **🔧 Ready‑to‑use field types** — NMEA, CSV, JSON, counters, and more — with built‑in validation
- **💻 Cross-platform** — Windows and Linux with signed packages (deb, rpm, AppImage, zip)

## 📝 Example JSON Template

Here's a minimal example of a packet template:

```json
{
  "fields": [
    { "name": "id", "type": "uint32", "value": 1 },
    { "name": "temperature", "type": "float32", "value": 23.5 },
    { "name": "status", "type": "string", "value": "OK" }
  ]
}
```

This defines a packet with three fields:
- `id` — 32-bit unsigned integer
- `temperature` — 32-bit floating point
- `status` — UTF-8 string

You can also use `"input"` fields to let users edit values in the GUI, or `"counter"` fields for auto-incrementing values.

See the [`templates/`](templates/) folder for more examples and a detailed guide on writing your own templates.

If you create a JSON template for a well-known protocol or packet, feel free to submit a pull request — I'll be happy to add it to the collection.

## 💡 Use Cases

**Who can benefit from UDP Packet Generator?**

- **Network protocol testing** — simulate clients and servers without writing code
- **Load testing** — generate a stream of packets at a configurable interval
- **Embedded systems development** — send telemetry, NMEA, or JSON data to devices
- **Education** — learn UDP packet structure and network programming in practice

## 🗺 Roadmap

<details>
<summary><b>Click to expand</b></summary>

### ✅ Already implemented (v2.1.x)
- [x] Integer fields (`uint8`–`uint32`, `int8`–`int32`) with configurable byte order
- [x] Floating-point fields (`float32`, `float64`)
- [x] String field (`string`) with UTF-8 support
- [x] JSON field with automatic syntax validation
- [x] Auto-incrementing counter (`counter`)
- [x] Real-time value editing without stopping transmission
- [x] Cross-platform build (Windows, Linux) with signed packages (deb, rpm, AppImage, zip)
- [x] **CSV field** – column count validation
- [x] **Visual error highlighting** in text editors for invalid data
- [x] **NMEA field** with checksum validation
- [x] **Check for new version**

### 🚧 Under active development (v2.2.x)
- [ ] Packet duplication to multiple targets
- [ ] Save/load scenarios (field values)

### 🔜 Planned (v2.3.x and beyond)
- [ ] Visual packet editor – drag-and-drop field creation in GUI
- [ ] Base64 field for text-safe binary data
- [ ] Real-time traffic monitor (rate, counters, graph)
- [ ] Additional encodings (Latin-1, UTF-16)
- [ ] Plugin system for custom field types and validators
</details>

## 📸 Screenshots

<details>
<summary><b>Click to view</b></summary>
<table>
    <tr>
        <td><img src="screenshots/dark.png" alt="Dark theme"></td>
        <td><img src="screenshots/light.png" alt="Light theme"></td>
    </tr>
    <tr>
        <td><img src="screenshots/dark_blue.png" alt="Dark-blue theme"></td>
        <td><img src="screenshots/light_blue.png" alt="Light-blue theme"></td>
    </tr>
</table>
</details>

## Build

### Requirements

- **Qt 6.8+** (Core, Gui, Widgets, Network)
- **CMake 3.16+**
- **C++17 compiler** (MSVC 2019+, GCC 9+, Clang 10+)

```bash
git clone https://github.com/UdpPacketGenerator/UdpPacketGenerator.git
cd UdpPacketGenerator
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.8.0/gcc_64
cmake --build .
```

## Usage

1. **Load a JSON template** – click `Load` and select a `.json` file with packet description.
2. **Edit input fields** – if the template contains `input` fields, set their values in the right panel.
3. **Set destination** – enter target IP address and port.
4. **Set source (optional)** – specify local port, or leave `0` for automatic selection.
5. **Choose interval** – set the transmission period in milliseconds.
6. **Start** – press `Start` to begin sending packets.

## ❓ Support & Questions

- **Bug reports & feature requests** — open an [issue on GitHub](https://github.com/UdpPacketGenerator/UdpPacketGenerator/issues)
- **Discussions** — use [GitHub Discussions](https://github.com/UdpPacketGenerator/UdpPacketGenerator/discussions) for questions and ideas
- **Documentation** — check the [`templates/`](templates/) folder and [`whatsnew.md`](whatsnew.md) for updates

## 🙌 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

⭐ **Star this repository** if you find it useful — it helps others discover the project!

## License

This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.
