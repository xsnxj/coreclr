// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

enum MEMORY_REGION_FLAGS : uint32_t
{
    // PF_X        = 0x01,      // Execute
    // PF_W        = 0x02,      // Write
    // PF_R        = 0x04,      // Read
    MEMORY_REGION_FLAG_PERMISSIONS_MASK = 0x0f,
    MEMORY_REGION_FLAG_SHARED = 0x10,
    MEMORY_REGION_FLAG_PRIVATE = 0x20,
    MEMORY_REGION_FLAG_MEMORY_BACKED = 0x40
};

struct MemoryRegion 
{
private:
    uint32_t m_flags;
    uint64_t m_startAddress;
    uint64_t m_endAddress;
    uint64_t m_offset;

    // The name used for NT_FILE output
    const char* m_fileName;

public:
    MemoryRegion(uint32_t flags, uint64_t start, uint64_t end) : 
        m_flags(flags),
        m_startAddress(start),
        m_endAddress(end),
        m_offset(0),
        m_fileName(nullptr)
    {
        assert((start & ~PAGE_MASK) == 0);
        assert((end & ~PAGE_MASK) == 0);
    }

    MemoryRegion(uint32_t flags, uint64_t start, uint64_t end, uint64_t offset, const char* filename) : 
        m_flags(flags),
        m_startAddress(start),
        m_endAddress(end),
        m_offset(offset),
        m_fileName(filename)
    {
        assert((start & ~PAGE_MASK) == 0);
        assert((end & ~PAGE_MASK) == 0);
    }

    // copy with new file name constructor
    MemoryRegion(const MemoryRegion& region, const char* fileName) : 
        m_flags(region.m_flags),
        m_startAddress(region.m_startAddress),
        m_endAddress(region.m_endAddress),
        m_offset(region.m_offset),
        m_fileName(fileName)
    {
    }

    // copy with new flags constructor. The file name is not copied.
    MemoryRegion(const MemoryRegion& region, uint32_t flags) : 
        m_flags(flags),
        m_startAddress(region.m_startAddress),
        m_endAddress(region.m_endAddress),
        m_offset(region.m_offset),
        m_fileName(nullptr)
    {
    }

    // copy constructor
    MemoryRegion(const MemoryRegion& region) : 
        m_flags(region.m_flags),
        m_startAddress(region.m_startAddress),
        m_endAddress(region.m_endAddress),
        m_offset(region.m_offset),
        m_fileName(region.m_fileName)
    {
    }

    const uint32_t Permissions() const { return m_flags & MEMORY_REGION_FLAG_PERMISSIONS_MASK; }
    const uint32_t Flags() const { return m_flags; }
    const bool IsBackedByMemory() const { return (m_flags & MEMORY_REGION_FLAG_MEMORY_BACKED) != 0; }
    const uint64_t StartAddress() const { return m_startAddress; }
    const uint64_t EndAddress() const { return m_endAddress; }
    const uint64_t Size() const { return m_endAddress - m_startAddress; }
    const uint64_t Offset() const { return m_offset; }
    const char* FileName() const { return m_fileName; }

    bool operator<(const MemoryRegion& rhs) const
    {
        return (m_startAddress < rhs.m_startAddress) && (m_endAddress <= rhs.m_startAddress);
    }

    // Returns true if "rhs" is wholly contained in this one
    bool Contains(const MemoryRegion& rhs) const
    {
        return (m_startAddress <= rhs.m_startAddress) && (m_endAddress >= rhs.m_endAddress);
    }

    // Free the file name memory
    void Cleanup()
    {
        if (m_fileName != nullptr)
        {
            free((void*)m_fileName);
            m_fileName = nullptr;
        }
    }

    void Trace() const
    {
        TRACE("%s%016lx - %016lx (%06ld) %016lx %02x %s\n", IsBackedByMemory() ? "*" : " ", m_startAddress, m_endAddress, 
            (Size() >> PAGE_SHIFT), m_offset, m_flags, m_fileName != nullptr ? m_fileName : "");
    }
};
