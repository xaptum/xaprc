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
        # Wait for the server to start up
        time.sleep(1.1)

    def tearDown(self):
        super(wifi_Test, self).tearDown()

        
    @unittest.skip("Not implemented yet & test needs updating")
    def test_get_wifi(self):
        resp = requests.get(URL)
        with open(DATA_PATH + '/something') as f:
            read_data = f.read()
            self.assertEqual(read_data, resp.json()['contents'])
        


if __name__ == '__main__':
    unittest.main(verbosity=2)
