#!/bin/sh

set -eo pipefail

GADGET=g.1
WITH_SERIAL=

SYSDIR=/sys/kernel/config/usb_gadget/
DEVDIR=$SYSDIR/$GADGET

# Gadget
echo "Creating USB gadget $GADGET"
mkdir -p $DEVDIR

echo 0x2fe0  > $DEVDIR/idVendor
echo @USB_IDPRODUCT@ > $DEVDIR/idProduct

echo @USB_BCDDEVICE@ > $DEVDIR/bcdDevice
echo 0x0200 > $DEVDIR/bcdUSB

echo 0x00   > $DEVDIR/bDeviceClass
echo 0x00   > $DEVDIR/bDeviceSubClass
echo 0x00   > $DEVDIR/bDeviceProtocol

mkdir -p $DEVDIR/strings/0x409
echo "$(cat /rom/serial)" > $DEVDIR/strings/0x409/serialnumber
echo "Xaptum, Inc." > $DEVDIR/strings/0x409/manufacturer
echo "@USB_PRODUCT@" > $DEVDIR/strings/0x409/product

# Ethernet Function
echo "Adding function ecm.usb0 to USB gadget $GADGET"
mkdir -p $DEVDIR/functions/ecm.usb0

echo "$(cat /rom/mac_address/2)" > $DEVDIR/functions/ecm.usb0/host_addr
echo "$(cat /rom/mac_address/3)" > $DEVDIR/functions/ecm.usb0/dev_addr

# Serial Function
if $DWITH_SERIAL
echo "Adding function acm.GS0 to USB gadget $GADGET"
mkdir -p $DEVDIR/functions/acm.GS0

# Config
mkdir -p $DEVDIR/configs/c.1
echo 



configs = (
  {
    id   = 1;
    name = "c";

    attrs = {
      bmAttributes = 0xC0;
    }

    functions = (
      {
        name     = "ecm.usb0";
        function = "ecm_usb0";
      },
      {% if with_serial %}
      {
        name     = "acm.GS0";
        function = "acm_GS0";
      }
      {% endif %}
    );
  }
);
