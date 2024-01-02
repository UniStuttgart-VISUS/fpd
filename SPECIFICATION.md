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


## File version
The file version determines which implementations are able to interpret the data in the file.

All changes to the specification must make sure that all readers sharing the same major version can at least interpret the contents of a file without bailing out. This means that the major version must be incremented whenever something is removed or the interpretation of existing bytes is changed. On the flipside, all implementations must make sure that they are able to ignore optional data they do not understand.


## Byte order marker
The two bytes 0x01 and 0x02 at this position determine how the endian of all subsequent data must be interpreted.


## Flexible header size
This `uint32` determines how many bytes all of the following header segments comprise in total. It can be used to read up to the end of all headers.


## Header segment
Each header segment starts with an `uint32` header of its own. The lower 24 bits (0xFFFFFF) hold the size of the segment, excluding the initial `uint32` header, in bytes. The upper eight bits (0xFF000000) identify one of the following segment types. Note that the header must be interpreted as `uint32` according to the byte order previously specified. It is wrong to first read one byte to determine the type and then three bytes to determine the size.

The segment type enumeration of the reference implementation is defined such that it can be directly `or`ed with the size of the segment.

### Frame table (0x01)
This segment is mandatory.

The frame table segment holds the absolute offsets (`SEEK_SET`) of all frames in the file. Each offset is represented as `uint64`. The number of frames can be computed by dividing the size of this header by the size of `uint64`.

### Format description (0x02)
This segment is mandatory.

The format description describes the semantics and layout of the particles. A format description comprises one or more elements of the following format:

| Type     | Length  | Description                                                                                                                      |
|----------|---------|----------------------------------------------------------------------------------------------------------------------------------|
| semantic | 8 Bytes | The semantic of the element, which can be one of the built-in `semantic`s or a custom one.                                       |
| format   | 4 Bytes | The format of the the element, which must be one of the enumerated `format`s.                                                    |
| uint16   | 2 Bytes | The zero-based index of the stream to which the element belongs to.                                                              |
| uint16   | 2 Bytes | Offset of the element within the stream, in bytes. If zero, the element will be assumed to start directly after its predecessor. |
| uint16   | 2 Bytes | The size of the element in bytes. If zero, the `format` will be used to determine the size.                                      |

### User-defined semantic description (0x03)
This segment is optional any only present if any of the format descriptions uses a custom semantic.

All strings in this segment are null-terminated UTF-16 strings. For each user-defined semantic used in the format description, there must be a matching entry in this array.

### File description (0x04)
This segment is optional.

This segments holds a single UTF-16 string with a human-readable description of the contents of the file. The string may not be null-terminated, but end just at the border of the segment.

### Bounding box (0x05)
This segment is optional.

This segment specifies the axis-aligned bounding box of the data over all frames and optionally a bounding box for each frame. The presence of per-frame bounding boxes can be determined by checking the size of the segment.

The segment starts with the `format` (4 bytes) of the bounding box. This must be a scalar format (one element), typically, it is `format::float32`.

At least one bounding box covering all frames of the file must be present in the previously specified format. The bounding box comprises six elements of the previously specified format specifying the minimum x, minimum y and minimum z position followed by the maximum x, maximum y and maximum z position. The minimum and maximum positions shall account for the radius of the particles in the data set.

If per-frame bounding boxes are included, one for each frame must follow in the same format as for the global one. It is erroneous to specify more than one bounding box, but less than (# of frames + 1) ones.

### Global particle properties (0x06)
This segment is optional.

This segment specifies properties that apply similarly to all particles. These properties must not be included in the per-particle format description.

The global particles are specified as one or more structures of the following content:

| Type     | Length   | Description                                                                                                                        |
|----------|----------|------------------------------------------------------------------------------------------------------------------------------------|
| semantic | 8 Bytes  | The semantic of the property, which can be one of the built-in `semantic`s or a custom one.                                        |
| format   | 4 Bytes  | The format of the the property, which must be one of the enumerated `format`s.                                                     |
| uint8    | Variable | The value of the property, which must be an element of the previous `format`. The size is dependent on the format of the property. |

## Frame data
The frame data is a raw consecutive list of frames each of which is a list of consecutive particles as described in the format description header segment. The number of particles is determined by the size of the frame in bytes divided by the size of a single particle as described in the format description header segment.

Padding within and between particles is only allowed to the extent as it is specified in the format description header segment. Padding between frames is not allowed unless the particle format specifies padding at the end of each particle. 
