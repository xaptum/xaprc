import json
import os
import re
import shutil
import unittest

from xaptum import embedded_server

def fullmatch(regex, string, flags=0):
    """Emulate Python 3.4 re.fullmatch()."""
    m = re.match(regex, string, flags=flags)
    if m and m.span()[1] == len(string):
        return m

class EnrichedTestCase(unittest.TestCase):

    def assertDictMatches(self, expect, actual):
        """Fail if the content of the two dicts do not match. Values may match
        in two ways. First, if the values are equal. Second, the
        expected value is interpretted as a regular expression against
        which to match the actual value.
        """
        if len(expect) != len(actual):
            raise AssertionError("Size %s does not equal size %s"%(len(expect), len(actual)))
        for key in expect:
            if key not in actual:
                raise AssertionError("Missing expected key %s"%key)
            if isinstance(expect[key], dict):
                self.assertDictMatches(expect[key], actual[key])
            elif isinstance(expect[key], list):
                self.assertListMatches(expect[key], actual[key])
            else:
                if expect[key] == actual[key]:
                    continue
                elif fullmatch(expect[key], actual[key]):
                    continue
                else:
                    raise AssertionError("Value %s does not match %s for key %s"%(actual[key], expect[key], key))

    def assertListMatches(self, expect, actual):
        """Fail if the content of the two lists do not match. Values may match
        in two ways. First, if the values are equal. Second, the
        expected value is interpretted as a regular expression against
        which to match the actual value.
        """
        if len(expect) != len(actual):
            raise AssertionError("Size %s does not equal size %s"%(len(expect), len(actual)))
        for i, (e, a) in enumerate(zip(expect, actual)):
            if isinstance(e, dict):
                self.assertDictMatches(e, a)
            elif isinstance(e, list):
                self.assertListMatches(e, a)
            else:
                if e == a:
                    continue
                elif fullmatch(e, a):
                    continue
                else:
                    raise AssertionError("Value %s does not match %s at position %s"%(a, e, i))

    def assertMatchesFirstLineOfFile(self, expected_filename, val):
        """Fail if the first line of the file does not match the passed-in value.
        Only the first line of 'filename' is checked.
        """
        with open(expected_filename) as f:
            read_data = f.readline().strip('\n')
            self.assertEqual(val, read_data)

    def assertMatchesFileContents(self, expected_filename, val):
        """Fail if the content of the file does not match the passed-in value.
        The entire contents of the file is checked
        """
        with open(expected_filename) as f:
            read_data = f.read()
            self.assertEqual(val, read_data)

    def assertJsonFileEquals(self, expected_filename, actual):
        """Fail if the provided JSON object does not match that in the expected file.
        """
        with open(expected_filename) as f:
            filedata = f.read()
            jdata = json.loads(filedata)
            self.assertDictEquals(jdata, actual)

    def copyConfigDirDestructive(self, src_path, dest_path):
        if (os.path.isdir(dest_path)):
            shutil.rmtree(dest_path)

        shutil.copytree(src_path, dest_path)


import sys
if int(sys.version[0]) == 2:
    EnrichedTestCase.assertRaisesRegex = EnrichedTestCase.assertRaisesRegexp

class IntegrationTestCase(EnrichedTestCase):

    @classmethod
    def setUpClass(cls):
        super(IntegrationTestCase, cls).setUpClass()

    @classmethod
    def tearDownClass(cls):
        super(IntegrationTestCase, cls).tearDownClass()

class SharedServer(object):

    @classmethod
    def setUpClass(cls, args=None):
        super(SharedServer, cls).setUpClass()

        cls._server = embedded_server.Embedded_Server(args=args)
        cls._server.start()

    @classmethod
    def tearDownClass(cls):
        cls._server.kill()

        super(SharedServer, cls).tearDownClass()
