#pragma once

// NOLINT(namespace-envoy)

#ifdef __APPLE__

#include <libkern/OSByteOrder.h>

#define htole16(x) OSSwapHostToLittleInt16((x))
#define htole32(x) OSSwapHostToLittleInt32((x))
#define htole64(x) OSSwapHostToLittleInt64((x))
#define le16toh(x) OSSwapLittleToHostInt16((x))
#define le32toh(x) OSSwapLittleToHostInt32((x))
#define le64toh(x) OSSwapLittleToHostInt64((x))

#define htobe16(x) OSSwapHostToBigInt16((x))
#define htobe32(x) OSSwapHostToBigInt32((x))
#define htobe64(x) OSSwapHostToBigInt64((x))
#define be16toh(x) OSSwapBigToHostInt16((x))
#define be32toh(x) OSSwapBigToHostInt32((x))
#define be64toh(x) OSSwapBigToHostInt64((x))
#elif defined(_WIN32)
#include <stdlib.h>
#define htole16(x) (x)
#define htole32(x) (x)
#define htole64(x) (x)
#define le16toh(x) (x)
#define le32toh(x) (x)
#define le64toh(x) (x)

#define htobe16(x) _byteswap_ushort((x))
#define htobe32(x) _byteswap_ulong((x))
#define htobe64(x) _byteswap_uint64((x))
#define be16toh(x) _byteswap_ushort((x))
#define be32toh(x) _byteswap_ulong((x))
#define be64toh(x) _byteswap_uint64((x))

#else

#include <endian.h>

#endif
