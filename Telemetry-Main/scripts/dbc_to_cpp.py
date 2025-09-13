import cantools
import numpy as np
import math
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
        for arrow_type, min_val, max_val in candidates:
            if decoded_min >= min_val and decoded_max <= max_val:
                return arrow_type
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
    out_file = open(Path(__file__).parent.parent / "fsdaq" / "generated_types.hpp", "w")
    template_file = open(Path(__file__).parent.parent / "fsdaq" / "generated_types.hpp.in", "r")
    template = template_file.read()

    assert rows_per_batch % 8 == 0

    signal_to_fsdaq_datatype: dict[str, str] = {}

    for msg in db.messages:
        for signal in msg.signals:
            signal_to_fsdaq_datatype[signal.name] = get_fsdaq_type_for_dbc_signal(signal)
            # print(signal.name)

    template = template.replace("@COLS@", str(len(signal_to_fsdaq_datatype)))
    template = template.replace("@ROWS_PER_BATCH@", str(rows_per_batch))

    col_names = ", ".join(['"' + col + '"' for col in signal_to_fsdaq_datatype.keys()])
    col_name_sizes = ", ".join([str(len(col_name)) for col_name in signal_to_fsdaq_datatype.keys()])
    col_name_types = ", ".join(['"' + fsdaq_type + '"' for fsdaq_type in signal_to_fsdaq_datatype.values()])
    template = template.replace("@COL_NAMES@", col_names)
    template = template.replace("@COL_NAME_SIZES@", col_name_sizes)
    template = template.replace("@COL_NAME_TYPES@", col_name_types)

    batch_struct_fields = []
    batch_row_struct_fields = []
    update_fields_from_row = []
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
    
    out_file.write(template)
    out_file.close()
    template_file.close()

# def generate_can_processor_hpp(db: cantools.db.Database):
#     out_file = open(Path(__file__).parent.parent / "fsdaq" / "can_processor_generated.hpp", "w")
#     template_file = open(Path(__file__).parent.parent / "fsdaq" / "can_processor_generated.hpp.in", "r")
#     template = template_file.read()
#
#     can_message_ids = []
#     for msg in db.messages:
#         can_message_ids.append(" "*4 + f"{msg.name} = {hex(msg.frame_id)},")
#
#     template = template.replace("@MESSAGE_IDS@", "\n".join(can_message_ids))
#
#     out_file.write(template)
#     out_file.close()
#     template_file.close()

# def generate_can_processor_cpp(db: cantools.db.Database):
#     out_file = open(Path(__file__).parent.parent / "fsdaq" / "can_processor_generated.cpp", "w")
#     template_file = open(Path(__file__).parent.parent / "fsdaq" / "can_processor_generated.cpp.in", "r")
#     template = template_file.read()
#
#     can_message_processing = []
#     for msg in db.messages:
#         msg_processing_lines = [" "*4 + f"case {msg.name}:"]
#         for signal in msg.signals:
#             if signal.is_float:
#                 raise ValueError("can't handle float signals yet!")
#             # msg_processing_lines.append(" "*8 + f"current_row->{signal.name} = 100;")
#             raw_buf_type = f"{signal.is_signed and "" or "u"}int{8 * math.ceil(signal.length / 8)}_t"
#             byte_offset = math.floor(signal.start / 8)
#             end_byte_offset = math.floor(signal.start + signal.length / 8)
#             bit_offset_in_byte = signal.start % 8
#             # underlying_bytes = 
#             last_byte_idx = math.ceil(signal.length / 8)
#             msg_processing_lines.append(" "*8 + f"{raw_buf_type} raw_{signal.name};");
#
#             msg_processing_lines.append("");
#         msg_processing_lines.append(" "*8 + "break;")
#         can_message_processing.append("\n".join(msg_processing_lines))
#
#     template = template.replace("@CAN_MESSAGE_PROCESSING@", "\n\n".join(can_message_processing))
#
#     out_file.write(template)
#     out_file.close()
#     template_file.close()



if __name__ == "__main__":
    # for debugging purposes:
    np.set_printoptions(formatter={"float_kind": "{:.8f}".format})

    db = cantools.db.Database()
    db.add_dbc_file("../CANbus.dbc")

    generate_encoder_hpp(db, rows_per_batch=80)
    # generate_can_processor_hpp(db)
    # generate_can_processor_cpp(db)




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
