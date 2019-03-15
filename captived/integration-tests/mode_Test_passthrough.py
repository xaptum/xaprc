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
URL = 'http://[::1]:4000/mode'
HEADERS = {'Content-Type':'application/json'}
# use this directory as the root path for this test
DATA_PATH = os.path.join(TESTDIR, 'config', 'passthrough')

################################################################################
### mode_Test_passthrough
### This test is nearly identical to the mode_Test, but it tests being able to
### use an alternate symbolic link to the configuration directory.
### Since the mode_Test already checked the functionality, all this does is
### test reading the from the alternate directory.
################################################################################
class mode_Test_passthrough(test.SharedServer, test.IntegrationTestCase):

    @classmethod
    def setUpClass(cls, args=None):
        super(mode_Test_passthrough, cls).setUpClass(['-p', DATA_PATH])


    def setUp(self):
        super(mode_Test_passthrough, self).setUp()
        # Wait for the embedded server to start up.
        time.sleep(1.1)

    def tearDown(self):
        # not changing in this test, so don't need to set mode back to default
        super(mode_Test_passthrough, self).tearDown()

    # This should get back the defult mode - which is passthrough for this test
    def test_get_default_mode(self):
        resp = requests.get(URL)
        self.assertEqual(resp.json(), "passthrough")


    def test_get_mode(self):
        router_mode = os.path.join(DATA_PATH, 'data', 'systemd', 'xbridgex.target.active', 'id')
        resp = requests.get(URL)
        self.assertMatchesFirstLineOfFile(router_mode, resp.json())
        

if __name__ == '__main__':
    unittest.main(verbosity=2)
