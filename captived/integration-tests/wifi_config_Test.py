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
URL = 'http://[::1]:4000/wifi/config'
HEADERS = {'Content-Type':'application/json'}

new_config = 'ctrl_interface=/var/run/wpa_supplicant\nap_scan=1\n\nnetwork={\nssid="customer_private"\npsk="none"\n}'

################################################################################
### wifi_config_Test
### Test /wifi/config REST resource
################################################################################
class wifi_config_Test(test.SharedServer, test.IntegrationTestCase):

    def setUp(self):
        super(wifi_config_Test, self).setUp()
        
        # copy the default config directory 
        # currently, testing will only work with debug build because of config
        # file paths.
        src_path = os.path.join(TESTDIR, 'config', 'default')
        self.config_path = os.path.join(CWD, 'config')
        self.copyConfigDirDestructive(src_path, self.config_path)

        # Wait for the embedded server to start up.
        time.sleep(2.1)

    def tearDown(self):
        super(wifi_config_Test, self).tearDown()

    def test_get_wifi_config(self):
        resp = requests.get(URL)
        self.assertMatchesFileContents(self.config_path + '/wpa_supplicant-nl80211-wlan0.conf',
                        resp.json()['contents'])

    def test_put_wifi_config(self):
        # get and store value to put back
        resp = requests.get(URL)
        orig_json_config = resp.json()
        new_json = {'contents':new_config}
        resp = requests.put(URL, headers=HEADERS, json=new_json)
        self.assertEqual(new_config, resp.json()['contents'].strip('\n'))

        # put the original values back
        resp = requests.put(URL, headers=HEADERS, json=orig_json_config)
        self.assertNotEqual(new_config, resp.json()['contents'])
        self.assertMatchesFileContents(self.config_path + '/wpa_supplicant-nl80211-wlan0.conf',
                        resp.json()['contents'])


if __name__ == '__main__':
    unittest.main(verbosity=2)
