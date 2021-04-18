#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#define BUFF_MAX_LEN 1024

#define ARR_SIZE(arr, type) (sizeof(arr) / sizeof(type))

#define ATTR(type, attr) (((type *)0)->attr)

#define ATTR_SIZE(type, attr) (sizeof(ATTR(type, attr)))

#define ATTR_OFFSET(type, attr) ((size_t)&ATTR(type, attr))

#define SERIALIZE_MEMCPY_ATTR(dest, src, attr) \
    memcpy(dest + ATTR_OFFSET(Row, attr), &src->attr, ATTR_SIZE(Row, attr))

#define DESERIALIZE_MEMCPY_ATTR(dest, src, attr) \
    memcpy(&dest->attr, src + ATTR_OFFSET(Row, attr), ATTR_SIZE(Row, attr))

#ifdef __cplusplus
}
#endif

#endif