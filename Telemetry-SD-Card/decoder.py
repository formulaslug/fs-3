import argparse
import numpy as np
import polars as pl

parser = argparse.ArgumentParser()
parser.add_argument('--input_file','-i',type=str, action='store', help='input file name', required=True)
parser.add_argument('--output_file','-o',type=str, action='store', help='output file name', default='out.arrow')
args = parser.parse_args()

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


with open (args.input_file, "rb" ) as f:
    # f = open("test.fsdaq", "rb")
    df = pl.DataFrame()
    batchlen = 0
    data = f.read()
    # print(ascii(data))
    header = ascii(data[:8])
    m = np.frombuffer(data[8:12], dtype=np.uint32, count=1)[0]
    n = np.frombuffer(data[12:16], dtype=np.uint32, count=1)[0]
    pos = 16
    colTitles = []
    print(f"Header: {header}, m: {m}, n: {n}")
    for i in range(m):
        length = data[pos]
        pos += 1
        title = data[pos:pos+length].decode('ascii')
        colTitles.append(title)
        pos += length
    colTypes = []
    for i in range(m):
        colType = data[pos:pos+2].decode('ascii')
        colTypes.append((types_dict[colType], int((2**int(colType[1]))/8*n)))
        pos += 2
    # print(colTypes)
    # print(len(data))
    while(ascii(data[pos:pos+8]) != header):
        frame_pieces = []
        for i in range(m):
            col_byte_len = colTypes[i][1]
            col_type = colTypes[i][0]
            # if(len(data) < (col_byte_len)):
            #     df.write_csv(args.output_file)
            #     exit(0)
            if col_type != bool:
                col_bit = np.frombuffer(data[pos:pos+col_byte_len], dtype=col_type, count=n)
            else:
                col_bit = np.frombuffer(np.unpackbits(data[pos:pos+col_byte_len]), dtype=bool, count=n)
            frame_piece_series = pl.Series(colTitles[i], col_bit)
            frame_pieces.append(frame_piece_series)
            pos += col_byte_len
        frame_piece = pl.DataFrame(frame_pieces)
        # print(frame_pieces)
        if df.is_empty():
            df = frame_piece
        else:
            df = df.vstack(frame_piece)
        # print(df)
    df.write_csv(args.output_file)