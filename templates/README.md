## En [Ru](README_RU.md)

# JSON Template Guide

This guide describes how to write JSON templates for **UDP Packet Generator**.
Templates define the structure of UDP packets — field names, types, sizes, and value sources.

To use a template, place the `.json` file in any location and load it from the application via the **Load** button.
Examples can be found in this folder.

For general project information, see the main [README](../README.md).

## Value Sources (`source`)

Each field must specify a `source` — where the field's data comes from.

| Source | Description |
|--------|-------------|
| `constant` | Fixed hex value from JSON. Not editable in GUI. |
| `counter` | Auto-incrementing integer. Starts from `start_value` (default 0). |
| `input` | User-editable field. Rendered as SpinBox (integers), DoubleSpinBox (floats) in GUI and etc. |

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
| `text` | special | Button -> Dialog |
| `json` | special | Button -> Dialog |

## Key Reference

| Key | Required | Applies to | Description |
|-----|:--------:|------------|-------------|
| `name` | yes | all | Unique field identifier |
| `type` | yes | all | Field data type (see table above) |
| `source` | yes | all | `constant`, `counter`, or `input` |
| `start_value` | for `counter` | `counter` | Starting value (default 0) |
| `min` | for `input` | `input` | Minimum value |
| `max` | for `input` | `input` | Maximum value |
| `decimals` | fot `input` float type | `input` | Decimals of DoubleSpinBox (default 3) |
| `value` | no | for `input` text type | Default value |
| `size` | no | text types | Data size, if not specified or zero, then calculated automatically. Otherwise, padded with 0x00 |

## Examples

### Simple packet with constant and counter

```json
{
  "fields": [
    { "name": "sync",
      "type": "uint8",
      "source": "constant"
    },
    { "name": "counter",
      "type": "uint16",
      "source": "counter",
      "start_value": 0 
    },
    { "name": "voltage",
      "type": "float32", 
      "source": "input",
      "min": 0.0,
      "max": 5.0, 
      "decimals": 4
    },
    { "name": "mode",
      "type": "int32",
      "source": "input",
      "min": -3,
      "max": 3,
    },    
    { "name": "name",
      "type": "text", 
      "source": "input",
      "value": "Oleg",
      "size": 6
    }
  ]
}
```

## 💡 Have a template to share?
If you create a JSON template for a known protocol or packet, pull requests are welcome!