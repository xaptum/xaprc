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


################################################################################
### root_level_status_Test
### Test the embedded REST server's root path.
################################################################################
class root_level_status_Test(test.SharedServer, test.IntegrationTestCase):

    @classmethod
    def setUpClass(cls, args=None):
        super(root_level_status_Test, cls).setUpClass(['-p', DATA_PATH])

    def setUp(self):
        super(root_level_status_Test, self).setUp()

    def tearDown(self):
        super(root_level_status_Test, self).tearDown()

    def test_get_root(self):
        resp = requests.get(URL)
        jresp = resp.json()

        # tuples are (path, json index)
        check_these = [ ('/rom/serial', 'serial_number'),
                        ('/data/enftun/enf1/address', 'control_address'),
                        ('/data/enftun/enf0/address', 'data_address')
                      ]
        for pair in check_these:
            self.assertMatchesFirstLineOfFile(DATA_PATH + pair[0], jresp[pair[1]])
        self.assertEqual('secure-host', jresp['mode'])

        address_obj = jresp['mac_address']
        for i in range(1, 5):
            mac_file = DATA_PATH + '/rom/mac_address/' + str(i)
            self.assertMatchesFirstLineOfFile(mac_file, address_obj[str(i)])

        firmware_resp = jresp['firmware_version']
        with open(DATA_PATH + '/etc/mender/artifact_info') as f:
            firmware_file = f.readline().strip('\n')
            firmware_file = firmware_file.split('=')[1]
            self.assertEqual(firmware_file, firmware_resp)

if __name__ == '__main__':
    unittest.main(verbosity=2)
