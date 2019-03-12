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
URL = 'http://[::1]:4000/reboot'
HEADERS = {'Content-Type':'application/json'}
# use this directory as the root path for this test
DATA_PATH = os.path.join(TESTDIR, 'config', 'default')

################################################################################
### reboot_Test
### Test /reboot REST resource
################################################################################
class reboot_Test(test.SharedServer, test.IntegrationTestCase):

    @classmethod
    def setUpClass(cls, args=None):
        super().setUpClass(['-p', DATA_PATH])


    def setUp(self):
        super().setUp()
        # Wait for the embedded server to start up.
        time.sleep(1.1)
        self.last_reboot_file = os.path.join(DATA_PATH, 'sbin', 'last_reboot.txt')
        if os.path.isfile(self.last_reboot_file):
            os.remove(self.last_reboot_file)

    def tearDown(self):
        # delete the last_reboot file
        if os.path.isfile(self.last_reboot_file):
            os.remove(self.last_reboot_file)
        super().tearDown()

    def test_get_reboot(self):
        resp = requests.get(URL)
        self.assertEqual(resp.status_code, 405)
        self.assertFalse(os.path.isfile(self.last_reboot_file))

    def test_put_reboot(self):
        resp = requests.put(URL, headers=HEADERS, json="doesn't matter")
        self.assertEqual(resp.json(), 'Reboot Scheduled')
        time.sleep(5.5)
        self.assertTrue(os.path.isfile(self.last_reboot_file))

    def test_reboot_no_json(self):
        resp = requests.put(URL)
        self.assertEqual(resp.json(), 'Reboot Scheduled')
        time.sleep(5.5)
        self.assertTrue(os.path.isfile(self.last_reboot_file))
        

if __name__ == '__main__':
    unittest.main(verbosity=2)
