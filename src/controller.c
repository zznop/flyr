/*
 * controller.c
 */

#include "parson/parson.h"
#include "utils.h"
#include "controller.h"

/**
 * JSON file data
 */
static struct json_value_t *_json_data = NULL;


/**
 * Parse dudley JSON file and validate the schema
 */
bool parse_dudley_file(const char *filepath)
{
    int ret = FAILURE;
    const char *name = NULL;
    struct json_value_t *schema = json_parse_string("{\"name\":\"\"}");
    _json_data = json_parse_file(filepath);
    if (!_json_data) {
        duderr("Failed to parse json file");
        goto done;
    }

    if (json_validate(schema, _json_data) != JSONSuccess) {
        duderr("Erroneous JSON schema");
        json_value_free(_json_data);
        goto done;
    }

    name = json_object_get_string(json_object(_json_data), "name");
    dudinfo("%s (%s) loaded successfully", filepath, name);
    ret = SUCCESS;
done:
    json_value_free(schema);
    return ret;
}
