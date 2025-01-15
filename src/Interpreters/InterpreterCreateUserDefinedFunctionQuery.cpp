#include <Interpreters/InterpreterCreateUserDefinedFunctionQuery.h>
#include <Interpreters/InterpreterFactory.h>

#include <Access/ContextAccess.h>
#include <Functions/UserDefined/IUserDefinedSQLObjectsStorage.h>
#include <Functions/UserDefined/UserDefinedSQLFunctionFactory.h>
#include <Interpreters/Context.h>
// #include <Interpreters/executeDDLQueryOnCluster.h>
// #include <Interpreters/removeOnClusterClauseIfNeeded.h>
#include <Interpreters/FunctionNameNormalizer.h>
#include <Parsers/ASTCreateUserDefinedFunctionQuery.h>


namespace DB
{

namespace ErrorCodes
{
extern const int INCORRECT_QUERY;
}

BlockIO InterpreterCreateUserDefinedFunctionQuery::execute()
{
    FunctionNameNormalizer().visit(query_ptr.get());
    // const auto updated_query_ptr = removeOnClusterClauseIfNeeded(query_ptr, getContext());
    auto * create_function_query = query_ptr->as<ASTCreateUserDefinedFunctionQuery>();
    // ASTCreateUserDefinedFunctionQuery & create_function_query = query_ptr->as<ASTCreateUserDefinedFunctionQuery &>();

    // AccessRightsElements access_rights_elements;
    // access_rights_elements.emplace_back(AccessType::CREATE_FUNCTION);

    // if (create_function_query.or_replace)
    //     access_rights_elements.emplace_back(AccessType::DROP_FUNCTION);

    // if (create_function_query->database_name.empty())
    //     create_function_query->database_name = context.getCurrentDatabase();

    // if (create_function_query->database_name == "default")
    //     throw Exception("Must specify a database for UDF", ErrorCodes::UNKNOWN_DATABASE);
    auto current_context = getContext();

    // if (!create_function_query.cluster.empty())
    // {
    //     if (current_context->getUserDefinedSQLObjectsStorage().isReplicated())
    //         throw Exception(ErrorCodes::INCORRECT_QUERY, "ON CLUSTER is not allowed because used-defined functions are replicated automatically");

    //     DDLQueryOnClusterParams params;
    //     params.access_to_check = std::move(access_rights_elements);
    //     return executeDDLQueryOnCluster(updated_query_ptr, current_context, params);
    // }

    // current_context->checkAccess(access_rights_elements);

    // auto function_name = create_function_query.getFunctionName();
    // bool throw_if_exists = !create_function_query.if_not_exists && !create_function_query.or_replace;
    // bool replace_if_exists = create_function_query.or_replace;

    UserDefinedSQLFunctionFactory::instance().registerUserDefinedFunction(
        current_context, create_function_query->function_name, query_ptr);
    return {};
}

void registerInterpreterCreateUserDefinedFunctionQuery(InterpreterFactory & factory)
{
    auto create_fn = [](const InterpreterFactory::Arguments & args)
    { return std::make_unique<InterpreterCreateUserDefinedFunctionQuery>(args.query, args.context); };
    factory.registerInterpreter("InterpreterCreateUserDefinedFunctionQuery", create_fn);
}

}
