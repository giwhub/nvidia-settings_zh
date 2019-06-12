/*
 * nvidia-settings: A tool for configuring the NVIDIA X driver on Unix
 * and Linux systems.
 *
 * Copyright (C) 2004 NVIDIA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses>.
 */

/*
 * parse.h - prototypes and definitions for the parser.
 */

#ifndef __PARSE_H__
#define __PARSE_H__

#include "NvCtrlAttributes.h"


#define NV_PARSER_ASSIGNMENT 0
#define NV_PARSER_QUERY 1

#define NV_PARSER_MAX_NAME_LEN 256

#define DISPLAY_NAME_SEPARATOR '/'


/*
 * error codes returned by nv_parse_attribute_string().
 */

#define NV_PARSER_STATUS_SUCCESS                             0
#define NV_PARSER_STATUS_BAD_ARGUMENT                        1
#define NV_PARSER_STATUS_EMPTY_STRING                        2
#define NV_PARSER_STATUS_ATTR_NAME_TOO_LONG                  3
#define NV_PARSER_STATUS_ATTR_NAME_MISSING                   4
#define NV_PARSER_STATUS_BAD_DISPLAY_DEVICE                  5
#define NV_PARSER_STATUS_MISSING_EQUAL_SIGN                  6
#define NV_PARSER_STATUS_NO_VALUE                            7
#define NV_PARSER_STATUS_TRAILING_GARBAGE                    8
#define NV_PARSER_STATUS_UNKNOWN_ATTR_NAME                   9
#define NV_PARSER_STATUS_MISSING_COMMA                      10
#define NV_PARSER_STATUS_TARGET_SPEC_NO_COLON               11
#define NV_PARSER_STATUS_TARGET_SPEC_BAD_TARGET             12
#define NV_PARSER_STATUS_TARGET_SPEC_NO_TARGET_ID           13
#define NV_PARSER_STATUS_TARGET_SPEC_BAD_TARGET_ID          14
#define NV_PARSER_STATUS_TARGET_SPEC_TRAILING_GARBAGE       15
#define NV_PARSER_STATUS_TARGET_SPEC_NO_TARGETS             16

/*
 * define useful types
 */

typedef unsigned int uint32;


/*
 * flags common to all attributes ('flags' in AttributeTableEntry)
 */

typedef struct AttributeFlagsRec {
    int is_gui_attribute       : 1;
    int is_framelock_attribute : 1;
    int is_sdi_attribute       : 1;
    int hijack_display_device  : 1;
    int no_config_write        : 1;
    int no_query_all           : 1;
} AttributeFlags;


/*
 * flags specific to integer attributes ('type_flags' in AttributeTableEntry)
 */

typedef struct AttributeIntFlagsRec {
    int is_100Hz          : 1;
    int is_1000Hz         : 1;
    int is_packed         : 1;
    int is_display_mask   : 1;
    int is_display_id     : 1;
    int no_zero           : 1;
    int is_switch_display : 1;
} AttributeIntFlags;


/*
 * The valid attribute names, and their corresponding protocol
 * attribute identifiers are stored in an array of
 * AttributeTableEntries.
 */

typedef struct _AttributeTableEntry {
    char *name;
    int attr;

    CtrlAttributeType type;
    AttributeFlags flags;
    union {
        AttributeIntFlags int_flags;
    } f;

    char *desc;
} AttributeTableEntry;


/*
 * ParsedAttribute - struct filled out by
 * nv_parse_attribute_string().
 */

typedef struct _ParsedAttribute {
    struct _ParsedAttribute *next;

    char *display;
    char *target_specification;
    /*
     * The target_type and target_id here are mostly set by the GUI to store
     * target-specific information, as well as the cmd line for handling the
     * case where an X screen is specified as part of the display (e.g.
     * "localhost:0.1").  Note that if the target_specification is specified,
     * the target_type and target_id are ignored when resolving to the list of
     * targets that should be operated on.
     */
    int target_type;
    int target_id;
    const AttributeTableEntry *attr_entry;
    union {
        int i;
        float f;
        const float *pf;
        char *str;
    } val;
    char *display_device_specification;
    uint32 display_device_mask;

    struct {
        int has_x_display       : 1;
        int has_target          : 1;
        int has_display_device  : 1;
        int has_value           : 1;
        int assign_all_displays : 1;
    } parser_flags;

    /*
     * Upon being resolved, the ParsedAttribute's target_type and target_id,
     * and/or target_specification get converted into a list of targets to
     * which the attribute should be processed.
     */
    struct _CtrlTargetNode *targets;
} ParsedAttribute;



/*
 * Attribute table; defined in parse.c
 */

extern const AttributeTableEntry attributeTable[];
extern const int attributeTableLen;



/* nv_get_sdi_csc_matrxi() - Returns an array of floats that specifies
 * all the color, offset and scale values for specifying one of the
 * Standard CSC. 's' is a string that names the matrix values to return.
 * The values are placed in the float buffer like so:
 *
 * { YR,  YG,  YB,   YOffset,  YScale,
 *   CrR, CrG, CrB,  CrOffset, CrScale,
 *   CbR, CbG, CbB,  CbOffset, CbScale }
 *
 */
const float * nv_get_sdi_csc_matrix(char *s);

/*
 * nv_parse_attribute_string() - this function parses an attribute
 * string, the syntax for which is:
 *
 * {host}:{display}.{screen}/{attribute name}[{display devices}]={value}
 *
 * {host}:{display}.{screen} indicates which X server and X screen to
 * interact with; this is optional.  If no X server is specified, then
 * the default X server is used.  If no X screen is specified, then
 * all X screens on the X server are used.
 *
 * {screen}/ may be specified by itself (i.e.: without the
 * "{host}:{display}." part).
 *
 * Additionally, instead of specifying a screen, a target
 * specification (target type and id) may be given in brackets:
 *
 *     [{target-type}:{target-id}]/{attribute name}...
 *
 * This can be used in place of "{screen}" when it is used by itself
 * on the left of the "/"; or, it can take the place of ".{screen}"
 * when used along with an X Display name:
 *
 *     {host}:{display}[{target-type}:{target-id}]/{attribute name}...
 *
 * {attribute name} should be a string without any whitespace (a case
 * insensitive compare will be done to find a match in the
 * attributeTable in parse.c).  {value} should be an integer.
 *
 * {display devices} is optional.  If no display mask is specified,
 * then all display devices are assumed.
 *
 * The query parameter controls whether the attribute string is parsed
 * for setting or querying.  If query == NV_PARSER_SET, then the
 * attribute string will be interpreted as described above.  If query
 * == NV_PARSER_QUERY, the "={value}" portion of the string should be
 * omitted.
 *
 * If successful, NV_PARSER_STATUS_SUCCESS will be returned and the
 * ParsedAttribute struct will contain the attribute id corresponding
 * to the attribute name.  If the X server or display devices were
 * given in the string, then those fields will be appropriately
 * assigned in the ParsedAttribute struct, and the
 * NV_PARSER_HAS_X_SERVER and NV_PARSER_HAS_DISPLAY_DEVICE_MASK bits
 * will be set in the flags field.
 */

int nv_parse_attribute_string(const char *, int, ParsedAttribute *);


/*
 * nv_assign_default_display() - assigns the display name to the
 * ParsedAttribute struct.  As a side affect, also assigns the screen
 * field, if a screen is specified in the display name.
 */

void nv_assign_default_display(ParsedAttribute *p, const char *display);


/*
 * nv_parse_strerror() - given the error status returned by
 * nv_parse_attribute_string(), this function returns a string
 * describing the error.
 */

const char *nv_parse_strerror(int);

int nv_strcasecmp(const char *, const char *);


char *remove_spaces(const char *o);
char *replace_characters(const char *o, const char c, const char r);

/*
 * display_mask/display_name conversions: the NV-CONTROL X extension
 * identifies a display device by a bit in a display device mask.  The
 * below functions translate between a display mask, and a string
 * describing the display devices.
 */

#define BITSHIFT_CRT  0
#define BITSHIFT_TV   8
#define BITSHIFT_DFP 16

#define BITMASK_ALL_CRT (0xff << BITSHIFT_CRT)
#define BITMASK_ALL_TV  (0xff << BITSHIFT_TV)
#define BITMASK_ALL_DFP (0xff << BITSHIFT_DFP)

#define INVALID_DISPLAY_DEVICE_MASK   (0xffffffff)

#define VALID_DISPLAY_DEVICES_MASK    (0x00ffffff)
#define DISPLAY_DEVICES_WILDCARD_MASK (0xff000000)

#define DISPLAY_DEVICES_WILDCARD_CRT  (1 << 24)
#define DISPLAY_DEVICES_WILDCARD_TV   (1 << 25)
#define DISPLAY_DEVICES_WILDCARD_DFP  (1 << 26)



char *display_device_mask_to_display_device_name(const uint32);

uint32 expand_display_device_mask_wildcards(const uint32);

ParsedAttribute *nv_parsed_attribute_init(void);

void nv_parsed_attribute_add(ParsedAttribute *head, ParsedAttribute *p);

void nv_parsed_attribute_free(ParsedAttribute *p);
void nv_parsed_attribute_clean(ParsedAttribute *p);

const AttributeTableEntry *nv_get_attribute_entry(const int attr,
                                                  const CtrlAttributeType type);

char *nv_standardize_screen_name(const char *display_name, int screen);



/* General parsing functions */

int nv_parse_numerical(const char *start, const char *end, int *val);
const char *parse_skip_whitespace(const char *str);
void parse_chop_whitespace(char *str);
const char *parse_skip_integer(const char *str);
const char *parse_read_integer(const char *str, int *num);
const char *parse_read_integer_pair(const char *str,
                                    const char separator, int *a, int *b);
const char *parse_read_name(const char *str, char **name, char term);
const char *parse_read_display_name(const char *str, unsigned int *mask);
const char *parse_read_display_id(const char *str, unsigned int *id);
int parse_read_float_range(const char *str, float *min, float *max);
char **nv_strtok(char *s, char c, int *n);
void nv_free_strtoks(char **s, int n);
int count_number_of_bits(unsigned int mask);

/* Token parsing functions */

typedef void (* apply_token_func)(char *token, char *value, void *data);

int parse_token_value_pairs(const char *str, apply_token_func func,
                            void *data);



#endif /* __PARSE_H__ */
