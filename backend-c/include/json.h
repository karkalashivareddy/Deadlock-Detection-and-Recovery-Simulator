#ifndef DEADLOCKLAB_JSON_H
#define DEADLOCKLAB_JSON_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define JSON_MAX_TOKENS 16384
#define JSON_MAX_DEPTH 64

typedef enum { JT_OBJECT, JT_ARRAY, JT_STRING, JT_NUMBER, JT_TRUE, JT_FALSE, JT_NULL } JsonType;
typedef struct { JsonType type; size_t start, end, next, parent; } JsonToken;
typedef struct { const char *input; size_t length, count; JsonToken *tokens; } JsonDoc;
typedef struct { size_t offset; char message[112]; } JsonError;

bool json_parse(JsonDoc *doc, const char *input, size_t length, JsonError *error);
void json_free(JsonDoc *doc);
size_t json_object_get(const JsonDoc *doc, size_t object, const char *key);
size_t json_array_size(const JsonDoc *doc, size_t array);
size_t json_array_get(const JsonDoc *doc, size_t array, size_t index);
bool json_string_copy(const JsonDoc *doc, size_t token, char *output, size_t capacity);
bool json_integer(const JsonDoc *doc, size_t token, int *output);
bool json_boolean(const JsonDoc *doc, size_t token, bool *output);
bool json_object_has_only(const JsonDoc *doc, size_t object, const char *const *keys, size_t key_count);

#endif
