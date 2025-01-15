#pragma once
#include <cstdint>
#include <limits>
#include <string>
#include <string.h>
#include <Common/BitHelpers.h>
#include <IO/ReadBufferFromMemory.h>



namespace DB::UDF
{
constexpr auto SQL = "SQL";
constexpr auto PYTHON = "PYTHON";
constexpr auto SCALAR = "SCALAR";
constexpr auto AGGREGATE = "AGGREGATE";
constexpr auto ML = "ML";
using UDFFlags = uint32_t;
enum class UDFLanguage
{
    Sql,
    Python
};
enum class UDFFunctionType
{
    Scalar,
    Aggregate,
    ML
};
static const char * Languages[2] = {SQL, PYTHON};
static const char * FunctionTypes[3] = {SCALAR, AGGREGATE, ML};
#define UDF_FIELD_TYPE GENMASK(28, 25)
#define UDF_FIELD_LANG GENMASK(31, 29)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
static inline int getEnumValFromString(const char* enum_name, const char * const * arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (strcmp(arr[i], enum_name) == 0) {
            return i;
        }
    }
    return -1;
}
#pragma clang diagnostic pop
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
static inline enum UDFLanguage getLanguage(UDFFlags flags) {
    return static_cast<UDFLanguage>(FIELD_GET(UDF_FIELD_LANG, flags));
}
static inline enum UDFFunctionType getFunctionType(UDFFlags flags)
{
    return static_cast<UDFFunctionType>(FIELD_GET(UDF_FIELD_TYPE, flags));
}
static inline bool setFlag(UDFFlags &flags, const char *value, const char** enums, size_t n, uint64_t mask) {
    int z = getEnumValFromString(value, enums, n);
    if (z == -1) {
        return false;
    }
    flags |= FIELD_PREP(mask, z);
    return true;
}
#pragma clang diagnostic pop
static inline const char * getLanguageStr(UDFLanguage language)  {
    return Languages[to_underlying(language)];
}
static inline const char * getFunctionTypeStr(UDFFunctionType func_type)
{
    return FunctionTypes[to_underlying(func_type)];
}
static inline bool setFunctionType(UDFFlags &flags, const char *type) {
    return setFlag(flags, type, FunctionTypes, sizeof(FunctionTypes), UDF_FIELD_TYPE);
}
static inline bool setLangauge(UDFFlags &flags, const char *language) {
    return setFlag(flags, language, Languages, sizeof(Languages), UDF_FIELD_LANG);
}
}
