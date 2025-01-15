#include <Parsers/ParserCreateUserDefinedFunctionQuery.h>
#include <Parsers/ASTCreateUserDefinedFunctionQuery.h>
#include <Parsers/ExpressionElementParsers.h>
#include <Parsers/CommonParsers.h>
#include <Parsers/ExpressionListParsers.h>
#include <Parsers/ASTIdentifier.h>
#include <DataTypes/DataTypeFactory.h>
#include <IO/ReadBufferFromMemory.h>
#include <IO/ReadHelpers.h>


namespace DB
{

namespace ErrorCodes {
    extern const int INCORRECT_UDF_FUNCTION_NAME;
};
bool ParserCreateUserDefinedFunctionQuery::parseImpl(IParser::Pos & pos, ASTPtr & node, Expected & expected)
{
    ParserKeyword s_create("CREATE");
    ParserKeyword s_function("FUNCTION");
    ParserIdentifier function_name_p;
    ParserKeyword s_function_type("TYPE");
    ParserIdentifier function_type_p;
    ParserKeyword s_returns("RETURNS");
    ParserIdentifierWithOptionalParameters return_type_p;
    ParserKeyword s_state_type("STATE_TYPE");
    ParserIdentifierWithOptionalParameters state_type_p;
    ParserKeyword s_language("LANGUAGE");
    ParserIdentifier function_language_p;
    ParserKeyword s_as("AS");
    ParserExpression lambda_p;

    ASTPtr function_name;
    ASTPtr database_name;
    ASTPtr function_type;
    ASTPtr function_language;
    ASTPtr function_core;
    String language = UDF::SQL;
    String type = UDF::SCALAR;
    ASTPtr return_type;
    ASTPtr state_type;
    DataTypePtr returnDataType = nullptr;
    DataTypePtr stateDataType = nullptr;
    ParserToken s_dot(TokenType::Dot);
    UDF::UDFFlags flags = 0;

    if (!s_create.ignore(pos, expected))
        return false;

    if (!s_function.ignore(pos, expected))
        return false;

    if (!function_name_p.parse(pos, function_name, expected))
        return false;

    if (s_dot.ignore(pos, expected))
    {
        database_name = function_name;
        if (!function_name_p.parse(pos, function_name, expected))
            return false;
    }

    if (s_function_type.ignore(pos, expected)) {
        if (!function_type_p.parse(pos, function_type, expected)) {
            return false;
        } else {
            type = function_type->as<ASTIdentifier &>().name();
        }
    }

    if (!UDF::setFunctionType(flags, type.c_str())) {
        return false;
    }

    if (s_returns.ignore(pos, expected)) {
        if (!return_type_p.parse(pos, return_type, expected)) {
            return false;
        } else {
            returnDataType = DataTypeFactory::instance().get(return_type);
        }
    }

    if (s_state_type.ignore(pos, expected)) {
        if (!state_type_p.parse(pos, state_type, expected)) {
            return false;
        } else {
            stateDataType = DataTypeFactory::instance().get(state_type);
        }
    }

    if (s_language.ignore(pos, expected)) {
        if (!function_language_p.parse(pos, function_language, expected)) {
            return false;
        } else {
            language = function_language->as<ASTIdentifier &>().name();
        }
    }

    if (!UDF::setLangauge(flags, language.c_str())) {
        return false;
    }

    if (!s_as.ignore(pos, expected))
        return false;

    String function_body;
    String tag;

    UDF::UDFLanguage lang = UDF::getLanguage(flags);

    switch(lang) {
        case UDF::UDFLanguage::Sql: {
            if (!lambda_p.parse(pos, function_core, expected)) {
                return false;
            }
            break;
        }
        case UDF::UDFLanguage::Python: {
            ReadBufferFromMemory buf(pos->begin, pos->size());
            if (!readDollarQuotedStringInto(function_body, buf, tag, pos->size())) {
                return false;
            }
            ++pos;
            break;
        }
    }

    auto create_function_query = std::make_shared<ASTCreateUserDefinedFunctionQuery>();
    node = create_function_query;

    if (database_name) {
        create_function_query->database_name = Poco::toLower(database_name->as<ASTIdentifier &>().name());
    }

    // const String &f_name = function_name->as<ASTIdentifier &>().name();

    create_function_query->function_name = Poco::toLower(function_name->as<ASTIdentifier &>().name());
    create_function_query->return_type = returnDataType;
    create_function_query->state_type = stateDataType;
    create_function_query->function_core = function_core;
    create_function_query->flags = flags;
    create_function_query->function_body = function_body;
    create_function_query->tag = tag;

    return true;
}
}
