//
//  mac_endian.h
//  hw6-fog_of_war
//
//  Created by Erin Elsbernd on 10/24/16.
//  Copyright © 2016 Erin Elsbernd. All rights reserved.
//

#ifndef mac_endian_h
#define mac_endian_h

// fix for macs to make endian.h work from:
// http://debugjournal.tumblr.com/post/99661010362/missing-byteswaph-and-endianh-on-mac-os-x

#ifdef __APPLE__
#include <machine/endian.h>
#include <libkern/OSByteOrder.h>

#define htobe16(x) OSSwapHostToBigInt16(x)
#define htole16(x) OSSwapHostToLittleInt16(x)
#define be16toh(x) OSSwapBigToHostInt16(x)
#define le16toh(x) OSSwapLittleToHostInt16(x)

#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)

#define htobe64(x) OSSwapHostToBigInt64(x)
#define htole64(x) OSSwapHostToLittleInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define le64toh(x) OSSwapLittleToHostInt64(x)

#define __BIG_ENDIAN    BIG_ENDIAN
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#define __BYTE_ORDER    BYTE_ORDER
#else
#include
#include
#endif

#endif /* mac_endian_h */
