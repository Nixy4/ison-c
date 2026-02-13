/**
 * @file value.c
 * @brief Value container implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "isonantic.h"

/* ==================== Value Functions ==================== */

isonantic_value_t* isonantic_value_create_string(const char* str) {
    if (str == NULL) return NULL;
    isonantic_value_t* value = (isonantic_value_t*)malloc(sizeof(isonantic_value_t));
    if (value == NULL) return NULL;
    value->type = ISONANTIC_VALUE_STRING;
    value->data.string_value = strdup(str);
    return value;
}

isonantic_value_t* isonantic_value_create_number(double num) {
    isonantic_value_t* value = (isonantic_value_t*)malloc(sizeof(isonantic_value_t));
    if (value == NULL) return NULL;
    value->type = ISONANTIC_VALUE_NUMBER;
    value->data.number_value = num;
    return value;
}

isonantic_value_t* isonantic_value_create_boolean(bool val) {
    isonantic_value_t* value = (isonantic_value_t*)malloc(sizeof(isonantic_value_t));
    if (value == NULL) return NULL;
    value->type = ISONANTIC_VALUE_BOOLEAN;
    value->data.boolean_value = val;
    return value;
}

isonantic_value_t* isonantic_value_create_null(void) {
    isonantic_value_t* value = (isonantic_value_t*)malloc(sizeof(isonantic_value_t));
    if (value == NULL) return NULL;
    value->type = ISONANTIC_VALUE_NULL;
    value->data.string_value = NULL;
    return value;
}

isonantic_value_t* isonantic_value_create_ref(const char* ref) {
    if (ref == NULL) return NULL;
    isonantic_value_t* value = (isonantic_value_t*)malloc(sizeof(isonantic_value_t));
    if (value == NULL) return NULL;
    value->type = ISONANTIC_VALUE_REFERENCE;
    value->data.ref_value = strdup(ref);
    return value;
}

void isonantic_value_free(isonantic_value_t* value) {
    if (value == NULL) return;
    switch (value->type) {
        case ISONANTIC_VALUE_STRING:
            free(value->data.string_value);
            break;
        case ISONANTIC_VALUE_REFERENCE:
            free(value->data.ref_value);
            break;
        case ISONANTIC_VALUE_OBJECT:
            isonantic_dict_free(value->data.object_value);
            break;
        case ISONANTIC_VALUE_ARRAY:
            isonantic_array_free(value->data.array_value);
            break;
        default:
            break;
    }
    free(value);
}

/* ==================== Dictionary Functions ==================== */

static unsigned int dict_hash(const char* key) {
    unsigned int hash = 0;
    while (*key) {
        hash = hash * 31 + *key++;
    }
    return hash;
}

isonantic_dict_t* isonantic_dict_create(int capacity) {
    if (capacity <= 0) capacity = 16;
    isonantic_dict_t* dict = (isonantic_dict_t*)malloc(sizeof(isonantic_dict_t));
    if (dict == NULL) return NULL;
    dict->buckets = (isonantic_dict_entry_t**)calloc(capacity, sizeof(isonantic_dict_entry_t*));
    if (dict->buckets == NULL) {
        free(dict);
        return NULL;
    }
    dict->capacity = capacity;
    dict->size = 0;
    return dict;
}

void isonantic_dict_set(isonantic_dict_t* dict, const char* key, void* value) {
    if (dict == NULL || key == NULL) return;

    /* Check if key already exists */
    isonantic_dict_entry_t* entry = dict->buckets[dict_hash(key) % dict->capacity];
    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }

    /* Add new entry */
    isonantic_dict_entry_t* new_entry = (isonantic_dict_entry_t*)malloc(sizeof(isonantic_dict_entry_t));
    if (new_entry == NULL) return;
    new_entry->key = strdup(key);
    new_entry->value = value;
    new_entry->next = dict->buckets[dict_hash(key) % dict->capacity];
    dict->buckets[dict_hash(key) % dict->capacity] = new_entry;
    dict->size++;
}

void* isonantic_dict_get(isonantic_dict_t* dict, const char* key) {
    if (dict == NULL || key == NULL) return NULL;
    isonantic_dict_entry_t* entry = dict->buckets[dict_hash(key) % dict->capacity];
    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

bool isonantic_dict_has_key(isonantic_dict_t* dict, const char* key) {
    return isonantic_dict_get(dict, key) != NULL;
}

int isonantic_dict_size(isonantic_dict_t* dict) {
    if (dict == NULL) return 0;
    return dict->size;
}

static void dict_entry_free(isonantic_dict_entry_t* entry) {
    if (entry == NULL) return;
    free(entry->key);
    dict_entry_free(entry->next);
    free(entry);
}

void isonantic_dict_free(isonantic_dict_t* dict) {
    if (dict == NULL) return;
    for (int i = 0; i < dict->capacity; i++) {
        dict_entry_free(dict->buckets[i]);
    }
    free(dict->buckets);
    free(dict);
}

/* ==================== Array Functions ==================== */

isonantic_array_t* isonantic_array_create(int capacity) {
    if (capacity <= 0) capacity = 16;
    isonantic_array_t* arr = (isonantic_array_t*)malloc(sizeof(isonantic_array_t));
    if (arr == NULL) return NULL;
    arr->items = (void**)malloc(capacity * sizeof(void*));
    if (arr->items == NULL) {
        free(arr);
        return NULL;
    }
    arr->size = 0;
    arr->capacity = capacity;
    return arr;
}

void isonantic_array_add(isonantic_array_t* arr, void* item) {
    if (arr == NULL) return;
    if (arr->size >= arr->capacity) {
        int new_capacity = arr->capacity * 2;
        void** new_items = (void**)realloc(arr->items, new_capacity * sizeof(void*));
        if (new_items == NULL) return;
        arr->items = new_items;
        arr->capacity = new_capacity;
    }
    arr->items[arr->size++] = item;
}

void* isonantic_array_get(isonantic_array_t* arr, int index) {
    if (arr == NULL || index < 0 || index >= arr->size) return NULL;
    return arr->items[index];
}

int isonantic_array_size(isonantic_array_t* arr) {
    if (arr == NULL) return 0;
    return arr->size;
}

void isonantic_array_free(isonantic_array_t* arr) {
    if (arr == NULL) return;
    free(arr->items);
    free(arr);
}

/* ==================== Validation Error Functions ==================== */

isonantic_validation_error_t* isonantic_validation_error_create(const char* field, const char* message, isonantic_value_t* value) {
    isonantic_validation_error_t* error = (isonantic_validation_error_t*)malloc(sizeof(isonantic_validation_error_t));
    if (error == NULL) return NULL;
    error->field = field ? strdup(field) : NULL;
    error->message = message ? strdup(message) : NULL;
    error->value = value;
    error->next = NULL;
    return error;
}

void isonantic_validation_errors_add(isonantic_validation_errors_t* errors, isonantic_validation_error_t* error) {
    if (errors == NULL || error == NULL) return;
    if (errors->tail == NULL) {
        errors->head = errors->tail = error;
    } else {
        errors->tail->next = error;
        errors->tail = error;
    }
    errors->count++;
}

isonantic_validation_errors_t* isonantic_validation_errors_create(void) {
    isonantic_validation_errors_t* errors = (isonantic_validation_errors_t*)malloc(sizeof(isonantic_validation_errors_t));
    if (errors == NULL) return NULL;
    errors->head = errors->tail = NULL;
    errors->count = 0;
    return errors;
}

bool isonantic_validation_errors_has_errors(isonantic_validation_errors_t* errors) {
    return errors != NULL && errors->count > 0;
}

int isonantic_validation_errors_count(isonantic_validation_errors_t* errors) {
    return errors ? errors->count : 0;
}

static void validation_error_free(isonantic_validation_error_t* error) {
    if (error == NULL) return;
    free(error->field);
    free(error->message);
    /* Don't free value - caller owns it */
    validation_error_free(error->next);
    free(error);
}

void isonantic_validation_errors_free(isonantic_validation_errors_t* errors) {
    if (errors == NULL) return;
    validation_error_free(errors->head);
    free(errors);
}

char* isonantic_validation_errors_to_string(isonantic_validation_errors_t* errors) {
    if (errors == NULL || errors->head == NULL) {
        char* result = strdup("");
        return result;
    }

    /* Calculate required size */
    size_t total_len = 0;
    isonantic_validation_error_t* curr = errors->head;
    while (curr != NULL) {
        total_len += strlen(curr->field) + 2 + strlen(curr->message) + 2; /* ": " and "; " */
        curr = curr->next;
    }

    char* result = (char*)malloc(total_len + 1);
    if (result == NULL) return NULL;
    result[0] = '\0';

    curr = errors->head;
    while (curr != NULL) {
        if (result[0] != '\0') {
            strcat(result, "; ");
        }
        strcat(result, curr->field);
        strcat(result, ": ");
        strcat(result, curr->message);
        curr = curr->next;
    }

    return result;
}
