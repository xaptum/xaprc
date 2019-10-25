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
URL = 'http://[::1]:4000/wifi'
# use this directory as the root path for this test
DATA_PATH = os.path.join(TESTDIR, 'config', 'default')



################################################################################
### wifi_Test
### Test the GET functionality of the /wifi rest resource
################################################################################
class wifi_Test(test.SharedServer, test.IntegrationTestCase):

    @classmethod
    def setUpClass(cls, args=None):
        super(wifi_Test, cls).setUpClass(['-p', DATA_PATH])

    def setUp(self):
        super(wifi_Test, self).setUp()
        # test uses fake 'iw' command which returns the contents of wlan0.iw.response
        self.iw_response_file = os.path.join(DATA_PATH, 'sbin', 'wlan0.iw.response')

    def tearDown(self):
        super(wifi_Test, self).tearDown()

        
    def test_get_wifi(self):
        connected_file = os.path.join(DATA_PATH, 'sbin', 'wlan0.connected')
        shutil.copyfile(connected_file, self.iw_response_file)
        resp = requests.get(URL)
        jresp = resp.json()
        print (json.dumps(jresp, indent=4))

        # check that 'status' exists in the json response
        self.assertIn('status', jresp)
        self.assertIn('connected', jresp['status'])
        self.assertIn('SSID', jresp['status'])
        self.assertEqual('test_wifi_1', jresp['status']['SSID'],
                'SSID did not match expected value.')
        self.assertIn('IPv4_addresses', jresp['status'])
        self.assertIn('IPv6_addresses', jresp['status'])

        # check that the config section exists
        self.assertIn('config', jresp)
        self.assertIn('passthrough', jresp['config'])
        self.assertIn('secure', jresp['config'])
        self.assertIn('sha256', jresp['config']['passthrough'])
        self.assertIn('sha256', jresp['config']['secure'])
        
    def test_get_disconnected_wifi(self):
        connected_file = os.path.join(DATA_PATH, 'sbin', 'wlan0.not')
        shutil.copyfile(connected_file, self.iw_response_file)
        resp = requests.get(URL)
        jresp = resp.json()
        print (json.dumps(jresp, indent=4))

        # check that 'status' exists in the json response
        self.assertIn('status', jresp)
        self.assertIn('connected', jresp['status'])
        self.assertIn('SSID', jresp['status'])
        self.assertEqual('', jresp['status']['SSID'],
                'SSID did not match expected value.')
        self.assertIn('IPv4_addresses', jresp['status'])
        self.assertIn('IPv6_addresses', jresp['status'])

        # check that the config section exists
        self.assertIn('config', jresp)
        self.assertIn('passthrough', jresp['config'])
        self.assertIn('secure', jresp['config'])
        self.assertIn('sha256', jresp['config']['passthrough'])
        self.assertIn('sha256', jresp['config']['secure'])
        

if __name__ == '__main__':
    unittest.main(verbosity=2)
