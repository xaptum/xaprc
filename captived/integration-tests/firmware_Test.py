#!/usr/bin/env python3

import json
import os
import re
import requests
import shutil
import time
import unittest
from pathlib import Path

import xaptum.embedded_server as eserver
import xaptum.test as test

TESTDIR = os.getcwd()
CWD = os.path.abspath(os.path.join(os.pardir, os.pardir))
URL = 'http://[::1]:4000'
# use this directory as the root path for this test
DATA_PATH = os.path.join(TESTDIR, 'config', 'default')

HEADERS = {'Content-Type':'application/json'}
PRINT_FIRMWARE = 'fw_printenv'

MENDER_URI = 'http://test.server.com:1234/firmware_6.7.8.artifact'

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

    #
    # Resets the test environment
    # uses the connected configurations
    # uses the dev model/directory of mender config.
    def reset_test_connected_dev(self):
        # use the connected configuration
        connected_file = os.path.join(DATA_PATH, 'sbin', 'wlan0.connected')
        shutil.copyfile(connected_file, self.iw_response_file)

        # use the dev model
        mender_data_dir = os.path.join(self.mender_dir, 'dev-test')
        print ('\nMender data dir = ', mender_data_dir)
        shutil.copyfile(mender_data_dir + '/artifact_info', self.mender_dir + '/artifact_info')
        
        self.set_bootcount(2)
        self.set_upgrade_available(0)

        # The mock 'mender' script stops looping and indicates a completed
        # download if '/bin/stop' exists.  Delete it to reset.
        stop_file = DATA_PATH + '/bin/stop'
        if os.path.exists(stop_file):
            os.remove(stop_file)

    #
    # Stops mender by creating the 'stop' file.
    # Waits for two seconds for mock mender to cycle its wait time
    def stop_mender(self):
        stop_file = DATA_PATH + '/bin/stop'
        Path(stop_file).touch()
        time.sleep(2)


    def version_from_artifact_file(self):
        ver_regex = 'artifact_name=[\w-]+-v(\d+\.\d+\.\d+-\d+)-g[0-9a-hA-H]+$'
        with open(DATA_PATH + '/etc/mender/artifact_info') as f:
            firmware_file = f.readline().strip('\n')
            print ('firmware_file =', firmware_file)
            m1 = re.match(ver_regex, firmware_file)
            ver = m1.groups()
            if m1:
                return ver[0]
            return 'Not found'


    def image_name_from_artifact_file(self):
        with open(DATA_PATH + '/etc/mender/artifact_info') as f:
            firmware_file = f.readline().strip('\n')
            firmware_file = firmware_file.split('=')[1]
            return firmware_file

    def test_00_get_root(self):
        self.reset_test_connected_dev()
        resp = requests.get(URL)
        jresp = resp.json()
        print (json.dumps(jresp, indent=4))
        
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
        self.assertEqual(self.image_name_from_artifact_file(), firmware_resp)

        # assert that the two firmware versions are equal - this also proves
        # that firmware.running_version is correct
        self.assertEqual(firmware_resp, jresp['firmware']['running_image'])

        # should be in state "normal"
        self.assertEqual('normal', jresp['firmware']['update_state'])

        model_resp = jresp['model']
        mender_data_dir = os.path.join(self.mender_dir, 'dev-test')

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

    def test_10_get_firmware(self):
        self.reset_test_connected_dev()

        firmware_url = URL + '/firmware'
        resp = requests.get(firmware_url)
        jresp = resp.json()
        print (json.dumps(jresp, indent=4))

        self.assertIn('running_version', jresp)
        self.assertEqual(self.version_from_artifact_file(), jresp['running_version'])

        self.assertIn('update_state', jresp)
        self.assertEqual('normal', jresp['update_state'])

        self.assertNotIn('update_URI', jresp)

    def test_20_update_firmware(self):
        self.reset_test_connected_dev()
        firmware_url = URL + '/firmware'

        put_json = {'update_URI':MENDER_URI}
        resp = requests.put(firmware_url, headers=HEADERS, json=put_json)
        jresp = resp.json()
        print ("\n\n" + json.dumps(jresp, indent=4))

        self.assertIn('running_version', jresp)
        self.assertEqual(self.version_from_artifact_file(), jresp['running_version'])

        self.assertIn('update_state', jresp)
        self.assertEqual('downloading', jresp['update_state'])

        self.assertIn('update_URI', jresp)
        self.assertEqual(MENDER_URI, jresp['update_URI'])

        # sleep and test that mender is still downloading
        time.sleep(2)
        resp = requests.get(firmware_url)
        jresp = resp.json()
        self.assertIn('update_state', jresp)
        self.assertEqual('downloading', jresp['update_state'])

        # stop mender and test state
        self.stop_mender()
        resp = requests.get(firmware_url)
        jresp = resp.json()
        self.assertIn('update_state', jresp)
        self.assertEqual('downloaded', jresp['update_state'])

    def test_30_commit_firmware(self):
        self.reset_test_connected_dev()
        
        # set to appear as if system rebooted after download
        self.set_bootcount(1)
        self.set_upgrade_available(1)

        firmware_url = URL + '/firmware'

        # check for uncomitted
        resp = requests.get(firmware_url)
        jresp = resp.json()
        print (json.dumps(jresp, indent=4))

        self.assertIn('running_version', jresp)
        self.assertEqual(self.version_from_artifact_file(), jresp['running_version'])

        self.assertIn('update_state', jresp)
        self.assertEqual('uncommitted', jresp['update_state'])

        # test doesn't shut down server as reboot would, so 'update_URI'
        # still exists.
        # self.assertNotIn('update_URI', jresp)

        # now commit
        commit_url = URL + '/firmware/commit'
        put_json = 'garbage data'
        resp = requests.put(commit_url, headers=HEADERS, json=put_json)
        jresp = resp.json()
        print ("\nResponse from commit:")
        print (json.dumps(jresp, indent=4))
        self.assertEqual(resp.status_code, 200)

        # Verify that commit sent back a firmware object.
        self.assertIn('running_version', jresp)
        self.assertEqual(self.version_from_artifact_file(), jresp['running_version'])

        self.assertIn('update_state', jresp)
        self.assertEqual('normal', jresp['update_state'])


        # get the firmware and check it
        resp = requests.get(firmware_url)
        jresp = resp.json()
        print (json.dumps(jresp, indent=4))

        self.assertIn('running_version', jresp)
        self.assertEqual(self.version_from_artifact_file(), jresp['running_version'])

        self.assertIn('update_state', jresp)
        self.assertEqual('normal', jresp['update_state'])

        # self.assertNotIn('update_URI', jresp)

if __name__ == '__main__':
    unittest.main(verbosity=2)
