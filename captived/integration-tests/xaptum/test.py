import re
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

    @classmethod
    def getTUN(cls):
        return cls._tun
