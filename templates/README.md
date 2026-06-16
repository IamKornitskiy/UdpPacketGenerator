## En [Ru](README_RU.md)

# JSON Template Guide

This guide describes how to write JSON templates for **UDP Packet Generator**.
Templates define the structure of UDP packets — field names, types, sizes, and value sources.

To use a template, place the `.json` file in any location and load it from the application via the **Load** button.
Examples can be found in this folder.

For general project information, see the main [README](../README.md).

## Value Sources (`value_source`)

Each field must specify a `value_source` — where the field's data comes from.

| Source | Description |
|--------|-------------|
| `constant` | Fixed hex value from JSON. Not editable in GUI. |
| `counter` | Auto-incrementing integer. Starts from `start_value` (default 0). Little-endian. |
| `reserved` | Fixed byte sequence from JSON. Not editable in GUI. |
| `input` | User-editable field. Rendered as SpinBox (integers) or DoubleSpinBox (floats) in GUI. |

## Field Types (`type`)

| Type | Size (bytes) | Input Widget |
|------|:------------:|--------------|
| `uint8` | 1 | SpinBox |
| `uint16` | 2 | SpinBox |
| `uint32` | 4 | SpinBox |
| `uint64` | 8 | None |
| `int8` | 1 | SpinBox |
| `int16` | 2 | SpinBox |
| `int32` | 4 | SpinBox |
| `int64` | 8 | None |
| `float32` | 4 | DoubleSpinBox |
| `float64` | 8 | DoubleSpinBox |
| `bytes` | specified by `size` key | – (use `constant` or `reserved`) |

## Key Reference

| Key | Required | Applies to | Description |
|-----|:--------:|------------|-------------|
| `name` | yes | all | Unique field identifier |
| `type` | yes | all | Field data type (see table above) |
| `size` | only for `bytes` | `bytes` | Field size in bytes |
| `value_source` | yes | all | `constant`, `counter`, `reserved`, or `input` |
| `default_value_hex` | for `constant`, `reserved` | `constant`, `reserved` | Hex string (e.g. `"AABB"`) |
| `start_value` | for `counter` | `counter` | Starting value (default 0) |
| `minInt` | for `input` + integer types | `input` | Minimum SpinBox value |
| `maxInt` | for `input` + integer types | `input` | Maximum SpinBox value |
| `minDouble` | for `input` + float types | `input` | Minimum DoubleSpinBox value |
| `maxDouble` | for `input` + float types | `input` | Maximum DoubleSpinBox value |

## Examples

### Simple packet with constant and counter

```json
{
  "fields": [
    { "name": "sync",
      "type": "uint8",
      "value_source": "constant", "default_value_hex": "AA" 
    },
    { "name": "counter",
      "type": "uint16",
      "value_source": "counter",
      "start_value": 0 
    },
    { "name": "reserve",
      "type": "bytes",
      "size": 4,
      "value_source": "reserved", "default_value_hex": "00000000" 
    },
    { "name": "voltage",
      "type": "float32", 
      "value_source": "input",
      "minDouble": 0.0, "maxDouble": 5.0, 
    },
    { "name": "mode",
      "type": "uint8",
      "value_source": "input",
      "minInt": 0,
      "maxInt": 3,
    }
  ]
}
```

## 💡 Have a template to share?
If you create a JSON template for a known protocol or packet, pull requests are welcome!