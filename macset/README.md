# macset

This package installs a systemd unit to set the mac address of an
interface. The service is enabled to set the MAC address of the
`wlan0` interface from `/rom/mac_address/1`.

## CMake Options

The following CMake configuration options are supported.

| Option      | Values  | Default | Description                   |
|-------------|---------|---------|-------------------------------|
| BUILD_WLAN0 | ON, OFF | ON      | Enable the service for wlan0. |

## Installing

```bash
cd build
cmake --build . --target install
```
