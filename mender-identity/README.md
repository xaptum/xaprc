# mender-identity

This package installs a script to export the ENF control network IP
address as the Mender device identity.

## CMake Options

The following CMake configuration options are supported.

| Option              | Values | Default                    | Description                             |
|---------------------|--------|----------------------------|-----------------------------------------|
| MENDER_INSTALL_PATH |        | /usr/share/mender/identity | The directory to install the script in. |

## Installing

```bash
cd build
cmake --build . --target install
```
