import unittest

from cpputils import eval_type


class TestEvalType(unittest.TestCase):
    def test_integer(self):
        self.assertEqual(type(eval_type("0")), int)
