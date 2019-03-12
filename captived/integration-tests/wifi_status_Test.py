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
        # Wait for the embedded server to start up.
        time.sleep(1.1)

    def tearDown(self):
        super(wifi_status_Test, self).tearDown()

    @unittest.skip('Not implemented and test needs updating')
    def test_get_wifi_status(self):
        resp = requests.get(URL)
        self.assertMatchesFileContents(DATA_PATH + '/some-status',
                        resp.json()['contents'])


if __name__ == '__main__':
    unittest.main(verbosity=2)
