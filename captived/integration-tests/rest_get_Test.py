#!/usr/bin/env python3

import json
import os
import requests
import shutil
import time
import unittest

import xaptum.embedded_server as eserver
import xaptum.test as test

CWD = os.path.abspath(os.path.join(os.pardir, os.pardir))
TESTDIR = os.path.join(CWD, 'testBin', 'integration-tests')
URL = 'http://[::1]:4000'


################################################################################
### rest_get_Test
### Test the basic GET functionality of the embedded REST server
################################################################################
class rest_get_Test(test.SharedServer, test.IntegrationTestCase):

    def setUp(self):
        super(rest_get_Test, self).setUp()
        
        # copy the default config directory 
        # currently, testing will only work with debug build because of config
        # file paths.
        src_path = os.path.join(TESTDIR, 'config', 'default')
        self.config_path = os.path.join(CWD, 'config')
        copy_recursive_destructive(src_path, self.config_path)

        # Wait for the server to start up
        time.sleep(2.1)

    def tearDown(self):
        super(rest_get_Test, self).tearDown()

    def test_get_root(self):
        resp = requests.get(URL)
        jresp = resp.json()

        # tuples are (path, json index)
        check_these = [ ('/serial_number', 'serial_number'),
                        ('/mac_address', 'mac_address'),
                        ('/mender/artifact_info', 'firmware_version'),
                        ('/enf1/address', 'control_address'),
                        ('/enf0/address', 'data_address'),
                        ('/router_mode', 'mode')
                      ]
        for pair in check_these:
            self.assertMatchesFileContents(self.config_path + pair[0], jresp[pair[1]])

    def test_get_mode(self):
        temp_url = URL + '/mode'
        resp = requests.get(temp_url)
        self.assertMatchesFileContents(self.config_path + '/router_mode', resp.json())

    def test_get_wifi_config(self):
        temp_url = URL + '/wifi/config'
        resp = requests.get(temp_url)
        with open(self.config_path + '/wpa_supplicant-nl80211-wlan0.conf') as f:
            read_data = f.read()
            self.assertEqual(read_data, resp.json()['contents'])
        
    @unittest.skip("Not implemented yet & test needs updating")
    def test_get_wifi(self):
        temp_url = URL + '/wifi'
        resp = requests.get(temp_url)
        with open(self.config_path + '/something') as f:
            read_data = f.read()
            self.assertEqual(read_data, resp.json()['contents'])
        
    @unittest.skip("Not implemented yet & test needs updating")
    def test_get_wifi_status(self):
        temp_url = URL + '/wifi/status'
        resp = requests.get(temp_url)
        with open(self.config_path + '/something') as f:
            read_data = f.read()
            self.assertEqual(read_data, resp.json()['contents'])
        

def copy_recursive_destructive(src_path, dest_path):
    if (os.path.isdir(dest_path)):
        shutil.rmtree(dest_path)

    shutil.copytree(src_path, dest_path)


if __name__ == '__main__':
    unittest.main(verbosity=2)
