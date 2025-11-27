# Flexible Particle Data
The flexible particle data (FPD) format replaces the previously used MegaMol particle list data (MMPLD) in [TRRojan](https://github.com/UniStuttgart-VISUS/trrojan) as input of particle data. This repository holds the C++ reference implementation for processing files of this format. The description of the on-disk format can be found in the [specification](SPECIFICATION.md).

## Usage
The following examples illustrate how to interact withthe reference implementation of FPD.

### Read meta data to consume a file
```c++
typedef fpd::meta_data<char> meta_data;

// Open the input file for binary I/O.
int fh = ::_open(path.c_str(), _O_BINARY | _O_RDONLY);
if (fh == -1) { /* Handle error. */ }

// Read the fixed-size header, which has always the same size.
std::vector<std::uint8_t> data(meta_data::fixed_header_size);
auto rem = static_cast<int>(data.size());
while (rem > 0) {
    auto read = ::_read(fh, data.data() + data.size() - rem, rem);
    if (read <= 0) { /* Handle error. */ }
    rem -= read;
}

// Parse what we have so far in order to determine what must be read
// to get the complete header.
meta_data md(fpd::encodings::utf8);
rem = static_cast<int>(meta_data::check_header_size(md, data.data(),
    data.size()));

// Read the variable-size header segments.
data.resize(rem);
while (rem > 0) {
    auto read = ::_read(fh, data.data() + data.size() - rem, rem);
    if (read <= 0) { /* Handle error. */ }
    rem -= read;
}

// Parse the variable-size header segments, which completes parsing of
// the meta data.
auto footer_offset = meta_data::parse(md, data.data(), data.size());

// If there is a footer, we must read it as well and parse again.
// Files without a footer will return zero for the offset here.
if (footer_offset > 0) {
    std::vector<std::uint8_t> footer(128);
    auto footer_size = 0;
    auto read = 0;

    ::_lseek(fh, static_cast<long>(footer_offset), SEEK_SET);

    do {
        read = ::_read(fh, footer.data() + footer_size,
            data.size() - footer_size);
        footer_size += read;
    } while (read > 0);

    meta_data::parse(md, data.data(), data.size(),
        footer.data(), footer_size);
}

// Seek to the start of the first frame.
::_lseek(fh, static_cast<long>(md.frame_offset(0)), SEEK_SET);
data.resize(md.frame_size(0));

// Read the data and do something useful with them ...

::_close(fh);
```

### Build in-memory meta data to create a new file
```c++
typedef fpd::meta_data_builder<wchar_t> meta_data_builder;

// Create a builder that allows for programmatically creating a meta
// data object.
meta_data_builder builder;

// Describe our file format: We have the position and the radius in
// a single float4 vector and a floating-point intensity value.
// These are stored interleaved as array of structures. In a second
// stream, we have a user-defined integer ID per particle.
builder.add_format_element(fpd::semantic::position_radius, fpd::format::float32xyzw, 0)
    .add_format_element(fpd::semantic::intensity, fpd::format::float32, 0)
    .add_format_element(fpd::user_defined_semantic(0), L"id", fpd::format::uint64, 1);

// Add a description to the file.
builder.with_description(L"オ マイ ゴッド");

// Specify how many frames we have and how many particles are in
// each of them.
builder.with_frames({ 17, 42, 42 });

// Create the meta data object and save it to disk.
auto md = builder.build();

auto file = ::CreateFileA("demo.fpd",
    GENERIC_READ | GENERIC_WRITE,
    0,
    nullptr,
    CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL,
    nullptr);
const auto size = md.save(file);
```

## Design goals
FPD has been designed to address several shortcomings of the MMPLD format in context of TRRojan. The following sections detail the design goals of this new format.

### Keep everything together
FPD keeps all data in one file as MMPLD does. If you copy the FPD file, you have everything, the meta data describing the particles and the particle data itself.

### Keep the format extensible
FPD keeps all meta data in at the begin of the file and organises these in individual blocks. This allows for later adding new kinds of meta data without modifying the overall file format.

Furthermore, FPD uses semantic versioning of its file format, which means that changes in the minor version must not impact the ability of an implementation supporting the same major version to process the file. This means that minor versions may, for instance, add new meta data blocks that a previous implementation would just ignore. A new minor version, however, must not remove a mandatory meta data block from the format.

While FPD only allows for a single type of particle in a file, it allows users to compose these particles as they wish rather than prescribing certain elements like MMPLD. The definition of the format of the particle works much like [D3D11_INPUT_ELEMENT_DESC](https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_input_element_desc) or [VkVertexInputAttributeDescription](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkVertexInputAttributeDescription.html), i.e. the user provides a list of elements with a type, location and size that make up a particle. FPD provides a set of predefined semantics like `fpd::semantic::position` and `fpd::semantic::colour`, but also allows users to add their own semantics. The elements forming a particle can be organised in groups, which allows for specifying the data as array of structures or as strucuture of arrays or a combination of both.

### Minimise read-to-read dependencies
Modern (NVMe) drives are designed to allow for efficiently processing a deep read queue. In order to leverage this feature, FPD minimises the number of read-to-read dependencies by keeping all meta data in a single block at the begin of the file. At most two reads are required to ingest all ofthe meta data.

### Store the data as they are required on the GPU
Just as the particle lists in MMLPD are designed to match OpenGL immediate mode APIs, the flexible particle format of FPD is designed to match how Direct3D 11, Direct3D 12 and Vulkan work. The elements of the particle description can be mapped to [D3D11_INPUT_ELEMENT_DESC](https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_input_element_desc) or [VkVertexInputAttributeDescription](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkVertexInputAttributeDescription.html) to describe the data to the graphics API. The particles match this layout and can simply be copied into the buffers of the graphics API.

### Allow for spatially organised data
The file format allows for optionally attaching a spatial structure in its flexible header segments. This does, however, not change the layout of the data itself, which can always be rendered brute force.

### Allow for compression
TRRojan supports DirectStorage with GDeflate wherefore FPD adds support for compressing frames.
