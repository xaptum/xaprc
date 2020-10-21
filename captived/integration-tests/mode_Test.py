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
DEFAULT_TARGET = os.path.join(DATA_PATH, 'data', 'systemd', 'system', 'default.target')

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
        self.last_reboot_file = os.path.join(DATA_PATH, 'sbin', 'last_reboot.txt')
        if os.path.isfile(self.last_reboot_file):
            os.remove(self.last_reboot_file)


    def tearDown(self):
        # put back to secure-host
        resp = requests.put(URL, headers=HEADERS, json='secure-host')

        # delete the last_reboot file
        if os.path.isfile(self.last_reboot_file):
            os.remove(self.last_reboot_file)

        super(mode_Test, self).tearDown()

    # This should get back the defult mode - which is secure-host
    def test_01_get_mode(self):
        resp = requests.get(URL)
        self.assertEqual(resp.json(), "secure-host")

    def test_02_put_invalid_mode(self):
        resp = requests.put(URL, headers=HEADERS, json='invalid-mode')
        self.assertEqual(resp.status_code, 400)

    def test_03_put_passthrough(self):
        resp = requests.put(URL, headers=HEADERS, json='passthrough')
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(resp.json(), 'passthrough')
        self.assertFalse(os.path.isfile(self.last_reboot_file))
        self.assertEqual(self.get_mode_from_default_target(), 'passthrough')

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

    @unittest.skip("secure-lan not supported by firmware")
    def test_05_put_secure_lan(self):
        resp = requests.put(URL, headers=HEADERS, json='secure-lan')
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(resp.json(), 'secure-lan')
        self.assertFalse(os.path.isfile(self.last_reboot_file))

    def test_06_put_secure_host(self):
        resp = requests.put(URL, headers=HEADERS, json='secure-host')
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(resp.json(), 'secure-host')
        self.assertFalse(os.path.isfile(self.last_reboot_file))
        self.assertEqual(self.get_mode_from_default_target(), 'secure-host')

    def test_get_mode(self):
        router_mode = self.get_mode_from_default_target()
        resp = requests.get(URL)
        self.assertEqual(router_mode, resp.json())
        
    # 
    # Gest the router mode from the default target file in /data/systemd/system
    #
    def get_mode_from_default_target(self):
        real_path = os.path.realpath(DEFAULT_TARGET)
        filename = os.path.basename(real_path)
        router_mode = os.path.splitext(filename)[0]
        return(router_mode)

if __name__ == '__main__':
    unittest.main(verbosity=2)
