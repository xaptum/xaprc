attrs : {
  idVendor  = 0x2fe0;
  idProduct = {{ id_product }};

  bcdUSB    = 0x0200;

  bDeviceClass    = 0x00;
  bDeviceSubClass = 0x00;
  bDeviceProtocol = 0x00;
};

strings = (
  {
    lang = 0x409;

    manufacturer = "Xaptum, Inc.";
    product      = "{{ product }}";
  }
);
