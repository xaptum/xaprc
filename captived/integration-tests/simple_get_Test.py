#!/usr/bin/env python3

import json
import os
import requests
import time
import unittest

import xaptum.embedded_server as eserver
import xaptum.test as test

TESTDIR = os.getcwd()
CWD = os.path.abspath(os.path.join(os.pardir, os.pardir))
URL = 'http://[::1]:4000'
# use this directory as the root path for this test
DATA_PATH = os.path.join(TESTDIR, 'config', 'default')

HEADERS = {'Content-Type':'application/json'}

# This is the list of resources to test
# tuples are (file_path, REST_path)
check_these = [ ('/rom/serial', '/serial_number'),
                ('/rom/mac_address/1', '/mac_address'),
                ('/data/enftun/enf1/address', '/control_address'),
                ('/data/enftun/enf0/address', '/data_address'),
                ('/etc/mender/artifact_info', '/firmware_version')
              ]


################################################################################
### simple_get_Test
### Test the various REST resources that only return a single value
### These resources should NOT support PUT requests.
################################################################################
class simple_get_Test(test.SharedServer, test.IntegrationTestCase):

    @classmethod
    def setUpClass(cls, args=None):
        super(simple_get_Test, cls).setUpClass(['-p', DATA_PATH])

    def setUp(self):
        super(simple_get_Test, self).setUp()
        # Wait for the embedded server to start up.
        time.sleep(1.1)

    def tearDown(self):
        # put back to secure_host
        resp = requests.put(URL, headers=HEADERS, json='secure_host')
        super(simple_get_Test, self).tearDown()

    def test_simple_resources_get(self):
        for pair in check_these:
            resp = requests.get(URL + pair[1])
            self.assertMatchesFirstLineOfFile(DATA_PATH + pair[0], resp.json())

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
