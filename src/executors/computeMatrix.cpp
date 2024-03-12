#include "global.h"
/**
 * @brief
 * SYNTAX: COMPUTE relation_name
 */
bool syntacticParseCOMPUTE_MATRIX()
{
    logger.log("syntacticParseCOMPUTE_MATRIX");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = COMPUTE_MATRIX;
    parsedQuery.loadRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseCOMPUTE_MATRIX()
{
    logger.log("semanticParseCOMPUTE_MATRIX");
    if (matrixCatalogue.isMatrix(parsedQuery.loadRelationName))
        return true;

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCOMPUTE_MATRIX()
{
    logger.log("executeCOMPUTE_MATRIX");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.loadRelationName);
    matrix->compute();
    return;
}