# Format specification

| Type   | Length  | Multiplicity | Description                   |
|--------|---------|--------------|-------------------------------|
| uint8  | 1 Byte  | 1            | Magic character. Must be 'F'. |
| uint8  | 1 Byte  | 1            | Magic character. Must be 'P'. |
| uint8  | 1 Byte  | 1            | Magic character. Must be 'D'. |
| uint8  | 1 Byte  | 1            | File version.                 |
| uint16 | 2 Byte  | 1            | Byte order marker, which is 1, 2 for little endian and 2, 1 for big endian |
| uint32 | 4 Bytes | 1            | Number of header segments     |
