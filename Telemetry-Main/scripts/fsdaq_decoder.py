import argparse
import numpy as np
import polars as pl

parser = argparse.ArgumentParser()
parser.add_argument('--input_file','-i',type=str, action='store', help='input file name', required=True)
parser.add_argument('--output_file','-o',type=str, action='store', help='output file name', default='out.arrow')
parser.add_argument('--verbose','-v', action='store_true', help='verbose output')
args = parser.parse_args()

v = args.verbose

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

with open(args.input_file, "rb" ) as f:
    batchlen = 0
    data = f.read()
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
    data_left = len(data[pos:])
    len_of_frame = sum([x[1] for x in colTypes])
    chunks = np.floor(data_left / len_of_frame)
    misc_bytes = (data_left % len_of_frame) #Used to be -8 for footer but ignoring footer
    print(f"chunks left: {chunks}")
    print(f"misc bytes left: {misc_bytes}")
    frames = []
    for i in range(int(chunks)):
        frame_pieces = []
        for i in range(m):
            col_byte_len = colTypes[i][1]
            col_type = colTypes[i][0]
            # if(len(data) < (col_byte_len)):
            #     df.write_csv(args.output_file)
            #     exit(0)
            if col_type != bool:
                if(v):
                    print(f"n = {n}; col_byte_len = {col_byte_len}; col_type = {col_type}")
                col_bit = np.frombuffer(data[pos:pos+col_byte_len], dtype=col_type, count=n)
            else:
                # print(f"n = {n}; int(n/8) = {int(n/8)}")
                if(v):
                    print(f"type = {col_type}")
                col_bit = np.frombuffer(data[pos:pos+col_byte_len], dtype=np.uint8, count=int(n/8))
                col_bit1 = np.unpackbits(col_bit)
                col_bit = [True if x == 1 else False for x in col_bit1]
            if(v):
                print(f"col_bit: {col_bit}")
            frame_piece_series = pl.Series(colTitles[i], col_bit)
            frame_pieces.append(frame_piece_series)
            pos += col_byte_len
        frames.append(pl.DataFrame(frame_pieces))
    df = pl.concat(frames, how="vertical")
    # print(df)
    if v: 
        print(f"Stuff Left: {ascii(data[pos:])}")
    df.write_parquet(args.output_file)
