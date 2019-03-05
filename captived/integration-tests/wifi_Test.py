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
URL = 'http://[::1]:4000/wifi'


################################################################################
### wifi_Test
### Test the GET functionality of the /wifi rest resource
################################################################################
class wifi_Test(test.SharedServer, test.IntegrationTestCase):

    def setUp(self):
        super(wifi_Test, self).setUp()
        
        # copy the default config directory 
        # currently, testing will only work with debug build because of config
        # file paths.
        src_path = os.path.join(TESTDIR, 'config', 'default')
        self.config_path = os.path.join(CWD, 'config')
        self.copyConfigDirDestructive(src_path, self.config_path)

        # Wait for the server to start up
        time.sleep(2.1)

    def tearDown(self):
        super(wifi_Test, self).tearDown()

        
    @unittest.skip("Not implemented yet & test needs updating")
    def test_get_wifi(self):
        resp = requests.get(URL)
        with open(self.config_path + '/something') as f:
            read_data = f.read()
            self.assertEqual(read_data, resp.json()['contents'])
        


if __name__ == '__main__':
    unittest.main(verbosity=2)
