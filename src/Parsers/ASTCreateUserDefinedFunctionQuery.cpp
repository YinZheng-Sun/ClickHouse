#include <IO/Operators.h>
#include <Parsers/ASTCreateUserDefinedFunctionQuery.h>
#include <Common/quoteString.h>

namespace DB
{
ASTPtr ASTCreateUserDefinedFunctionQuery::clone() const
{
    auto res = std::make_shared<ASTCreateUserDefinedFunctionQuery>(*this);
    // res->children.clear();

    // res->function_name = function_name->clone();
    // res->children.push_back(res->function_name);

    // res->function_core = function_core->clone();
    // res->children.push_back(res->function_core);
    return res;
}

void ASTCreateUserDefinedFunctionQuery::formatImpl(
    const IAST::FormatSettings & settings, IAST::FormatState & state, IAST::FormatStateStacked frame) const
{
    settings.ostr << (settings.hilite ? hilite_keyword : "") << "CREATE FUCTION " << (settings.hilite ? hilite_none : "");
    if (!database_name.empty())
    {
        settings.ostr << (settings.hilite ? hilite_identifier : "") << backQuoteIfNeed(database_name)
                      << (settings.hilite ? hilite_none : "");
        settings.ostr << (settings.hilite ? hilite_identifier : "") << "." << (settings.hilite ? hilite_none : "");
    }

    UDF::UDFLanguage language = UDF::getLanguage(flags);
    UDF::UDFFunctionType type = UDF::getFunctionType(flags);
    settings.ostr << (settings.hilite ? hilite_identifier : "") << backQuoteIfNeed(function_name) << (settings.hilite ? hilite_none : "");
    settings.ostr << (settings.hilite ? hilite_keyword : "") << " TYPE " << (settings.hilite ? hilite_none : "");
    settings.ostr << (settings.hilite ? hilite_identifier : "") << UDF::getFunctionTypeStr(type) << (settings.hilite ? hilite_none : "");
    if (return_type)
    {
        settings.ostr << (settings.hilite ? hilite_keyword : "") << " RETURNS " << (settings.hilite ? hilite_none : "");
        settings.ostr << (settings.hilite ? hilite_identifier : "") << return_type->getName() << (settings.hilite ? hilite_none : "");
    }
    if (state_type)
    {
        settings.ostr << (settings.hilite ? hilite_keyword : "") << " STATE_TYPE " << (settings.hilite ? hilite_none : "");
        settings.ostr << (settings.hilite ? hilite_identifier : "") << state_type->getName() << (settings.hilite ? hilite_none : "");
    }
    settings.ostr << (settings.hilite ? hilite_keyword : "") << " LANGUAGE " << (settings.hilite ? hilite_none : "");
    settings.ostr << (settings.hilite ? hilite_identifier : "") << backQuoteIfNeed(UDF::getLanguageStr(language)) << (settings.hilite ? hilite_none : "");
    settings.ostr << (settings.hilite ? hilite_keyword : "") << " AS " << (settings.hilite ? hilite_none : "");

    switch (language)
    {
        case UDF::UDFLanguage::Sql:
            function_core->formatImpl(settings, state, frame);
            break;
        case UDF::UDFLanguage::Python: {
            settings.ostr << "\n" << tag << (settings.hilite ? hilite_none : "");
            settings.ostr << function_body;
            settings.ostr << "\n" << tag << (settings.hilite ? hilite_none : "");
            break;
        }
    }
    // if (or_replace)
    //     settings.ostr << "OR REPLACE ";

    // settings.ostr << "FUNCTION ";

    // if (if_not_exists)
    //     settings.ostr << "IF NOT EXISTS ";

    // settings.ostr << (settings.hilite ? hilite_none : "");

    // settings.ostr << (settings.hilite ? hilite_identifier : "") << backQuoteIfNeed(getFunctionName()) << (settings.hilite ? hilite_none : "");

    // formatOnCluster(settings);

    // settings.ostr << (settings.hilite ? hilite_keyword : "") << " AS " << (settings.hilite ? hilite_none : "");
    // function_core->formatImpl(settings, state, frame);
}

// String ASTCreateFunctionQuery::getFunctionName() const
// {
//     String name;
//     tryGetIdentifierNameInto(function_name, name);
//     return name;
// }
}
