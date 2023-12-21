# Format specification

## Main format
| Type           | Length   | Multiplicity | Description                                                                |
|----------------|----------|--------------|----------------------------------------------------------------------------|
| Magic ID       | 1 Byte   | 1            | Magic character sequence.                                                  |
| uint8          | 1 Byte   | 1            | File major version.                                                        |
| uint8          | 1 Byte   | 1            | File minor version.                                                        |
| uint16         | 2 Bytes  | 1            | Byte order marker, which is 1, 2 for little endian and 2, 1 for big endian |
| uint32         | 4 Bytes  | 1            | Number of header segments (`nheaders`)                                     |
| Header segment | Variable | `nheaders`   | Sequence of previously announced header segments                           |
| Frame data     | Variable | 1 - n        | Sequence of previously announced frames                                    |

## Magic ID
The magic identifier for a flexible particle data file, which must be the sequence "FPD" of upper case ASCII characters.

## Header segment

## Frame data
