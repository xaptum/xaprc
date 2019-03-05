#!/usr/bin/env python3

import json
import os
import requests
import time
import unittest

import xaptum.embedded_server as eserver
import xaptum.test as test

CWD = os.path.abspath(os.path.join(os.pardir, os.pardir))
TESTDIR = os.path.join(CWD, 'testBin', 'integration-tests')
URL = 'http://[::1]:4000'


################################################################################
### root_level_status_Test
### Test the embedded REST server's root path.
################################################################################
class root_level_status_Test(test.SharedServer, test.IntegrationTestCase):

    def setUp(self):
        super(root_level_status_Test, self).setUp()
        
        # copy the default config directory 
        # currently, testing will only work with debug build because of config
        # file paths.
        src_path = os.path.join(TESTDIR, 'config', 'default')
        self.config_path = os.path.join(CWD, 'config')
        self.copyConfigDirDestructive(src_path, self.config_path)

        # Wait for the server to start up
        time.sleep(2.1)

    def tearDown(self):
        super(root_level_status_Test, self).tearDown()

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
            self.assertMatchesFirstLineOfFile(self.config_path + pair[0], jresp[pair[1]])

        

if __name__ == '__main__':
    unittest.main(verbosity=2)
