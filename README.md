## En [Ru](README_RU.md)

<h1 align="center">UDP Packet Generator</h1>
<p align="center">
  <img src="screenshots/UdpPacketGenerator.gif" width="600" alt="Demo"/>
  <br>A flexible UDP packet generator configured entirely through JSON templates.</br>
</p>


[![GitHub release](https://img.shields.io/github/v/release/IamKornitskiy/UdpPacketGenerator)](https://github.com/IamKornitskiy/UdpPacketGenerator/releases) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Qt 6.8+](https://img.shields.io/badge/Qt-6.8%2B-lightgray?logo=Qt&logoColor=white&labelColor=brightgreen
)](https://www.qt.io/)

## About

UDP Packet Generator creates and sends custom UDP packets based on a JSON template.  
Define packet fields with different types, set them as constants, counters, or user inputs.
<details>
<summary><b>🗺 Roadmap</b></summary>

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

<details>
<summary><b>Screenshots</b></summary>
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
git clone https://github.com/IamKornitskiy/UdpPacketGenerator.git
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

## JSON Template

Packet structure is defined in a JSON file.  
See [`templates/`](templates/) for examples and a detailed guide on writing your own templates.

If you create a JSON template for a well-known protocol or packet, feel free to submit a pull request — I'll be happy to add it to the collection.

## License

This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.
