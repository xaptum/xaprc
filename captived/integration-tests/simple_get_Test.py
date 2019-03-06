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
HEADERS = {'Content-Type':'application/json'}

# This is the list of resources to test
# tuples are (file_path, REST_path)
check_these = [ ('/serial_number', '/serial_number'),
                ('/mac_address', '/mac_address'),
                ('/enf1/address', '/control_address'),
                ('/enf0/address', '/data_address'),
                ('/mender/artifact_info', '/firmware_version')
              ]


################################################################################
### simple_get_Test
### Test the various REST resources that only return a single value
### These resources should NOT support PUT requests.
################################################################################
class simple_get_Test(test.SharedServer, test.IntegrationTestCase):

    def setUp(self):
        super(simple_get_Test, self).setUp()
        
        # copy the default config directory 
        # currently, testing will only work with debug build because of config
        # file paths.
        src_path = os.path.join(TESTDIR, 'config', 'default')
        self.config_path = os.path.join(CWD, 'config')
        self.copyConfigDirDestructive(src_path, self.config_path)

        # Wait for the embedded server to start up.
        time.sleep(2.1)

    def tearDown(self):
        # put back to secure_host
        resp = requests.put(URL, headers=HEADERS, json='secure_host')
        super(simple_get_Test, self).tearDown()

    def test_simple_resources_get(self):
        for pair in check_these:
            resp = requests.get(URL + pair[1])
            self.assertMatchesFirstLineOfFile(self.config_path + pair[0], resp.json())

    # PUT is not legal in any of the listed resources
    def test_simple_resources_put(self):
        for pair in check_these:
            resp = requests.put(URL + pair[1], headers=HEADERS, json='junk-data')
            print ('--------------')
            print ('PUT response for: ', pair[1], '\n', resp.text)
            self.assertEqual(resp.status_code, 405)
            resp = requests.get(URL + pair[1])
            print ('GET response for: ', pair[1], '\n', resp.text)
            self.assertNotEqual('junk-data', resp.json())

        

if __name__ == '__main__':
    unittest.main(verbosity=2)
