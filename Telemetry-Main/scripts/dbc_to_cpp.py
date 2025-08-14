import cantools
import numpy as np
from pathlib import Path

# See choose_best_float_type() for explanation
DEFAULT_FLOAT_TOLERANCE = 0.1

FSDAQ_TYPE_TO_C_TYPE = {
    "b0": "bool",
    "u3": "uint8_t",
    "i3": "int8_t",
    "u4": "uint16_t",
    "i4": "int16_t",
    "u5": "uint32_t",
    "i5": "int32_t",
    "u6": "uint64_t",
    "i6": "int64_t",
    "f5": "float",
    "f6": "double",
}

C_TYPE_TO_TYPE_SUFFIX = {
    "bool": "",
    "uint8_t": "u",
    "int8_t": "",
    "uint16_t": "u",
    "int16_t": "",
    "uint32_t": "u",
    "int32_t": "",
    "int64_t": "ll",
    "uint64_t": "ull",
    "float": "f",
    "double": "",
}

fsdaq_folder_path = Path(__file__).parent.parent / "fsdaq"


# Find the appropriate type to represent a given signal.
# - If length==1, bool is used
# - If scale==1, the correctly sized integer type is used
# - Otherwise, choose_best_float_type() is used
# See choose_best_float_type() for a description of float_tolerance
def get_fsdaq_type_for_dbc_signal(
    s: cantools.db.Signal, float_tolerance: float = DEFAULT_FLOAT_TOLERANCE
) -> str:
    if s.length == 1 and s.scale == 1 and s.offset == 0:
        return "b0"

    raw_min = -(1 << (s.length - 1)) if s.is_signed else 0
    raw_max = (1 << (s.length - 1)) - 1 if s.is_signed else (1 << s.length) - 1

    if s.scale == 1:
        decoded_min = raw_min
        decoded_max = raw_max
        candidates = [
            ("u3", 0, 255),
            ("i3", -128, 127),
            ("u4", 0, 65535),
            ("i4", -32768, 32767),
            ("u5", 0, 2**32 - 1),
            ("i5", -(2**31), 2**31 - 1),
        ]
        for fsdaq_type, min_val, max_val in candidates:
            if decoded_min >= min_val and decoded_max <= max_val:
                return fsdaq_type
        return "i5" if s.is_signed else "u5"

    return choose_best_float_type(s, float_tolerance)


# Find the appropriate float type to represent the signal within the given
# tolerance to the signal's scale. Determined by testing if every possible real
# (scaled) value is representable within tolerance. Eg: if tolerance is 0.5
# (50%), then any float whose value is within 50%*(signal's scale) of the actual
# value is considered close enough
def choose_best_float_type(s: cantools.db.Signal, tolerance: float):
    raw_min = -(1 << (s.length - 1)) if s.is_signed else 0
    raw_max = (1 << (s.length - 1)) - 1 if s.is_signed else (1 << s.length) - 1

    # Interpret min/max as integers, create a range of every integer in between
    possible_raw_values = np.arange(raw_min, raw_max + 1)
    # Scale that range of integers to get every possible decimal value
    possible_decoded_values = possible_raw_values * s.scale + s.offset

    # Make sure every possible decimal value is representable by some float
    # type, up to a precision of 10% of the signal's scale
    atol = abs(s.scale) * tolerance
    for dtype, fsdaq_type in [
        # (np.float16, "f?"),
        (np.float32, "f5"),
        (np.float64, "f6"),
    ]:
        # If every possibel real value is close enough using a certain float
        # type, choose that float type. rtol is 0 as we don't care about
        # relative tolerance, only tolerance based on the signal's scale
        if np.allclose(
            possible_decoded_values,
            possible_decoded_values.astype(dtype),
            rtol=0,
            atol=atol,
        ):
            return fsdaq_type

    # Fallback if not exactly representable within 5% tolerance of scale (rare)
    print(
        f"Warning: Signal {s.name} not representable as 64 bit float within {tolerance} tolerance; ignoring!"
    )
    return "f6"


def generate_encoder_hpp(db: cantools.db.Database, rows_per_batch: int = 80):
    out_file = open(fsdaq_folder_path / "encoder_generated.hpp", "w")
    template_file = open(fsdaq_folder_path / "encoder_generated.hpp.in", "r")
    template = template_file.read()

    assert rows_per_batch % 8 == 0

    signal_to_fsdaq_datatype: dict[str, str] = {}

    for msg in db.messages:
        for signal in msg.signals:
            signal_to_fsdaq_datatype[signal.name] = get_fsdaq_type_for_dbc_signal(
                signal
            )
            # print(signal.name)

    template = template.replace("@COLS@", str(len(signal_to_fsdaq_datatype)))
    template = template.replace("@ROWS_PER_BATCH@", str(rows_per_batch))

    # fmt: off
    col_names = ", ".join(['"' + col + '"' for col in signal_to_fsdaq_datatype.keys()])
    col_name_sizes = ", ".join([str(len(col_name)) for col_name in signal_to_fsdaq_datatype.keys()])
    col_name_types = ", ".join(['"' + fsdaq_type + '"' for fsdaq_type in signal_to_fsdaq_datatype.values()])
    # fmt: on
    template = template.replace("@COL_NAMES@", col_names)
    template = template.replace("@COL_NAME_SIZES@", col_name_sizes)
    template = template.replace("@COL_NAME_TYPES@", col_name_types)

    batch_struct_fields = []
    batch_row_struct_fields = []
    update_fields_from_row = []
    # fmt: off
    for col_name, fsdaq_type in signal_to_fsdaq_datatype.items():
        if fsdaq_type == "b0":
            batch_struct_fields.append(" "*4 + f"uint8_t {col_name}[ROWS_PER_BATCH/8];")
            update_fields_from_row.append(" "*8 + f"this->{col_name}[idx/8] |= row.{col_name} << idx%8;")
        else:
            batch_struct_fields.append(" "*4 + FSDAQ_TYPE_TO_C_TYPE[fsdaq_type] + f" {col_name}[ROWS_PER_BATCH];")
            update_fields_from_row.append(" "*8 + f"this->{col_name}[idx] = row.{col_name};")
        batch_row_struct_fields.append(" "*4 + FSDAQ_TYPE_TO_C_TYPE[fsdaq_type] + f" {col_name};")
    template = template.replace("@DATA_BATCH_STRUCT_FIELDS@", "\n".join(batch_struct_fields))
    template = template.replace("@DATA_ROW_STRUCT_FIELDS@", "\n".join(batch_row_struct_fields))

    template = template.replace("@UPDATE_FIELDS_FROM_ROW@", "\n".join(update_fields_from_row))
    # fmt: on

    out_file.write(template)
    out_file.close()
    template_file.close()


def generate_can_processor_hpp(db: cantools.db.Database):
    out_file = open(fsdaq_folder_path / "can_processor_generated.hpp", "w")
    template_file = open(fsdaq_folder_path / "can_processor_generated.hpp.in", "r")
    template = template_file.read()

    can_message_ids = []
    for msg in db.messages:
        can_message_ids.append(" " * 4 + f"{msg.name} = {hex(msg.frame_id)},")

    template = template.replace("@MESSAGE_IDS@", "\n".join(can_message_ids))

    out_file.write(template)
    out_file.close()
    template_file.close()


# The following contains logic taken from cantools:
# https://github.com/cantools/cantools/blob/master/src/cantools/database/can/c_source.py#L753
def calculate_signal_unpacking_segments(signal: cantools.db.Signal):
    segments = []
    byte_index, bit_offset = divmod(signal.start, 8)
    bits_left = signal.length

    while bits_left > 0:
        if signal.byte_order == "big_endian":
            if bits_left >= (bit_offset + 1):
                length = bit_offset + 1
                bit_offset = 7
                shift = -(bits_left - length)
                mask = (1 << length) - 1
            else:
                length = bits_left
                shift = bit_offset - length + 1
                mask = (1 << length) - 1
                mask <<= bit_offset - length + 1
        else:
            shift = (bits_left - signal.length) + bit_offset

            if bits_left >= (8 - bit_offset):
                length = 8 - bit_offset
                mask = (1 << length) - 1
                mask <<= bit_offset
                bit_offset = 0
            else:
                length = bits_left
                mask = (1 << length) - 1
                mask <<= bit_offset

        if shift < 0:
            shift = -shift
            shift_direction = "left"
        else:
            shift_direction = "right"

        segments.append((byte_index, shift, shift_direction, mask))

        bits_left -= length
        byte_index += 1

    return segments


# The following contains logic taken from cantools:
# https://github.com/cantools/cantools/blob/master/src/cantools/database/can/c_source.py#L753
def generate_can_processor_cpp(db: cantools.db.Database):
    out_file = open(fsdaq_folder_path / "can_processor_generated.cpp", "w")
    template_file = open(fsdaq_folder_path / "can_processor_generated.cpp.in", "r")
    template = template_file.read()

    can_message_processing = []
    for msg in db.messages:
        msg_processing_lines = []
        variable_lines = []
        for signal in msg.signals:
            # if signal.is_float:
            #     raise ValueError("can't handle float signals yet!")
            # # msg_processing_lines.append(" "*8 + f"current_row->{signal.name} = 100;")
            # raw_buf_type = f"{signal.is_signed and "" or "u"}int{8 * math.ceil(signal.length / 8)}_t"

            # end_byte_offset = math.floor(signal.start + signal.length / 8)

            fsdaq_type = get_fsdaq_type_for_dbc_signal(signal)
            fsdaq_type_length = 2 ** int(fsdaq_type[1])
            if fsdaq_type_length < 8:
                fsdaq_type_length = 8

            segments = calculate_signal_unpacking_segments(signal)
            # The cantools logic only checks if is_float or is_signed; but since
            # we're also applying scale and offset inline, which can't be done
            # across multiple byte segments, we have to check len(segments) too
            needs_conversion_variable = (
                len(segments) > 1 or signal.conversion.is_float or signal.is_signed
            )

            # fmt: off
            conversion_type_length = (
                8 if signal.length <= 8
                else 16 if signal.length <= 16
                else 32 if signal.length <= 32
                else 64
            ) # fmt: on

            conversion_type_name = f'uint{conversion_type_length}_t'

            if needs_conversion_variable:
                variable = " "*8 + f'{conversion_type_name} {signal.name};'
                variable_lines.append(variable)

            for i, (index, shift, shift_direction, mask) in enumerate(segments):
                unpack_fmt = "({var_type})(({var_type})(msg.data[{src_idx}] & 0x{mask:02x}u) {shift_op} {shift_count}u)"
                line_fmt = " "*8 + "{lhs} {op} {unpack}{scale_offset}; // {type_info}"

                scale_offset = (f" * {signal.scale}" if signal.scale != 1 else "") + (f" + {signal.offset}" if signal.offset != 0 else "")

                line = line_fmt.format(
                    lhs = ("" if needs_conversion_variable else "current_row->") + signal.name,
                    op = "=" if i == 0 else "|=",
                    unpack = unpack_fmt.format(
                        var_type=f"uint{conversion_type_length}_t",
                        src_idx=index,
                        mask=mask,
                        shift_op="<<" if shift_direction=="left" else ">>",
                        shift_count=shift,
                    ),
                    scale_offset = "" if needs_conversion_variable else scale_offset,
                    type_info = FSDAQ_TYPE_TO_C_TYPE[fsdaq_type] + " *" + str(signal.scale) + " +" + str(signal.offset),
                )
                msg_processing_lines.append(line)

            if signal.conversion.is_float:
                conversion = " "*8 + f'memcpy(&current_row->{signal.name}, &{signal.name}, sizeof(current_row->{signal.name}));'
                msg_processing_lines.append(conversion)
            elif signal.is_signed:
                mask = ((1 << (conversion_type_length - signal.length)) - 1)
                # If raw integer length is different than final integer variable
                # length, it requires a sign-extension
                if mask != 0:
                    mask <<= signal.length
                    sign_extension = (
                        " "*8 + "if (({name} & (1{suffix} << {shift})) != 0{suffix}) {"
                        " "*8 + "    {name} |= 0x{mask:x}{suffix};"
                        " "*8 + "}\n"
                    ).format(
                        name=signal.name,
                        shift=signal.length - 1,
                        mask=mask,
                        suffix={ 8: 'u', 16: 'u', 32: 'u', 64: 'ull' }[conversion_type_length],
                    )
                    msg_processing_lines.extend(sign_extension.splitlines())

                conversion = " "*8 + f'current_row->{signal.name} = (int{conversion_type_length}_t){signal.name};'
                msg_processing_lines.append(conversion)

            # msg_processing_lines.append(" "*8 + f"{raw_buf_type} raw_{signal.name};");

            msg_processing_lines.append("");
        msg_processing_lines.append(" " * 8 + "break;")

        can_message_processing.append(" " * 4 + f"case {msg.name}:")
        if len(variable_lines) > 0:
            can_message_processing.append("\n".join(variable_lines) + "\n")
        can_message_processing.append("\n".join(msg_processing_lines))
        can_message_processing.append("")

    template = template.replace(
        "@CAN_MESSAGE_PROCESSING@", "\n".join(can_message_processing)
    )

    out_file.write(template)
    out_file.close()
    template_file.close()


if __name__ == "__main__":
    # for debugging purposes:
    np.set_printoptions(formatter={"float_kind": "{:.8f}".format})

    db = cantools.db.Database()
    db.add_dbc_file("../CANbus.dbc")

    generate_encoder_hpp(db, rows_per_batch=80)
    generate_can_processor_hpp(db)
    generate_can_processor_cpp(db)


# OLD NANOARROW GENERATION CODE:
#
# def generate_nanoarrow_code(signal_to_datatype: dict[str, pa.DataType]):
#     with open("./nanoarrow_generated_from_dbc.hpp", "w") as f:
#         cols = len(signal_to_datatype)
#
#         f.writelines(
#             [
#                 f"#include <nanoarrow/nanoarrow.hpp>\n",
#                 f"#include <nanoarrow/nanoarrow.h>\n",
#                 f"#include <nanoarrow/nanoarrow_ipc.hpp>\n",
#                 f"#include <nanoarrow/nanoarrow_ipc.h>\n",
#                 f"\n"
#                 f"na::UniqueSchema make_nanoarrow_schema() {{\n"
#                 f"    na::UniqueSchema schema_root;\n"
#                 f"    ArrowSchemaInit(schema_root.get());\n"
#                 f"    ArrowSchemaSetTypeStruct(schema_root.get(), {cols});\n"
#                 f"\n",
#             ]
#         )
#         for i, (name, datatype) in enumerate(signal_to_datatype.items()):
#             nanoarrow_type_macro = PYARROW_TO_NANOARROW[datatype]
#             f.writelines(
#                 [
#                     f"    ArrowSchemaInitFromType(schema_root->children[{i}], {nanoarrow_type_macro});\n",
#                     f'    ArrowSchemaSetName(schema_root->children[{i}], "{name}");\n',
#                 ]
#             )
#         f.writelines(
#             [
#                 "    return schema_root;\n"
#                 "}\n",
#             ]
#         )
#
#         f.writelines(
#             [
#                 f"na::UniqueArray make_nanoarrow_array(ArrowSchema *schema_root, int batch_rows) {{\n",
#                 f"    ArrowError error;\n",
#                 f"\n",
#                 f"    na::UniqueArray array_root;\n",
#                 f"    ARROW_ERROR_PRINT(ArrowArrayInitFromSchema(array_root.get(), schema_root, &error));\n",
#                 f"    ArrowArrayAllocateChildren(array_root.get(), {cols});\n",
#                 f"    for (int i = 0; i < {cols}; i++) {{\n",
#                 f"        ARROW_ERROR_PRINT(ArrowArrayInitFromSchema(array_root->children[i], schema_root->children[i], &error));\n",
#                 # f"        ArrowArrayStartAppending(array_root->children[i]);",
#                 f"        ArrowArrayReserve(array_root->children[i], batch_rows);\n",
#                 # f"    for (int i = 0; i < ROWS; i++) {{",
#                 # f"      ArrowArrayAppendInt(array_root->children[i], 12340 + i);",
#                 # f"    }}",
#                 # f"    ARROW_ERROR_PRINT(ArrowArrayFinishBuildingDefault(array_root->children[i], &error));",
#             ]
#         )
#         f.writelines(
#             [
#                 "    }\n",
#                 "    return array_root;\n",
#                 "}\n",
#             ]
#         )

# # NOTE: This mapping is NOT exhuastive
# PYARROW_TO_NANOARROW: dict[pa.DataType, str] = {
#     pa.null(): "NANOARROW_TYPE_NA",
#     pa.bool_(): "NANOARROW_TYPE_BOOL",
#     pa.int8(): "NANOARROW_TYPE_INT8",
#     pa.uint8(): "NANOARROW_TYPE_UINT8",
#     pa.int16(): "NANOARROW_TYPE_INT16",
#     pa.uint16(): "NANOARROW_TYPE_UINT16",
#     pa.int32(): "NANOARROW_TYPE_INT32",
#     pa.uint32(): "NANOARROW_TYPE_UINT32",
#     pa.int64(): "NANOARROW_TYPE_INT64",
#     pa.uint64(): "NANOARROW_TYPE_UINT64",
#     pa.float16(): "NANOARROW_TYPE_HALF_FLOAT",
#     pa.float32(): "NANOARROW_TYPE_FLOAT",
#     pa.float64(): "NANOARROW_TYPE_DOUBLE",
#     pa.float64(): "NANOARROW_TYPE_DOUBLE",
#     pa.date32(): "NANOARROW_TYPE_DATE32",
#     pa.date64(): "NANOARROW_TYPE_DATE64",
#     pa.time32("ms"): "NANOARROW_TYPE_TIME32",
#     pa.time64("ns"): "NANOARROW_TYPE_TIME64",
#     pa.timestamp("ms"): "NANOARROW_TYPE_TIMESTAMP",
#     pa.duration("ms"): "NANOARROW_TYPE_DURATION",
# }
