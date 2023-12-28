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
Each header segment starts with an `uint32` header of its own. The lower 24 bits (0xFFFFFF) hold the size of the segment, excluding the initial `uint32` header, in bytes. The upper eight bits (0xFF000000) identify one of the following segment types. Note that the header must be interpreted as `uint32` according to the byte order previously specified. It is wrong to first read one byte to determine the type and then three bytes to determine the size.

The segment type enumeration of the reference implementation is defined such that it can be directly `or`ed with the size of the segment.

### Frame table (0x01)
This segment is mandatory.

The frame table segment holds the absolute offsets (`SEEK_SET`) of all frames in the file. Each offset is represented as `uint64`. The number of frames can be computed by dividing the size of this header by the size of `uint64`.

### Format description (0x02)
This segment is mandatory.

The format description describes the semantics and layout of the particles. A format description comprises one or more elements of the following format:

| Type   | Description                                                                                                                      |
|--------|----------------------------------------------------------------------------------------------------------------------------------|
| uint64 | The semantic of the element, which can be one of the built-in `semantic`s or a custom one.                                       |
| format | The format of the the element, which must be one of the enumerated `format`s.                                                    |
| uint16 | The zero-based index of the stream to which the element belongs to.                                                              |
| uint16 | Offset of the element within the stream, in bytes. If zero, the element will be assumed to start directly after its predecessor. |
| uint16 | The size of the element in bytes. If zero, the `format` will be used to determine the size.                                      |

### User-defined semantic description (0x03)
This segment is optional any only present if any of the format descriptions uses a custom semantic.

All strings in this segment are null-terminated UTF-16 strings. For each user-defined semantic used in the format description, there must be a matching entry in this array.

### File description (0x04)
This segment is optional and holds a single UTF-16 string with a human-readable description of the contents of the file.


## Frame data
The frame data is a raw consecutive list of frames each of which is a list of consecutive particles as described in the format description header segment. The number of particles is determined by the size of the frame in bytes divided by the size of a single particle as described in the format description header segment.

Padding within and between particles is only allowed to the extent as it is specified in the format description header segment. Padding between frames is not allowed unless the particle format specifies padding at the end of each particle. 
