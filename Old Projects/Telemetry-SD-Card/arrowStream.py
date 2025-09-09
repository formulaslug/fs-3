import argparse
import numpy as np
import polars as pl
import serial

parser = argparse.ArgumentParser()
parser.add_argument('--input_file','-i',type=str, action='store', help='input file name', required=True)
parser.add_argument('--port','-P',type=str, action='store', help='serial_port', required=True)
parser.add_argument('--log','-l',type=str, action='store', help='serial_port', required=True)
parser.add_argument('--output_file','-o',type=str, action='store', help='output file name', default='out.arrow')
args = parser.parse_args()

# Conversion from ASCII defined types in custom binary to numpy types.
# [type, n] where size is 2**n bits
types_dict = {
    "i3": np.int8,
    "i4": np.int16,
    "i5": np.int32,
    "i6": np.int64,
    "u3": np.uint8,
    "u4": np.uint16,
    "u5": np.uint32,
    "u6": np.uint64,
    "f4": np.float16,
    "f5": np.float32,
    "f6": np.float64,
    "b0": bool
    }

# Initialize dataframe
df = pl.DataFrame()

# Open serial port. Intentionall have no timeout so that we can read until we get the expected header/schema.
s = serial.Serial(args.port, baudrate=115200)
s.set_buffer_size(rx_size=1000000)
s.reset_input_buffer()
s.reset_output_buffer()
s.write(b'QS') # Query Schema

# Attempts to read the schema from the serial port.
# Requests by sending 'QS' 
for i in range(1000):
    header = ascii(s.read(8))
    m = np.frombuffer(s.read(4), dtype=np.uint32, count=1)[0]
    n = np.frombuffer(s.read(4), dtype=np.uint32, count=1)[0]

    ## Expects to receive record bathes in some multiple of bytes therefore it will not work if n is not a multiple of 8.
    if ((n%8 != 0) or (n <= 0) or (m <= 0) or (header[:5] != "FSDAQ")):
        s.write(b'QS') # Query Schema again if n is not a multiple of 8 - Indicates failed attempt to receive schema to the NUCLEO
    else:
        s.write(b'RS') # Received Schema - Indicates to NUCLEO to start sending data
        break
    if i == 999:
        s.write(b'SE') # Schema Error - Indicates to the NUCLEO that it failed to get the schema and will exit the program.
        s.close()
        raise Exception("Schema Error: Failed to receive valid schema after 1000 attempts.")

# print(f"Header: {header}, m: {m}, n: {n}")

# Reading in the colum titles. They are specified as 
colTitles = []
for i in range(m):
    length = s.read() # uint8 length of title
    title = s.read(length).decode('ascii') # ascii string of title
    colTitles.append(title) # Add title to list of titles

# Reading in the column types. They are specified as 2 byte ascii strings where 
# the first character is type of type (int, uint, float, bool)
# and the second character is log2() the size of the type in bits (3, 4, 5, 6). Ex. f5 = float32, b0 = bool, u4 = uint16
colTypes = []
for i in range(m):
    colType = s.read(2).decode('ascii') # 2 byte ascii string of type
    if colType not in types_dict:
        s.write(b'SE') # Schema Error - Indicates to the NUCLEO that it failed to get the schema and will exit the program.
        s.close()
        raise Exception(f"Schema Error: Invalid column type {colType} received.")
    colTypes.append((types_dict[colType], int((2**int(colType[1]))/8*n)))


while (True):
    frame_pieces = []
    for i in range(m):
        col_byte_len = colTypes[i][1]
        col_type = colTypes[i][0]
        col_bit = np.frombuffer(s.read(col_byte_len), dtype=col_type, count=n)
        frame_piece_series = pl.Series(colTitles[i], col_bit)
        frame_pieces.append(frame_piece_series)
        pos += col_byte_len
    frame_piece = pl.DataFrame(frame_pieces)
    df.to_arrow()