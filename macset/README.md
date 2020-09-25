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

## License
Copyright (c) 2018 Xaptum, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
