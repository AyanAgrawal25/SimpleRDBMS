#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT_MATRIX relation_name
 */
bool syntacticParsePRINT_MATRIX()
{
    logger.log("syntacticParsePRINT_MATRIX");
    if (tokenizedQuery.size() != 3 ||
        tokenizedQuery[1] != "MATRIX")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINT_MATRIX;
    parsedQuery.printRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParsePRINT_MATRIX()
{
    logger.log("semanticParsePRINT_MATRIX");
    if (!matrixCatalogue.isMatrix(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINT_MATRIX()
{
    logger.log("executePRINT_MATRIX");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.printRelationName);
    matrix->print();
    return;
}
