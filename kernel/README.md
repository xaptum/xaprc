# xaprc

A Linux kernel driver for the Xaptum ENF router cards, a line of mini
PCI-e network cards to connect IoT devices and gateways to the Xaptum
Edge Network Fabric, a secure IPv6 overlay network for IoT.

## Supported Product IDs

| Model #    | Description                | USB Vendor ID | USB Product ID |
|------------|----------------------------|---------------|----------------|
| XAP-RC-001 | Xaptum Router Card         | 0x2FE0        | 0x8B01         |
| XAP-RW-001 | Xaptum Router Card w/ WiFi | 0x2FE0        | 0x8B02         |

## Installation

### Debian (Stretch)

(TODO)

``` bash
# Install the Xaptum API repo GPG signing key.
apt-key adv --keyserver keyserver.ubuntu.com --recv-keys c615bfaa7fe1b4ca

# Add the repository to your APT sources.
echo "deb http://dl.bintray.com/xaptum/deb stretch main" > /etc/apt/sources.list.d/xaptum.list

# Install the library
sudo apt-get install xaprc-dkms
```

### From Source

#### Build Dependencies

* `make`
* `gcc`
* Linux kernel headers for your distribution

#### xaprc

```bash
make
sudo make modules_install
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
