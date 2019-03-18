#!/usr/bin/env python3

import hashlib
import json
import os
import requests
import shutil
import time
import unittest

import xaptum.embedded_server as eserver
import xaptum.test as test

TESTDIR = os.getcwd()
CWD = os.path.abspath(os.path.join(os.pardir, os.pardir))
URL_PASSTHROUGH = 'http://[::1]:4000/wifi/config/passthrough'
URL_SECURE_HOST = 'http://[::1]:4000/wifi/config/secure_host'
# use this directory as the root path for this test
DATA_PATH = os.path.join(TESTDIR, 'config', 'default')

HEADERS = {'Content-Type':'application/json'}
FILE_WIFI_CONFIG_PASSTHROUGH = '/data/connman/passthrough/wifi.config'
FILE_WIFI_CONFIG_SECURE_HOST = '/data/connman/secure-host/wifi.config'

new_config = 'ctrl_interface=/var/run/wpa_supplicant\nap_scan=1\n\nnetwork={\nssid="customer_private"\npsk="none"\n}'

################################################################################
### wifi_config_Test
### Test /wifi/config REST resource
################################################################################
class wifi_config_Test(test.SharedServer, test.IntegrationTestCase):

    @classmethod
    def setUpClass(cls, args=None):
        super(wifi_config_Test, cls).setUpClass(['-p', DATA_PATH])

    def setUp(self):
        super(wifi_config_Test, self).setUp()

    def tearDown(self):
        super(wifi_config_Test, self).tearDown()

    def test_get_wifi_config_passthrough(self):
        resp = requests.get(URL_PASSTHROUGH)
        self.assertMatchesFileContents(DATA_PATH + FILE_WIFI_CONFIG_PASSTHROUGH,
                                       resp.json()['contents'])
        self.assertMatchesFileSHA256(DATA_PATH + FILE_WIFI_CONFIG_PASSTHROUGH,
                                     resp.json()['sha256'])

    def test_get_wifi_config_secure_host(self):
        resp = requests.get(URL_SECURE_HOST)
        self.assertMatchesFileContents(DATA_PATH + FILE_WIFI_CONFIG_SECURE_HOST,
                                       resp.json()['contents'])
        self.assertMatchesFileSHA256(DATA_PATH + FILE_WIFI_CONFIG_SECURE_HOST,
                                     resp.json()['sha256'])

    def test_put_wifi_config_passthrough(self):
        # get and store value to put back
        resp = requests.get(URL_PASSTHROUGH)
        orig_json_config = resp.json()
        new_json = {'contents':new_config}
        resp = requests.put(URL_PASSTHROUGH, headers=HEADERS, json=new_json)
        self.assertEqual(new_config, resp.json()['contents'].strip('\n'))

        # put the original values back
        resp = requests.put(URL_PASSTHROUGH, headers=HEADERS, json=orig_json_config)
        self.assertNotEqual(new_config, resp.json()['contents'])
        self.assertMatchesFileContents(DATA_PATH + FILE_WIFI_CONFIG_PASSTHROUGH,
                        resp.json()['contents'])

    def test_put_wifi_config_secure_host(self):
        # get and store value to put back
        resp = requests.get(URL_SECURE_HOST)
        orig_json_config = resp.json()
        new_json = {'contents':new_config}
        resp = requests.put(URL_SECURE_HOST, headers=HEADERS, json=new_json)
        self.assertEqual(new_config, resp.json()['contents'].strip('\n'))

        # put the original values back
        resp = requests.put(URL_SECURE_HOST, headers=HEADERS, json=orig_json_config)
        self.assertNotEqual(new_config, resp.json()['contents'])
        self.assertMatchesFileContents(DATA_PATH + FILE_WIFI_CONFIG_SECURE_HOST,
                        resp.json()['contents'])


if __name__ == '__main__':
    unittest.main(verbosity=2)
