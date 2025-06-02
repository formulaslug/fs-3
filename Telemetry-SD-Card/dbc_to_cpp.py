import cantools
import pyarrow as pa
import numpy as np

DEFAULT_FLOAT_TOLERANCE = 0.1

# Find the appropriate type to represent a given signal.
# - If length==1, bool is used
# - If scale==1, the correctly sized integer type is used
# - Otherwise, choose_float_type() is used
# See choose_float_type() for a description of float_tolerance
def get_arrow_type_for_signal(
    s: cantools.db.Signal, float_tolerance: float = DEFAULT_FLOAT_TOLERANCE
) -> pa.DataType:
    if s.length == 1 and s.scale == 1 and s.offset == 0:
        return pa.bool_()

    raw_min = -(1 << (s.length - 1)) if s.is_signed else 0
    raw_max = (1 << (s.length - 1)) - 1 if s.is_signed else (1 << s.length) - 1

    if s.scale == 1:
        decoded_min = raw_min
        decoded_max = raw_max
        candidates = [
            (pa.uint8(), 0, 255),
            (pa.int8(), -128, 127),
            (pa.uint16(), 0, 65535),
            (pa.int16(), -32768, 32767),
            (pa.uint32(), 0, 2**32 - 1),
            (pa.int32(), -(2**31), 2**31 - 1),
        ]
        for arrow_type, min_val, max_val in candidates:
            if decoded_min >= min_val and decoded_max <= max_val:
                return arrow_type
        return pa.int64() if s.is_signed else pa.uint64()

    return choose_float_type(s, float_tolerance)


# Find the appropriate float type to represent the signal within the given
# tolerance to the signal's scale. Determined by testing if every possible real
# (scaled) value is representable within tolerance. Eg: if tolerance is 0.5
# (50%), then any float whose value is within 50%*(signal's scale) of the actual
# value is considered close enough
def choose_float_type(s: cantools.db.Signal, tolerance: float):
    raw_min = -(1 << (s.length - 1)) if s.is_signed else 0
    raw_max = (1 << (s.length - 1)) - 1 if s.is_signed else (1 << s.length) - 1

    # Interpret min/max as integers, create a range of every integer in between
    possible_raw_values = np.arange(raw_min, raw_max + 1)
    # Scale that range of integers to get every possible decimal value
    possible_decoded_values = possible_raw_values * s.scale + s.offset

    # Make sure every possible decimal value is representable by some float
    # type, up to a precision of 10% of the signal's scale
    atol = abs(s.scale) * tolerance
    for dtype, arrow_type in [
        (np.float16, pa.float16()),
        (np.float32, pa.float32()),
        (np.float64, pa.float64()),
    ]:
        # if signal.name == "ACC_SEG4_VOLTS_CELL1" and arrow_type == pa.float32():
        #     print(possible_decoded_values.astype(dtype))

        # If every possibel real value is close enough using a certain float
        # type, choose that float type. rtol is 0 as we don't care about
        # relative tolerance, only tolerance based on the signal's scale
        if np.allclose(
            possible_decoded_values,
            possible_decoded_values.astype(dtype),
            rtol=0,
            atol=atol,
        ):
            return arrow_type

    # Fallback if not exactly representable within 5% tolerance of scale (rare)
    print(
        f"Warning: Signal ${s.name} not representable as 64 bit float within 5% tolerance; ignoring!"
    )
    return pa.float64()


if __name__ == "__main__":
    # for debugging purposes
    np.set_printoptions(formatter={'float_kind':"{:.8f}".format})

    db = cantools.db.Database()

    db.add_dbc_file("../CANbus.dbc")

    # signal_to_datatype: dict[str, pa.DataType] = {}
    signal_to_datatype = {}

    for msg in db.messages:
        for signal in msg.signals:
            signal_to_datatype[signal.name] = (
                get_arrow_type_for_signal(signal),
                # signal.length,
                # signal.offset,
                # signal.scale,
                # signal.minimum,
                # signal.maximum,
            )

    # __import__("pprint").pprint(signal_to_datatype)
    for k, v in signal_to_datatype.items():
        v: str = str(v)
        v = v.removeprefix("(DataType(")
        v = v.removesuffix("),)")
        print("{:33s} {}".format(k, v))
