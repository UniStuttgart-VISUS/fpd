# Format specification

## Main format
| Type           | Length   | Description                                                                            |
|----------------|----------|----------------------------------------------------------------------------------------|
| Magic ID       | 3 Bytes  | Magic character sequence.                                                              |
| uint8          | 1 Byte   | File major version.                                                                    |
| uint8          | 1 Byte   | File minor version.                                                                    |
| uint16         | 2 Bytes  | Byte order marker, which is 0x01, 0x02 for little endian and 0x02, 0x01 for big endian |
| uint32         | 4 Bytes  | Overall size of the header segments in bytes                                           |
| Header segment | Variable | Sequence of previously announced header segments                                       |
| Frame data     | Variable | Sequence of frames announced in header segments                                        |

## Magic ID
The magic identifier for a flexible particle data file, which must be the sequence "FPD" of upper case ASCII characters.

## Header segment
Each header segment starts with an `uint32` header of its own. The lower 24 bits (0xFFFFFF) hold the size of the segment in bytes. The upper eight bits (0xFF000000) identify one of the following segment types. The segment type enumeration of the reference implementation is defined such that it can be directly `or`ed with the size of the segment. 

### Frame table (0x01)
This segment is mandatory.

The frame table segment holds the absolute offsets (`SEEK_SET`) of all frames in the file. Each offset is represented as `uint64`.

### Format description (0x02)
This segment is mandatory.

The format description describes the semantics of the particles.


## Frame data
The frame data is a raw consecutive list of frames each of which is a list of consecutive particles as described in the format description header segment. The number of particles is determined by the size of the frame in bytes divided by the size of a single particle as described in the format description header segment.

Padding within and between particles is only allowed to the extent as it is specified in the format description header segment. Padding between frames is not allowed.
