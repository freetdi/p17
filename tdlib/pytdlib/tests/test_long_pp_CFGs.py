import tdlib
import unittest
import sys

if(len(sys.argv)<2 or sys.argv[1]!="long"):
	sys.exit(77)

sys.argv=sys.argv[:1]

from graphs import *
import CFGs

PREFIX = "CFGs"
COUNT = 1816

class TestTdLib_packages(unittest.TestCase):
    def test_CFGs_PP(self):
        i=132
        print i;
        G = Graph(eval(PREFIX+".V_"+str(i)), eval(PREFIX+".E_"+str(i)))
        T, w = tdlib.PP(G)

        for i in range(0, COUNT+1):
            G = Graph(eval(PREFIX+".V_"+str(i)), eval(PREFIX+".E_"+str(i)))
            T, w = tdlib.PP(G)
            print i, w

if __name__ == '__main__':
    unittest.main()

# vim:ts=8:sw=4:et
