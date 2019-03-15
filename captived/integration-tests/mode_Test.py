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
DATA_PATH = os.path.join(TESTDIR, 'config', 'default')

################################################################################
### mode_Test
### Test /mode REST resource
################################################################################
class mode_Test(test.SharedServer, test.IntegrationTestCase):

    @classmethod
    def setUpClass(cls, args=None):
        super(mode_Test, cls).setUpClass(['-p', DATA_PATH])


    def setUp(self):
        super(mode_Test, self).setUp()

    def tearDown(self):
        # put back to secure_host
        resp = requests.put(URL, headers=HEADERS, json='secure_host')
        super(mode_Test, self).tearDown()

    # This should get back the defult mode - which is secure_host
    def test_01_get_mode(self):
        resp = requests.get(URL)
        self.assertEqual(resp.json(), "secure_host")

    def test_02_put_invalid_mode(self):
        resp = requests.put(URL, headers=HEADERS, json='invalid-mode')
        self.assertEqual(resp.status_code, 400)

    def test_03_put_passthrough(self):
        resp = requests.put(URL, headers=HEADERS, json='passthrough')
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(resp.json(), 'passthrough')

    def test_04_put_invalid2(self):
        # get mode before test starts
        resp = requests.get(URL)
        orig_mode = resp.json()
        # now set an invalid mode
        resp = requests.put(URL, headers=HEADERS, json='invalid2')
        self.assertEqual(resp.status_code, 400)
        
        # check mode - should not have changed.
        resp = requests.get(URL)
        self.assertEqual(resp.json(), orig_mode)
        
    def test_05_put_secure_lan(self):
        resp = requests.put(URL, headers=HEADERS, json='secure_lan')
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(resp.json(), 'secure_lan')

    def test_06_put_secure_host(self):
        resp = requests.put(URL, headers=HEADERS, json='secure_host')
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(resp.json(), 'secure_host')

    def test_get_mode(self):
        router_mode = os.path.join(DATA_PATH, 'data', 'default_target')
        resp = requests.get(URL)
        self.assertMatchesFirstLineOfFile(router_mode, resp.json())
        

if __name__ == '__main__':
    unittest.main(verbosity=2)
