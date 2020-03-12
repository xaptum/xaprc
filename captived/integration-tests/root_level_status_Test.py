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

FIRMWARE_PREFIX = 'fw_env'
PRINT_FIRMWARE = 'fw_printenv'


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
        # test uses fake 'iw' command which returns the contents of wlan0.iw.response
        self.iw_response_file = os.path.join(DATA_PATH, 'sbin', 'wlan0.iw.response')
        self.mender_dir = os.path.join(DATA_PATH, 'etc', 'mender')

    def tearDown(self):
        super(root_level_status_Test, self).tearDown()

    # These two methods set up the responses from the mock fw_printenv
    def set_bootcount(self, num):
        with open(DATA_PATH + '/sbin/fw_env_bootcount', 'w') as f:
            f.write("bootcount={}\n".format(num))

    def set_upgrade_available(self, num):
        with open(DATA_PATH + '/sbin/fw_env_upgrade_available', 'w') as f:
            f.write("upgrade_available={}\n".format(num))

    def test_get_root(self):
        # use the connected configuration
        connected_file = os.path.join(DATA_PATH, 'sbin', 'wlan0.connected')
        shutil.copyfile(connected_file, self.iw_response_file)

        # use the dev model
        mender_data_dir = os.path.join(self.mender_dir, 'dev-test')
        print ('\nMender data dir = ', mender_data_dir)
        shutil.copyfile(mender_data_dir + '/artifact_info', self.mender_dir + '/artifact_info')
        
        self.set_bootcount(2)
        self.set_upgrade_available(0)

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

        # assert that the two firmware versions are equal - this also proves
        # that firmware.running_version is correct
        self.assertEqual(firmware_resp, jresp['firmware']['running_image'])

        # should be in state "normal"
        self.assertEqual('normal', jresp['firmware']['update_state'])

        model_resp = jresp['model']
        with open(mender_data_dir + '/model', 'r') as f:
            model_from_file = f.read().strip('\n')
        print ('Model retrieved from file = ', model_from_file)
        self.assertEqual(model_from_file, model_resp)

        self.assertIn('uptime', jresp)
        # test that wifi contains a status & status contains "connected"
        # We're not guaranteed to be able to retrieve IP addresses on test
        # machine because we're looking for `wlan0`
        self.assertIn('wifi', jresp)
        self.assertIn('status', jresp['wifi'])
        self.assertIn('connected', jresp['wifi']['status'])
        self.assertIn('SSID', jresp['wifi']['status'])
        self.assertEqual('test_wifi_1', jresp['wifi']['status']['SSID'],
                'SSID did not match expected value.')
        self.assertIn('IPv4_addresses', jresp['wifi']['status'])
        self.assertIn('IPv6_addresses', jresp['wifi']['status'])

if __name__ == '__main__':
    unittest.main(verbosity=2)
