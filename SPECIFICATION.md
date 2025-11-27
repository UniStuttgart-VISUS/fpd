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
| Footer segment | Variable | If a footer segment is present in the header, until the end of the file                |


## Magic ID
The magic identifier for a flexible particle data file, which must be the sequence "FPD" of upper case ASCII characters.


## File version
The file version determines which implementations are able to interpret the data in the file.

All changes to the specification must make sure that all readers sharing the same major version can at least interpret the contents of a file without bailing out. This means that the major version must be incremented whenever something is removed or the interpretation of existing bytes is changed. On the flipside, all implementations must make sure that they are able to ignore optional data they do not understand.


## Byte order marker
The two bytes 0x01 and 0x02 at this position determine how the endian of all subsequent data must be interpreted. All numbers in the file greater than one byte must be byte-swapped if this endian is not the native endian of the system.


## Flexible header size
This `uint32` determines how many bytes all of the following header segments comprise in total. It can be used to read up to the end of all headers.


## Header segment
Each header segment starts with an `uint32` header of its own. The lower 24 bits (0xFFFFFF) hold the size of the segment, excluding the initial `uint32` header, in bytes. The upper eight bits (0xFF000000) identify one of the following segment types. Note that the header must be interpreted as `uint32` according to the byte order previously specified. It is wrong to first read one byte to determine the type and then three bytes to determine the size.

The segment type enumeration of the reference implementation is defined such that it can be directly `or`ed with the size of the segment.

### Frame table (0x01)
This segment is mandatory.

The frame table segment holds the number of particles for each frame in the file. Each number is represented as `uint64`.

| Type           | Length   | Description                                      |
|----------------|----------|--------------------------------------------------|
| uint64 | 8 Bytes | The number of particles in the first frame.               |
| ...    | 8 Bytes | The number of particles in the subsequent frames.         |

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
This segment is optional and only present if any of the format descriptions uses a custom semantic.

All strings in this segment are null-terminated UTF-8 strings. For each user-defined semantic used in the format description, there must be a matching entry in this array.

### File description (0x04)
This segment is optional.

This segments holds a single UTF-8 string with a human-readable description of the contents of the file. The string may not be null-terminated, but end just at the border of the segment.

### Bounding box (0x05)
This segment is optional.

This segment specifies the axis-aligned bounding box of the data over all frames and optionally a bounding box for each frame. The presence of per-frame bounding boxes can be determined by checking the size of the segment.

The segment starts with the `format` (4 bytes) of the bounding box. This must be a scalar format (one element), while typically is `format::float32`, followed by the bounding boxes.

| Type     | Length   | Description                                                                                                                          |
|----------|----------|--------------------------------------------------------------------------------------------------------------------------------------|
| format   | 4 Bytes  | The format used to specifiy the locations of the borders of the bounding box, which must be a scalar from the enumerated `format`s.  |
| [format] | Variable | The minimum location on the x-axis. This must have the format specified before.                                                      |
| [format] | Variable | The minimum location on the y-axis. This must have the format specified before.                                                      |
| [format] | Variable | The minimum location on the z-axis. This must have the format specified before.                                                      |
| [format] | Variable | The maximum location on the x-axis. This must have the format specified before.                                                      |
| [format] | Variable | The maximum location on the y-axis. This must have the format specified before.                                                      |
| [format] | Variable | The maximum location on the z-axis. This must have the format specified before.                                                      |
| ...      | ...      | Additional per-frame bounding boxes in the same format as the global bounding box before.                                            |

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

### Spatial hierarchy (0x07)
This segment is optional.

This segment specifies whether there is a spatial hierarchy in the data and potentially additional information about this hierarchy. It always starts with the type of the `spatial_hierarchy` followed by optional data depending on the type of the hierarchy:

| Type              | Length   | Description                                                                                |
|-------------------|----------|--------------------------------------------------------------------------------------------|
| spatial_hierarchy | 4 Bytes  | The type of the spatial hierarchy, which must be one of the built-in `spatial_hierarchy`s. |
| ...               | Variable | Additional data depend on the type of the spatial hierarchy specified before.              |

### None (0x00000000)
No additional data are present.

### OSPRay-style P-kd tree (0x00000001)
For this type of tree, the file must store how the split axes are chosen:
| Type  | Length | Description                                                                                                                                                                                  |
|-------|--------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| uint8 | 1 Byte | The zero-based index of the coordinate where the split axis is embedded in the last three bits. If larger than the number of components, the splits are performed round robin starting at x. |

### Compression (0x08)
This segment is optional.

This segment specifies whether and how the particle data are compressed. The segment looks like:

| Type        | Length   | Description                                                                                                                                                                                        |
|-------------|----------|-----------------------------------------------------------------------------------------------------------------------|
| compression | 4 Bytes  | The type of the compression algorithm used, which must be one of the built-in `compression`s.                         |
| uint8       | Variable | Additional data that descibe how to decompress the data. This can be empty if the compression is `compression::none`. |

#### None (0x00000000)
No additional data are present.

#### GDeflate (0x00000001)
The variable data is a list of four-bytes numbers that specify the size of the compressed blocks. There must be one for each frame, but frames might also be subdivided into (the same) number of sub-blocks:
| Type   | Length   | Description                                                                                                                                                                                        |
|--------|----------|----------------------------------------------------------------------------------------------------------------------------------------------|
| uint32 | 4 Bytes  | The length of each compressed block. There must be one for each frame. The total number of blocks must be divisible by the number of frames. |
| ...    | 4 Bytes  | ...                                                                                                                                          |

If the data comprises of more than one group of elements, there must be either one block per frame (the frame is compressed as a whole) or the number of blocks must be divisible by the number of element groups and the groups are compressed individually. The rationale behind this is that subdividing a frame usually means that it should be streamed progressively. In this case, users must be able to identify where the individual groups are located to decompress all data for matching particles.

### Footer (0x09)
This segment is optional.

This segment specifies the offset of a footer from the begin of the file. The footer can store the same header blocks as the flexible header at the begin of the file and can be used when incrementally writing a file. This might be useful if the overall size of the header is not known in advance, for instance if the number of frames to be written is not yet known.

Using a footer segment has a performance penalty when reading the file as parsing the header will fail if the footer is detected. The caller then needs to read the footer as well and parse again with both, header and footer.

The segment holds the following data:
| Type   | Length   | Description                                                                                 |
|--------|----------|---------------------------------------------------------------------------------------------|
| offset | 8 Bytes  | An unsigned integer value representing the offset of the footer from the begin of the file. |


## Frame data
The frame data is a raw consecutive list of frames each of which is a list of consecutive particles as described in the format description header segment. The number of particles is determined by the size of the frame in bytes divided by the size of a single particle as described in the format description header segment.

Padding within and between particles is only allowed to the extent as it is specified in the format description header segment. Padding between frames is not allowed unless the particle format specifies padding at the end of each particle. 
