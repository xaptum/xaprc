# wpa-credentials

This package installs the wpa SSID and password to conenct to on startup

## CMake Options

The following CMake configuration options are supported.

| Option                 | Values    | Default          | Description                                         |
|------------------------|-----------|------------------|-----------------------------------------------------|
| XAPRC_SSID             |           | xaprc_default    | The SSID to connect to.                             |
| XAPRC_PASS             |           | password123!     | The password to connect with.                       |

## Installing

```bash
cd build
cmake --build . --target install
```
