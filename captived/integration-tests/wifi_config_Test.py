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
        self.last_restart_file = os.path.join(DATA_PATH, 'bin', 'connman.service_restart.txt')
        if os.path.isfile(self.last_restart_file):
            os.remove(self.last_restart_file)

    def tearDown(self):
        # delete the last_reboot file
        if os.path.isfile(self.last_restart_file):
            os.remove(self.last_restart_file)
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
        self.run_put_test (URL_PASSTHROUGH, FILE_WIFI_CONFIG_PASSTHROUGH)

    def test_put_wifi_config_secure_host(self):
        self.run_put_test (URL_SECURE_HOST, FILE_WIFI_CONFIG_SECURE_HOST)


    def run_put_test(self, url, wifi_config_file):
        # get and store value to put back
        resp = requests.get(url)
        orig_json_config = resp.json()
        new_json = {'contents':new_config}
        resp = requests.put(url, headers=HEADERS, json=new_json)
        self.assertEqual(new_config, resp.json()['contents'].strip('\n'))
        self.assertTrue(os.path.isfile(self.last_restart_file))

        # check that the file was just created
        modified_time = os.path.getmtime(self.last_restart_file)
        cur_time = time.time()
        self.assertLess(cur_time - modified_time, 3.0)

        # put the original values back
        resp = requests.put(url, headers=HEADERS, json=orig_json_config)
        self.assertNotEqual(new_config, resp.json()['contents'])
        self.assertMatchesFileContents(DATA_PATH + wifi_config_file,
                        resp.json()['contents'])

        # check that the restart-file was modified again
        self.assertLess(modified_time, os.path.getmtime(self.last_restart_file))


if __name__ == '__main__':
    unittest.main(verbosity=2)
