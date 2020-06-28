/*
 * Copyright 2018 Xaptum, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <linux/usb/ch9.h>
#include <usbg/usbg.h>
#include <usbg/function/net.h>
#include <string.h>

#define USB_PROTO_IAD 0x01

/* Check for required defines */
#ifndef XAP_PRODUCT_ID
#error Variable XAP_PRODUCT_ID not specified
#endif

#ifndef XAP_PRODUCT_STR
#error Variable XAP_PRODUCT_STR not specified
#endif

#define XAP_VENDOR_ID	0x2fe0
#define XAP_REV_ID	0x414
#define XAP_MANUFACTURER_STR "Xaptum, Inc."
#define XAP_USBG_STR "g1"
/**
 * @brief Read a file of a known length to a string
 * @param path The path of the file to read
 * @param len The length of the string to read
 * @return A pointer to newly allocated memory containing the string.
 * @warning Caller is responible for freeing returned pointer.
 */
char* readFile(const char *path, long len)
{
	FILE *fPtr;
	long fSize;
	char *ret;

	/* Account for the null terminator */
	len++;

	if(!path)
	{
		fprintf(stderr, "No serial file path given\n");
		return NULL;
	}

	fPtr = fopen(path,"r");

	/* Open the file */
	if(!fPtr)
	{
		fprintf(stderr, "Failed loading serial file %s\n", path);
		return NULL;
	}

	/* Get the file size */
	fseek (fPtr , 0 , SEEK_END);
	fSize = ftell(fPtr);
	rewind(fPtr);
	if(fSize != len)
	{
		fprintf(stderr, "Failed loading serial file %s, incorrect length %ld \n", path, fSize);
		return NULL;
	}

	/* Read the file to a string */
	ret = malloc(len);
	fread(ret,1,len,fPtr);
	ret[len] = '\0';

	return ret;
}

static void print_usbg_error(const char *message, int code)
{
	fprintf(stderr, "%s\n", message);
	fprintf(stderr, "Error: %s : %s\n", usbg_error_name(code),
			usbg_strerror(code));
}

/**
 * @brief Convert a mac address string to a series of 6 bytes
 * @param addr The string to read from
 * @param dest The memory buffer to write to, expected to be length 6
 * @return A pointer to newly allocated memory containing the string.
 */
static void macStrToOctet(char *addr, char *dest)
{
	if(!addr || !dest || strlen(addr) < 16)
	{
		return;
	}
	for(int i=0;i<6;i++)
	{
		char buf[5] = {'0', 'x', addr[i*3], addr[i*3+1], 0};
		dest[i] = strtol(buf,NULL,0);
	}
}

static void create_usb_gadget(usbg_state *state)
{
	usbg_gadget *gadget;
	usbg_config *config;
	usbg_function *f_acm0, *f_ecm, *f_hss;
	int ret = -EINVAL;
	int usbg_ret;

	char * mac_addr1 = readFile("/rom/mac_address/1",17);
	char * mac_addr2 = readFile("/rom/mac_address/2",17);
	char * serial = readFile("/rom/serial",14);

	struct usbg_gadget_attrs g_attrs = {
		.bcdUSB = 0x0200,
		.bDeviceClass = USB_CLASS_MISC,
		.bDeviceSubClass = USB_CLASS_COMM,
		.bDeviceProtocol = USB_PROTO_IAD,
		.bMaxPacketSize0 = 64, /* Max allowed ep0 packet size */
		.idVendor = XAP_VENDOR_ID,
		.idProduct = XAP_PRODUCT_ID,
		.bcdDevice = 0x0000, /* Verson of device */
	};

	struct usbg_gadget_strs g_strs = {
		.manufacturer = XAP_MANUFACTURER_STR, /* Manufacturer */
		.product = XAP_PRODUCT_STR,
		.serial = serial
	};

	struct usbg_config_strs c_strs = {
		.configuration = ""
	};


	/* Set up the ethernet attributes */
	struct usbg_f_net_attrs eth_attrs = {0};

	macStrToOctet(mac_addr1,eth_attrs.host_addr.ether_addr_octet);
	macStrToOctet(mac_addr2,eth_attrs.dev_addr.ether_addr_octet);
	eth_attrs.ifname = "\0";
	eth_attrs.qmult = 5;

	usbg_ret = usbg_create_gadget(state, XAP_USBG_STR, &g_attrs, &g_strs, &gadget);
	if (usbg_ret != USBG_SUCCESS)
	{
		print_usbg_error( "Error on create gadget",usbg_ret);
		goto exit_clean;
	}


	#ifdef XAP_SERIAL
	usbg_ret = usbg_create_function(gadget, USBG_F_ACM, "GS0", NULL, &f_acm0);
	if (usbg_ret != USBG_SUCCESS)
	{
		print_usbg_error( "Error creating acm0 function",usbg_ret);
		goto exit_clean;
	}
	#endif

	usbg_ret = usbg_create_function(gadget, USBG_F_ECM, "usb0", &eth_attrs, &f_ecm);
	if (usbg_ret != USBG_SUCCESS)
	{
		print_usbg_error( "Error creating ecm function",usbg_ret);
		goto exit_clean;
	}

	#ifdef XAP_HSS
	usbg_ret = usbg_create_function(gadget, USBG_F_HSS, "hss0", NULL, &f_hss);
	if (usbg_ret != USBG_SUCCESS)
	{
		print_usbg_error( "Error creating hss function",usbg_ret);
		goto exit_clean;
	}
	#endif

	/* NULL can be passed to use kernel defaults */
	usbg_ret = usbg_create_config(gadget, 1, "The only one", NULL, &c_strs, &config);
	if (usbg_ret != USBG_SUCCESS)
	{
		print_usbg_error( "Error creating config",usbg_ret);
		goto exit_clean;
	}

	#ifdef XAP_SERIAL
	usbg_ret = usbg_add_config_function(config, "acm.GS0", f_acm0);
	if (usbg_ret != USBG_SUCCESS)
	{
		print_usbg_error( "Error adding acm.GS0",usbg_ret);
		goto exit_clean;
	}
	#endif

	usbg_ret = usbg_add_config_function(config, "ecm.usb0", f_ecm);
	if (usbg_ret != USBG_SUCCESS)
	{
		print_usbg_error( "Error adding ecm.usb0",usbg_ret);
		goto exit_clean;
	}


	#ifdef XAP_HSS
	usbg_ret = usbg_add_config_function(config, "hss.hss0", f_hss);
	if (usbg_ret != USBG_SUCCESS)
	{
		print_usbg_error( "Error adding hss.hss0",usbg_ret);
		goto exit_clean;
	}
	#endif

	usbg_ret = usbg_enable_gadget(gadget, DEFAULT_UDC);
	if (usbg_ret != USBG_SUCCESS)
	{
		print_usbg_error( "Error enabling gadget",usbg_ret);
		goto exit_clean;
	}

exit_clean:
	usbg_cleanup(state);

exit_noclean:
	free(mac_addr1);
	free(mac_addr2);
	free(serial);
}

static void destroy_usb_gadget(usbg_state *state)
{
	int usbg_ret;
	usbg_gadget *gadget;

	gadget = usbg_get_gadget(state,XAP_USBG_STR);
	usbg_ret = usbg_disable_gadget(gadget);

	if (usbg_ret != USBG_SUCCESS)
	{
		print_usbg_error( "Error disabling gadget",usbg_ret);
	}

	usbg_ret = usbg_rm_gadget(gadget, USBG_RM_RECURSE);
	if (usbg_ret != USBG_SUCCESS)
	{
		print_usbg_error( "Error freeing gadget",usbg_ret);
	}
}

static void print_usage(int argc, char **argv)
{
	if(argc > 0)
		printf("Usage: %s (create|destroy)", argv[0]);
	else
		printf("Usage: xap-usb-gadget (create|destroy)");
}

int main(int argc, char **argv)
{
	usbg_state *state;
	int usbg_ret;

	/* Check the parameters and print a usage message if they are invalid */
	if(argc != 2 || (strcmp(argv[1],"create")!=0 && strcmp(argv[1],"destroy")!=0))
	{
		print_usage(argc, argv);
		goto exit;
	}

	//Create a USGB state
	usbg_ret = usbg_init("/sys/kernel/config", &state);
	if (usbg_ret != USBG_SUCCESS)
	{
		print_usbg_error(  "Error on USB gadget init",usbg_ret);
		goto exit;
	}
	if(strcmp(argv[1],"create") == 0)
	{
		create_usb_gadget(state);
	}
	if(strcmp(argv[1],"destroy") == 0)
	{
		destroy_usb_gadget(state);
	}

	exit:
	return 0;
}
