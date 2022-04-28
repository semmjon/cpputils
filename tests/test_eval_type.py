import sys
import unittest
from datetime import datetime, timedelta, timezone

from cpputils import datetime_ms, eval_type


class TestEvalType(unittest.TestCase):
    def test_integer(self):
        [
            self.assertEqual(eval_type(str(x)), x)
            for x in [*range(10), sys.maxsize, -sys.maxsize]
        ]

    def test_float(self):
        [
            self.assertEqual(eval_type(str(x)), x)
            for x in [0.1, 1.0, 0.0, -0.1, 0.00000000000000000001]
        ]

    def test_timestamp_formats(self):
        [
            self.assertTrue(type(eval_type(str(x))) in [datetime, datetime_ms])
            for x in [
                "20060317 13:27:54.123",
                "2006/03/17 13:27:54.123",
                "17/03/2006 13:27:54.123",
                "20060317 13:27:54",
                "2006/03/17 13:27:54",
                "17/03/2006 13:27:54",
                "2006-03-17 13:27:54.123",
                "17-03-2006 13:27:54.123",
                "2006-03-17 13:27:54",
                "17-03-2006 13:27:54",
                "2006-03-17T13:27:54",
                "2006-03-17T13:27:54.123",
                "20060317T13:27:54",
                "20060317T13:27:54.123",
                "17-03-2006T13:27:54.123",
                "17-03-2006T13:27:54",
                "2006-03-17T13:27:54Z",
                "2006-03-17T13:27:54+03:45",
                "2006-03-17T13:27:54-05:37",
                "2006-03-17T13:27Z",
                "2006-03-17T13:27+03:45",
                "2006-03-17T13:27-05:37",
                "17/Mar/2006:13:27:54 -0537",
                "17/Mar/2006:13:27:54 +0537",
                "Sat, 17 Mar 2006 13:27:54 GMT",
                "Sat, 17 Mar 2006 13:27:54 EST",
                "Sat, 17 Mar 2006 13:27:54 UT",
                "Sat, 17 Mar 2006 13:27:54 M",
                "Sat, 17 Mar 2006 13:27:54 -0234",
                "Sat, 17 Mar 2006 13:27:54 +0325",
            ]
        ]

    def test_timestamp_values(self):
        self.assertEqual(
            str(eval_type("Sat, 17 Mar 2006 13:27:54 +0325")),
            "2006-03-17 13:27:54+03:25",
        )
        self.assertEqual(
            str(eval_type("20060317 13:27:54.123")), "2006-03-17 13:27:54.123000+00"
        )
        self.assertEqual(
            str(eval_type("Sat, 17 Mar 2006 13:27:54 EST")), "2006-03-17 13:27:54-05:00"
        )
        self.assertEqual(eval_type("Sat, 17 Mar 2006 13:27:54 -0234").year, 2006)
        self.assertEqual(eval_type("Sat, 17 Mar 2006 13:27:54 -0234").day, 17)
        self.assertEqual(eval_type("Sat, 17 Mar 2006 13:27:54 -0234").month, 3)
        self.assertEqual(eval_type("Sat, 17 Mar 2006 13:27:54 -0234").hour, 13)
        self.assertEqual(eval_type("Sat, 17 Mar 2006 13:27:54 -0234").minute, 27)
        self.assertEqual(eval_type("Sat, 17 Mar 2006 13:27:54 -0234").second, 54)
        self.assertEqual(eval_type("Sat, 17 Mar 2006 13:27:54 -0234").microsecond, 0)
        days = [
            "Monday",
            "Tuesday",
            "Wednesday",
            "Thursday",
            "Friday",
            "Saturday",
            "Sunday",
        ]
        self.assertEqual(
            days[eval_type("Sat, 17 Mar 2006 13:27:54 -0234").isoweekday()], "Saturday"
        )
        self.assertEqual(
            eval_type("Sat, 17 Mar 2006 13:27:54 -0234").tzinfo,
            timezone(timedelta(-1, 77160)),
        )
