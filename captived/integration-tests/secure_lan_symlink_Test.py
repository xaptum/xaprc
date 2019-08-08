#!/usr/bin/env python3

import hashlib
import json
import os
import requests
import shutil
import time
import unittest
import stat

import xaptum.embedded_server as eserver
import xaptum.test as test

TESTDIR = os.getcwd()
CWD = os.path.abspath(os.path.join(os.pardir, os.pardir))
URL_PASSTHROUGH = 'http://[::1]:4000/wifi/config/passthrough'
URL_SECURE = 'http://[::1]:4000/wifi/config/secure'
# use this directory as the root path for this test
DATA_PATH = os.path.join(TESTDIR, 'config', 'default')
LINK_TARGET = os.path.join(DATA_PATH, 'data', 'connman', 'secure-host')

HEADERS = {'Content-Type':'application/json'}
FILE_WIFI_CONFIG_PASSTHROUGH = '/data/connman/passthrough/wifi.config'
FILE_WIFI_CONFIG_SECURE_HOST = '/data/connman/secure-host/wifi.config'
FILE_WIFI_CONFIG_SECURE_LAN = '/data/connman/secure-lan/wifi.config'

new_config = 'ctrl_interface=/var/run/wpa_supplicant\nap_scan=1\n\nnetwork={\nssid="customer_private"\npsk="none"\n}'

################################################################################
### secure_lan_symlink_Test
### Test /wifi/config REST resource
################################################################################
class secure_lan_symlink_Test(test.SharedServer, test.IntegrationTestCase):

    @classmethod
    def setUpClass(cls, args=None):
        super(secure_lan_symlink_Test, cls).setUpClass(['-p', DATA_PATH])

    def setUp(self):
        super(secure_lan_symlink_Test, self).setUp()
        self.symlink = os.path.join(DATA_PATH, 'data', 'connman', 'secure-lan')

        self.last_restart_file = os.path.join(DATA_PATH, 'bin', 'connman.service_restart.txt')
        if os.path.isfile(self.last_restart_file):
            os.remove(self.last_restart_file)

    def tearDown(self):
        # delete the last_reboot file
        if os.path.isfile(self.last_restart_file):
            os.remove(self.last_restart_file)
        super(secure_lan_symlink_Test, self).tearDown()

    # Starting condition : symlink did not exist
    def test_symlink_didnt_exist(self):
        secure_lan_symlink_Test.stopServer()
        self.remove_file(self.symlink)
        secure_lan_symlink_Test.startServer()
        self.assertTrue(os.path.islink(self.symlink))
        self.assertEqual(LINK_TARGET, os.readlink(self.symlink))

    # Starting condition : correct symlink already existed
    def test_symlink_was_correct(self):
        self.assertTrue(os.path.islink(self.symlink),
                'secure-lan was not a link when the test started.')
        self.assertEqual(LINK_TARGET, os.readlink(self.symlink),
                'secure-lan was not linked to secure-host when the test started')
        last_mod_time = os.path.getmtime(self.symlink)
        secure_lan_symlink_Test.stopServer()
        secure_lan_symlink_Test.startServer()
        self.assertTrue(os.path.islink(self.symlink))
        self.assertEqual(LINK_TARGET, os.readlink(self.symlink))
        self.assertEqual(last_mod_time, os.path.getmtime(self.symlink))

    # Starting condition: link to the wrong target
    def test_symlink_was_wrong(self):
        secure_lan_symlink_Test.stopServer()
        self.remove_file(self.symlink)
        passthrough = os.path.join(DATA_PATH, 'data', 'connman', 'passthrough')
        os.symlink(passthrough, self.symlink)
        self.assertTrue(os.path.islink(self.symlink), 
                'test setup failed to create a symbolic link')
        secure_lan_symlink_Test.startServer()
        self.assertTrue(os.path.islink(self.symlink))
        self.assertEqual(LINK_TARGET, os.readlink(self.symlink))

    # Starting condition: secure-lan is a directory
    def test_secure_lan_was_directory(self):
        secure_lan_symlink_Test.stopServer()
        self.remove_file(self.symlink)
        os.mkdir(self.symlink)
        self.assertTrue(os.path.isdir(self.symlink),
                'test setup failed to create a directory')
        secure_lan_symlink_Test.startServer()
        self.assertTrue(os.path.islink(self.symlink))
        self.assertEqual(LINK_TARGET, os.readlink(self.symlink))

    # Starting condition: secure-lan is a file
    def test_secure_lan_was_file(self):
        secure_lan_symlink_Test.stopServer()
        self.remove_file(self.symlink)
        #os.mknod(self.symlink, stat.S_IFREG)
        output = open(self.symlink, 'w')
        output.write('Hello World\n')
        output.close()

        self.assertTrue(os.path.isfile(self.symlink),
                'test setup failed to create a file.')
        secure_lan_symlink_Test.startServer()
        self.assertTrue(os.path.islink(self.symlink))
        self.assertEqual(LINK_TARGET, os.readlink(self.symlink))


    def remove_file(self, fq_filename):
        if os.path.islink(fq_filename):
            print (fq_filename, " existed as symlink - removing")
            os.remove(fq_filename)
        elif os.path.isfile(fq_filename):
            print (fq_filename, " existed as file - removing")
            os.remove(fq_filename)
        elif os.path.isdir(fq_filename):
            print (fq_filename, " existed as directory - removing")
            os.removedirs(fq_filename)
        elif os.path.lexists (fq_filename):
            print (fq_filename, " exists, but is unknown type")
            os.removed(fq_filename)
        else:
            print (fq_filename, " did not exist.")



    def run_put_test(self, url, wifi_config_file):
        # get and store value to put back
        resp = requests.get(url)
        orig_json_config = resp.json()
        new_json = {'contents':new_config}
        resp = requests.put(url, headers=HEADERS, json=new_json)
        self.assertEqual(new_config, resp.json()['contents'].strip('\n'))
        self.assertTrue(os.path.isfile(self.last_restart_file))

        # check that the file was just created
        modified_time = os.path.getmtime(self.last_restart_file)
        cur_time = time.time()
        self.assertLess(cur_time - modified_time, 3.0)

        # put the original values back
        resp = requests.put(url, headers=HEADERS, json=orig_json_config)
        self.assertNotEqual(new_config, resp.json()['contents'])
        self.assertMatchesFileContents(DATA_PATH + wifi_config_file,
                        resp.json()['contents'])

        # check that the restart-file was modified again
        self.assertLess(modified_time, os.path.getmtime(self.last_restart_file))


if __name__ == '__main__':
    unittest.main(verbosity=2)
