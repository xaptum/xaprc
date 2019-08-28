#!/usr/bin/env python3

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
URL = 'http://[::1]:4000/wifi/status'
# use this directory as the root path for this test
DATA_PATH = os.path.join(TESTDIR, 'config', 'default')

HEADERS = {'Content-Type':'application/json'}

new_config = 'ctrl_interface=/var/run/wpa_supplicant\nap_scan=1\n\nnetwork={\nssid="customer_private"\npsk="none"\n}'

################################################################################
### wifi_status_Test
### Test /wifi/status REST resource
################################################################################
class wifi_status_Test(test.SharedServer, test.IntegrationTestCase):

    @classmethod
    def setUpClass(cls, args=None):
        super(wifi_status_Test, cls).setUpClass(['-p', DATA_PATH])

    def setUp(self):
        super(wifi_status_Test, self).setUp()
        self.iw_response_file = os.path.join(DATA_PATH, 'sbin', 'wlan0.iw.response')

    def tearDown(self):
        super(wifi_status_Test, self).tearDown()

    # This can't be compared to a file because the status depends on the
    # specific machine on which the tests are running.
    def test_get_wifi_status(self):
        connected_file = os.path.join(DATA_PATH, 'sbin', 'wlan0.connected')
        shutil.copyfile(connected_file, self.iw_response_file)
        resp = requests.get(URL)
        jresp = resp.json()
        print ('\nJSON status response = \n')
        print (json.dumps(jresp, indent=4), '\n')
        
        self.assertIn('connected', jresp)
        self.assertIn('SSID', jresp)
        self.assertEqual('test_wifi_1', jresp['SSID'], 'SSID did not match expected value.')
        # can't compare IP addresses because we may not have
        # wifi connection on the test machine.

    def test_get_wifi_status_not_connected(self):
        not_connected_file = os.path.join(DATA_PATH, 'sbin', 'wlan0.not')
        shutil.copyfile(not_connected_file, self.iw_response_file)
        resp = requests.get(URL)
        jresp = resp.json()
        print ('\nJSON status response = \n')
        print (json.dumps(jresp, indent=4), '\n')
        
        self.assertIn('connected', jresp)
        self.assertNotIn('SSID', jresp, 'Test should not find SSID, but did.')
        # can't compare IP addresses because we may not have
        # wifi connection on the test machine.

if __name__ == '__main__':
    unittest.main(verbosity=2)
