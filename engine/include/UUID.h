#pragma once
//temp
#define CE_PLATFORM_LINUX
#ifdef CE_PLATFORM_WINDOWS
#include <xhash>
#elif defined(CE_PLATFORM_LINUX)
#include <hashtable.h>
#endif
#include <cstdint>

namespace CE {

    class UUID{
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const { return m_UUID;}
    private:
        uint64_t m_UUID;
    };
}

namespace std {

    template<>
    struct hash<CE::UUID>{

        std::size_t operator()(const CE::UUID& uuid) const{

            return hash<uint64_t>()((uint64_t)uuid);
        }
    };
}
