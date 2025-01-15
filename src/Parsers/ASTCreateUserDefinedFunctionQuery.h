#pragma once
#include <Parsers/IAST.h>
#include <DataTypes/IDataType.h>
#include <Functions/UDFFlags.h>

namespace DB
{
class ASTCreateUserDefinedFunctionQuery : public IAST
{
public:
    String function_name;
    String database_name;
    String function_language;
    uint64_t version{0};
    ASTPtr function_core;
    String function_body;
    DataTypePtr return_type;
    DataTypePtr state_type;
    String tag;
    UDF::UDFFlags flags;


    String getID(char) const override { return "CreateUserDefinedFunctionQuery"; }

    ASTPtr clone() const override;

    void formatImpl(const FormatSettings & s, FormatState & state, FormatStateStacked frame) const override;
};
}
