/**
 * @file isonantic.h
 * @brief Zod-like validation and type-safe schemas for ISON format in C.
 */

#ifndef ISONANTIC_H
#define ISONANTIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version */
#define ISONANTIC_VERSION "1.0.0"

/* Forward declarations */
typedef struct isonantic_schema_t isonantic_schema_t;
typedef struct isonantic_validation_error_t isonantic_validation_error_t;
typedef struct isonantic_validation_errors_t isonantic_validation_errors_t;
typedef struct isonantic_safe_parse_result_t isonantic_safe_parse_result_t;
typedef struct isonantic_dict_t isonantic_dict_t;
typedef struct isonantic_array_t isonantic_array_t;
typedef struct isonantic_dict_entry_t isonantic_dict_entry_t;

/* Value types for validation */
typedef enum {
    ISONANTIC_VALUE_NULL,
    ISONANTIC_VALUE_STRING,
    ISONANTIC_VALUE_NUMBER,
    ISONANTIC_VALUE_BOOLEAN,
    ISONANTIC_VALUE_OBJECT,
    ISONANTIC_VALUE_ARRAY,
    ISONANTIC_VALUE_REFERENCE
} isonantic_value_type_t;

/* Generic value container */
typedef struct {
    isonantic_value_type_t type;
    union {
        char* string_value;
        double number_value;
        bool boolean_value;
        isonantic_dict_t* object_value;
        isonantic_array_t* array_value;
        char* ref_value;
    } data;
} isonantic_value_t;

/* Validation error */
struct isonantic_validation_error_t {
    char* field;
    char* message;
    isonantic_value_t* value;
    isonantic_validation_error_t* next;
};

/* Collection of validation errors */
struct isonantic_validation_errors_t {
    isonantic_validation_error_t* head;
    isonantic_validation_error_t* tail;
    int count;
};

/* Safe parse result */
struct isonantic_safe_parse_result_t {
    bool success;
    isonantic_dict_t* data;
    isonantic_validation_errors_t* error;
};

/* Refinement function type */
typedef isonantic_validation_errors_t* (*isonantic_refinement_fn_t)(isonantic_value_t* value, void* user_data);

/* Refinement structure */
typedef struct {
    isonantic_refinement_fn_t fn;
    void* user_data;
    char* error_message;
} isonantic_refinement_t;

/* Dictionary/Map implementation */
struct isonantic_dict_entry_t {
    char* key;
    void* value;
    struct isonantic_dict_entry_t* next;
};

struct isonantic_dict_t {
    struct isonantic_dict_entry_t** buckets;
    int capacity;
    int size;
};

/* Array implementation */
struct isonantic_array_t {
    void** items;
    int size;
    int capacity;
};

/* Base schema structure */
struct isonantic_schema_t {
    /* Validation */
    isonantic_validation_errors_t* (*validate)(isonantic_schema_t* schema, isonantic_value_t* value);

    /* Properties */
    bool optional;
    bool has_default;
    isonantic_value_t* default_value;
    char* description;

    /* Refinements */
    isonantic_array_t* refinements;

    /* Free function */
    void (*free_schema)(isonantic_schema_t* schema);
};

/* String schema */
typedef struct {
    isonantic_schema_t base;
    int* min_len;
    int* max_len;
    int* exact_len;
    void* pattern;
    bool is_email;
    bool is_url;
} isonantic_string_schema_t;

/* Number schema */
typedef struct {
    isonantic_schema_t base;
    double* min_val;
    double* max_val;
    bool is_int;
    bool is_positive;
    bool is_negative;
} isonantic_number_schema_t;

/* Boolean schema */
typedef struct {
    isonantic_schema_t base;
} isonantic_boolean_schema_t;

/* Null schema */
typedef struct {
    isonantic_schema_t base;
} isonantic_null_schema_t;

/* Reference schema */
typedef struct {
    isonantic_schema_t base;
    char* ns;
    char* relationship;
} isonantic_ref_schema_t;

/* Object schema */
typedef struct {
    isonantic_schema_t base;
    isonantic_dict_t* fields;
} isonantic_object_schema_t;

/* Array schema */
typedef struct {
    isonantic_schema_t base;
    isonantic_schema_t* item_schema;
    int* min_len;
    int* max_len;
} isonantic_array_schema_t;

/* Table schema */
typedef struct {
    isonantic_schema_t base;
    char* name;
    isonantic_dict_t* fields;
    isonantic_schema_t* row_schema;
} isonantic_table_schema_t;

/* Document schema */
typedef struct {
    isonantic_dict_t* blocks;
} isonantic_document_schema_t;

/* I namespace - provides access to all schema builders */
typedef struct {
    isonantic_schema_t* (*String)(void);
    isonantic_schema_t* (*Number)(void);
    isonantic_schema_t* (*Int)(void);
    isonantic_schema_t* (*Float)(void);
    isonantic_schema_t* (*Boolean)(void);
    isonantic_schema_t* (*Bool)(void);
    isonantic_schema_t* (*Null)(void);
    isonantic_schema_t* (*Ref)(void);
    isonantic_schema_t* (*Reference)(void);
    isonantic_schema_t* (*Object)(isonantic_dict_t* fields);
    isonantic_schema_t* (*Array)(isonantic_schema_t* item_schema);
    isonantic_schema_t* (*Table)(const char* name, isonantic_dict_t* fields);
} isonantic_i_namespace_t;

extern isonantic_i_namespace_t I;

/* ==================== Utility Functions ==================== */

isonantic_value_t* isonantic_value_create_string(const char* str);
isonantic_value_t* isonantic_value_create_number(double num);
isonantic_value_t* isonantic_value_create_boolean(bool val);
isonantic_value_t* isonantic_value_create_null(void);
isonantic_value_t* isonantic_value_create_ref(const char* ref);
void isonantic_value_free(isonantic_value_t* value);

/* ==================== Dictionary Functions ==================== */

isonantic_dict_t* isonantic_dict_create(int capacity);
void isonantic_dict_set(isonantic_dict_t* dict, const char* key, void* value);
void* isonantic_dict_get(isonantic_dict_t* dict, const char* key);
bool isonantic_dict_has_key(isonantic_dict_t* dict, const char* key);
int isonantic_dict_size(isonantic_dict_t* dict);
void isonantic_dict_free(isonantic_dict_t* dict);

/* ==================== Array Functions ==================== */

isonantic_array_t* isonantic_array_create(int capacity);
void isonantic_array_add(isonantic_array_t* arr, void* item);
void* isonantic_array_get(isonantic_array_t* arr, int index);
int isonantic_array_size(isonantic_array_t* arr);
void isonantic_array_free(isonantic_array_t* arr);

/* ==================== Validation Error Functions ==================== */

isonantic_validation_error_t* isonantic_validation_error_create(const char* field, const char* message, isonantic_value_t* value);
void isonantic_validation_errors_add(isonantic_validation_errors_t* errors, isonantic_validation_error_t* error);
isonantic_validation_errors_t* isonantic_validation_errors_create(void);
bool isonantic_validation_errors_has_errors(isonantic_validation_errors_t* errors);
int isonantic_validation_errors_count(isonantic_validation_errors_t* errors);
char* isonantic_validation_errors_to_string(isonantic_validation_errors_t* errors);
void isonantic_validation_errors_free(isonantic_validation_errors_t* errors);

/* ==================== Base Schema Functions ==================== */

void isonantic_schema_set_optional(isonantic_schema_t* schema);
void isonantic_schema_set_default(isonantic_schema_t* schema, isonantic_value_t* value);
void isonantic_schema_set_description(isonantic_schema_t* schema, const char* desc);
void isonantic_schema_add_refinement(isonantic_schema_t* schema, isonantic_refinement_fn_t fn, void* user_data, const char* message);
isonantic_validation_errors_t* isonantic_schema_run_refinements(isonantic_schema_t* schema, isonantic_value_t* value);

/* ==================== String Schema ==================== */

isonantic_schema_t* isonantic_string_create(void);
isonantic_schema_t* isonantic_string_min(isonantic_schema_t* schema, int n);
isonantic_schema_t* isonantic_string_max(isonantic_schema_t* schema, int n);
isonantic_schema_t* isonantic_string_length(isonantic_schema_t* schema, int n);
isonantic_schema_t* isonantic_string_email(isonantic_schema_t* schema);
isonantic_schema_t* isonantic_string_url(isonantic_schema_t* schema);
isonantic_schema_t* isonantic_string_regex(isonantic_schema_t* schema, const char* pattern);
isonantic_schema_t* isonantic_string_optional(isonantic_schema_t* schema);
isonantic_schema_t* isonantic_string_default(isonantic_schema_t* schema, const char* val);
isonantic_schema_t* isonantic_string_describe(isonantic_schema_t* schema, const char* desc);
isonantic_schema_t* isonantic_string_refine(isonantic_schema_t* schema, bool (*fn)(const char*), const char* msg);

/* ==================== Number Schema ==================== */

isonantic_schema_t* isonantic_number_create(void);
isonantic_schema_t* isonantic_int_create(void);
isonantic_schema_t* isonantic_number_min(isonantic_schema_t* schema, double n);
isonantic_schema_t* isonantic_number_max(isonantic_schema_t* schema, double n);
isonantic_schema_t* isonantic_number_positive(isonantic_schema_t* schema);
isonantic_schema_t* isonantic_number_negative(isonantic_schema_t* schema);
isonantic_schema_t* isonantic_number_optional(isonantic_schema_t* schema);
isonantic_schema_t* isonantic_number_default(isonantic_schema_t* schema, double val);
isonantic_schema_t* isonantic_number_describe(isonantic_schema_t* schema, const char* desc);
isonantic_schema_t* isonantic_number_refine(isonantic_schema_t* schema, bool (*fn)(double), const char* msg);

/* ==================== Boolean Schema ==================== */

isonantic_schema_t* isonantic_boolean_create(void);
isonantic_schema_t* isonantic_boolean_optional(isonantic_schema_t* schema);
isonantic_schema_t* isonantic_boolean_default(isonantic_schema_t* schema, bool val);
isonantic_schema_t* isonantic_boolean_describe(isonantic_schema_t* schema, const char* desc);

/* ==================== Null Schema ==================== */

isonantic_schema_t* isonantic_null_create(void);

/* ==================== Reference Schema ==================== */

isonantic_schema_t* isonantic_ref_create(void);
isonantic_schema_t* isonantic_ref_namespace(isonantic_schema_t* schema, const char* ns);
isonantic_schema_t* isonantic_ref_relationship(isonantic_schema_t* schema, const char* rel);
isonantic_schema_t* isonantic_ref_optional(isonantic_schema_t* schema);
isonantic_schema_t* isonantic_ref_describe(isonantic_schema_t* schema, const char* desc);

/* ==================== Object Schema ==================== */

isonantic_schema_t* isonantic_object_create(isonantic_dict_t* fields);
isonantic_schema_t* isonantic_object_optional(isonantic_schema_t* schema);
isonantic_schema_t* isonantic_object_describe(isonantic_schema_t* schema, const char* desc);
isonantic_schema_t* isonantic_object_extend(isonantic_schema_t* schema, isonantic_dict_t* fields);
isonantic_schema_t* isonantic_object_pick(isonantic_schema_t* schema, int num_keys, const char** keys);
isonantic_schema_t* isonantic_object_omit(isonantic_schema_t* schema, int num_keys, const char** keys);

/* ==================== Array Schema ==================== */

isonantic_schema_t* isonantic_array_create_schema(isonantic_schema_t* item_schema);
isonantic_schema_t* isonantic_array_min(isonantic_schema_t* schema, int n);
isonantic_schema_t* isonantic_array_max(isonantic_schema_t* schema, int n);
isonantic_schema_t* isonantic_array_optional(isonantic_schema_t* schema);
isonantic_schema_t* isonantic_array_describe(isonantic_schema_t* schema, const char* desc);

/* ==================== Table Schema ==================== */

isonantic_schema_t* isonantic_table_create(const char* name, isonantic_dict_t* fields);
isonantic_schema_t* isonantic_table_optional(isonantic_schema_t* schema);
isonantic_schema_t* isonantic_table_describe(isonantic_schema_t* schema, const char* desc);
const char* isonantic_table_get_name(isonantic_schema_t* schema);

/* ==================== Document Schema ==================== */

isonantic_schema_t* isonantic_document_create(isonantic_dict_t* blocks);
isonantic_dict_t* isonantic_document_parse(isonantic_schema_t* schema, isonantic_dict_t* value, isonantic_validation_errors_t** error);
isonantic_safe_parse_result_t isonantic_document_safe_parse(isonantic_schema_t* schema, isonantic_dict_t* value);

/* ==================== Free Functions ==================== */

void isonantic_schema_free(isonantic_schema_t* schema);

#ifdef __cplusplus
}
#endif

#endif /* ISONANTIC_H */
