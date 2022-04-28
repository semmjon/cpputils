from datetime import datetime


def _check_milliseconds_field(millisecond):
    if not 0 <= millisecond <= 999:
        raise ValueError("microsecond must be in 0..999", millisecond)


class datetime_ms(datetime):
    def __new__(
        cls,
        year,
        month=None,
        day=None,
        hour=0,
        minute=0,
        second=0,
        millisecond=0,
        tzinfo=None,
        *,
        fold=0
    ):
        _check_milliseconds_field(millisecond)
        self = datetime.__new__(
            cls, year, month, day, hour, minute, second, millisecond * 1000, tzinfo
        )
        return self

    @property
    def millisecond(self):
        return int(self.microsecond / 1000)

    def __str__(self):
        "Convert to string, for str()."
        return self.isoformat(sep=" ")[:-3]
