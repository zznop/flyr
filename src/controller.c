/*
 * controller.c
 */

#include "parson/parson.h"
#include "utils.h"
#include "controller.h"

/**
 * Root value of the dudley JSON file
 */
static struct json_value_t *_json_root_value = NULL;

/**
 * Raw data buffer consumed from JSON "input" method
 */
static uint8_t *_raw_data = NULL;

/**
 * Size of raw data buffer
 */
static size_t _raw_data_size = 0;

/**
 * Read the inline hex string from the file and write it to _raw_data as a bytearray
 */
static int _consume_inline_data(struct json_value_t *json_input_value)
{
    size_t data_size = 0;
    size_t i;
    const char *pos = NULL;
    const char *hexstr = json_object_get_string(json_object(json_input_value), "data");
    if (!hexstr) {
        duderr("input data was not supplied");
        return FAILURE;
    }

    // Ensure the input is a hex string
	if (hexstr[strspn(hexstr, "0123456789abcdefABCDEF")]) {
		duderr("input data is not a valid hex string");
		return FAILURE;
	}

	// Allocate the raw data buffer
    data_size = strlen(hexstr) / 2;
    _raw_data = (uint8_t *)malloc(data_size);
    if (!_raw_data) {
        duderr("out of memory");
        return FAILURE;
    }

	// Convert the hex string to a byte array and copy
    pos = hexstr;
    for (i = 0; i < data_size; i++) {
        sscanf(pos, "%2hhx", &_raw_data[i]);
        pos += 2;
        _raw_data_size++;
    }

    return SUCCESS;
}

/**
 * Parse input value and consume data based on parameters
 */
static int _consume_input(void)
{
    struct json_value_t *json_input_value = NULL;
    const char *method = NULL;
    int ret = FAILURE;

    if (_json_root_value == NULL) {
        return FAILURE;
    }

    json_input_value = json_object_get_value(json_object(_json_root_value), "input");
    if (!json_input_value) {
        goto done;
    }

    method = json_object_get_string(json_object(json_input_value), "method");
    if (!method) {
        duderr("input method was not specified");
        goto done;
    }

    if (!strcmp(method, "inline-data")) {
        if (_consume_inline_data(json_input_value)) {
            goto done;
        }

		dudinfo("%lu bytes of input data has been consumed", _raw_data_size);
    } else {
        duderr("unsupported input method: %s", method);
        goto done;
    }
    ret = SUCCESS;
done:
    if (json_input_value) {
        json_value_free(json_input_value);
    }
    return ret;
}

/**
 * Parse dudley JSON file and validate the schema
 */
int parse_dudley_file(const char *filepath)
{
    int ret = FAILURE;
    const char *name = NULL;
    struct json_value_t *schema = json_parse_string(
        "{"
            "\"name\":\"\","
            "\"input\": {},"
            "\"output\": {},"
            "\"events\": {}"
        "}"
    );

    _json_root_value = json_parse_file(filepath);
    if (!_json_root_value) {
        duderr("JSON formatted input is invalid");
        goto done;
    }

    if (json_validate(schema, _json_root_value) != JSONSuccess) {
        duderr("Erroneous JSON schema");
        json_value_free(_json_root_value);
        goto done;
    }

    name = json_object_get_string(json_object(_json_root_value), "name");
    dudinfo("%s (%s) loaded successfully", filepath, name);

    if (_consume_input()) {
        duderr("Failed to parse and initialize the input method");
        goto done;
    }

    ret = SUCCESS;
done:
    json_value_free(schema);
    return ret;
}
