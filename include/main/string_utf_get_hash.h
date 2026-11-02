#ifndef INCLUDE_MAIN_STRING_UTF_GET_HASH_H
#define INCLUDE_MAIN_STRING_UTF_GET_HASH_H

void reloadConstString(void *heap_boundary);

/* Functions and data used by other translation units. */
SceneString *loadConstString(const char *bytes, int length);
SceneString *loadConstString2(const char *bytes, int length);

#endif /* INCLUDE_MAIN_STRING_UTF_GET_HASH_H */
