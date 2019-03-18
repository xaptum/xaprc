#!/usr/bin/env python3

import hashlib
import json
import os
import requests
import shutil
import time
import unittest

import xaptum.embedded_server as eserver
import xaptum.test as test

TESTDIR = os.getcwd()
CWD = os.path.abspath(os.path.join(os.pardir, os.pardir))
URL = 'http://[::1]:4000/uptime'
# use this directory as the root path for this test
DATA_PATH = os.path.join(TESTDIR, 'config', 'default')

HEADERS = {'Content-Type':'application/json'}
FILE_UPTIME = '/proc/uptime'

class uptime_Test(test.SharedServer, test.IntegrationTestCase):

    @classmethod
    def setUpClass(cls, args=None):
        super().setUpClass(['-p', DATA_PATH])

    def setUp(self):
        super().setUp()

    def tearDown(self):
        super().tearDown()

    def test_get_uptime(self):
        resp = requests.get(URL)
        self.assertAlmostEqual(3442785.53, resp.json())

if __name__ == '__main__':
    unittest.main(verbosity=2)
