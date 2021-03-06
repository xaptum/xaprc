#!/usr/bin/env python3

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
URL = 'http://[::1]:4000'
# use this directory as the root path for this test
DATA_PATH = os.path.join(TESTDIR, 'config', 'default')

HEADERS = {'Content-Type':'application/json'}

# This is the list of resources to test
# tuples are (file_path, REST_path)
check_these = [ ('/rom/serial', '/serial_number'),
                ('/rom/mac_address/1', '/mac_address/1'),
                ('/rom/mac_address/2', '/mac_address/2'),
                ('/rom/mac_address/3', '/mac_address/3'),
                ('/rom/mac_address/4', '/mac_address/4'),
                ('/data/enftun/enf1/address', '/control_address'),
                ('/data/enftun/enf0/address', '/data_address')
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
        self.mender_dir = os.path.join(DATA_PATH, 'etc', 'mender')


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

    def test_version_get(self):
        firmware_url = URL + '/firmware_version'
        resp = requests.get(firmware_url)
        with open (DATA_PATH + '/etc/mender/artifact_info') as f:
            firmware_file = f.readline().strip('\n').split('=')[1]
            self.assertEqual(firmware_file, resp.json())

    def test_version_put(self):
        firmware_url = URL + '/firmware_version'
        resp = requests.put(firmware_url, headers=HEADERS, json='junk-data')
        print ('PUT response for: ', firmware_url, '\n', resp.text)
        self.assertEqual(resp.status_code, 405)
        resp = requests.get(firmware_url)
        print ('GET response for: ', firmware_url, '\n', resp.text)
        self.assertNotEqual('junk-data', resp.json())

    def test_model_get_dev(self):
        # copy dev file
        mender_data_dir = os.path.join(self.mender_dir, 'dev-test')
        print ('Mender data dir = ', mender_data_dir)
        shutil.copyfile(mender_data_dir + '/artifact_info', 
                        self.mender_dir + '/artifact_info')

        model_url = URL + '/model'
        resp = requests.get(model_url)
        with open(mender_data_dir + '/model', 'r') as f:
            model_from_file = f.read().strip('\n')

        print ('Model retrieved from file = ', model_from_file)
        self.assertEqual(model_from_file, resp.json())

    def test_model_get_prod(self):
        # copy dev file
        mender_data_dir = os.path.join(self.mender_dir, 'prod-test')
        print ('Mender data dir = ', mender_data_dir)
        shutil.copyfile(mender_data_dir + '/artifact_info', 
                        self.mender_dir + '/artifact_info')

        model_url = URL + '/model'
        resp = requests.get(model_url)
        with open(mender_data_dir + '/model', 'r') as f:
            model_from_file = f.read().strip('\n')

        print ('Model retrieved from file = ', model_from_file)
        self.assertEqual(model_from_file, resp.json())

    def test_model_put(self):
        model_url = URL + '/model'
        resp = requests.put(model_url, headers=HEADERS, json='junk-data')
        print ('PUT response for: ', model_url, '\n', resp.text)
        self.assertEqual(resp.status_code, 405)
        resp = requests.get(model_url)
        print ('GET response for: ', model_url, '\n', resp.text)
        self.assertNotEqual('junk-data', resp.json())


if __name__ == '__main__':
    unittest.main(verbosity=2)
