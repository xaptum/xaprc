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
URL = 'http://[::1]:4000/mode'
HEADERS = {'Content-Type':'application/json'}

################################################################################
### rest_mode_Test
### Test getting and putting the router card mode.
################################################################################
class rest_mode_Test(test.SharedServer, test.IntegrationTestCase):

    def setUp(self):
        super(rest_mode_Test, self).setUp()
        
        # copy the default config directory 
        # currently, testing will only work with debug build because of config
        # file paths.
        src_path = os.path.join(TESTDIR, 'config', 'default')
        self.config_path = os.path.join(CWD, 'config')
        copy_recursive_destructive(src_path, self.config_path)

        # Wait for the client connection to be processed by the XMB
        time.sleep(2.1)

    def tearDown(self):
        # put back to secure_host
        resp = requests.put(URL, headers=HEADERS, json='secure_host')
        super(rest_mode_Test, self).tearDown()

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
        

def copy_recursive_destructive(src_path, dest_path):
    if (os.path.isdir(dest_path)):
        shutil.rmtree(dest_path)

    shutil.copytree(src_path, dest_path)


if __name__ == '__main__':
    unittest.main(verbosity=2)
