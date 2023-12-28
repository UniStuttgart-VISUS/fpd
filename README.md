# Flexible Particle Data
The flexible particle data (FPD) format replaces the previously used MegaMol particle list data (MMPLD) in [TRRojan](https://github.com/UniStuttgart-VISUS/trrojan) as input of particle data. This repository holds the C++ reference implementation for processing files of this format.

## Design goals
FPD has been designed to address several shortcomings of the MMPLD format in context of [TRRojan](https://github.com/UniStuttgart-VISUS/trrojan). The following sections detail the design goals of this new format.

### Keep everything together
FPD keeps all data in one file as MMPLD does. If you copy the FPD file, you have everything, the meta data describing the particles and the particle data itself.

### Keep the format extensible
FPD keeps all meta data in at the begin of the file and organises these in individual blocks. This allows for later adding new kinds of meta data without modifying the overall file format.

Furthermore, FPD uses semantic versioning of its file format, which means that changes in the minor version must not impact the ability of an implementation supporting the same major version to process the file. This means that minor versions may, for instance, add new meta data blocks that a previous implementation would just ignore. A new minor version, however, must not remove a mandatory meta data block from the format.

While FPD only allows for a single type of particle in a file, it allows users to compose these particles as they wish rather than prescribing certain elements like MMPLD. The definition of the format of the particle works much like [D3D11_INPUT_ELEMENT_DESC](https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_input_element_desc) or [VkVertexInputAttributeDescription](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkVertexInputAttributeDescription.html), i.e. the user provides a list of elements with a type, location and size that make up a particle.

### Minimise read-to-read dependencies
Modern (NVMe) drives are designed to allow for efficiently processing a deep read queue. In order to leverage this feature, FPD minimises the number of read-to-read dependencies by keeping all meta data in a single block at the begin of the file. At most two reads are required to ingest all ofthe meta data.

### Store the data as they are required on the GPU

### Allow for spatially organised data

### Allow for compression

