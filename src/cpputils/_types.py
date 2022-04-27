from datetime import datetime, timedelta


def _check_milliseconds_field(millisecond):
    if not 0 <= millisecond <= 999:
        raise ValueError("microsecond must be in 0..999", millisecond)


class DatetimeMS(datetime):
    @classmethod
    def __new__(
        cls,
        year: int,
        month: int = None,
        day: int = None,
        hour: int = 0,
        minute: int = 0,
        second: int = 0,
        millisecond: int = 0,
        tzinfo: timedelta = None,
        *,
        fold=0
    ):
        _check_milliseconds_field(millisecond)
        return datetime.__new__(
            cls, year, month, day, hour, minute, second, millisecond * 1000, tzinfo
        )

    @property
    def millisecond(self):
        return int(self.microsecond / 1000)

    def __str__(self):
        """Convert to string, for str()."""
        return self.isoformat(sep=" ")[:-3]
