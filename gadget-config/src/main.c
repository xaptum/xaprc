#include <errno.h>
#include <stdio.h>
#include <linux/usb/ch9.h>
#include <usbg/usbg.h>

int main(void)
{
  usbg_state *s;
  usbg_gadget *g;
  usbg_config *c;
  usbg_function *f_acm0, *f_ecm;

  int ret = -EINVAL;
  int usbg_ret;

  struct usbg_gadget_attrs g_attrs = {
                                      .bcdUSB = 0x0200,
                                      .bDeviceClass = USB_CLASS_PER_INTERFACE,
                                      .bDeviceSubClass = 0x00,
                                      .bDeviceProtocol = 0x00,
                                      .bMaxPacketSize0 = 64,
                                      .idVendor = VENDOR,
                                      .idProduct = PRODUCT,
                                      .bcdDevice = 0x0001,
  };

  struct usbg_gadget_strs g_strs = {
                                    .serial = "";
                                    .manufacturer = "";
                                    .product = "";
  };

  struct usbg_config_strs c_strs = {
                                    .configuration = "";
  }

  
}
