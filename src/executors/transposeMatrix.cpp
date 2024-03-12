#include "global.h"

/**
 * @brief
 * SYNTAX: TRANSPOSE <relation_name>
 */

bool syntacticParseTRANSPOSE_MATRIX()
{
    logger.log("syntacticParseTRANSPOSE_MATRIX");
    if (tokenizedQuery.size() != 3 || tokenizedQuery[1] != "MATRIX")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = TRANSPOSE_MATRIX;
    parsedQuery.exportRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseTRANSPOSE_MATRIX()
{
    logger.log("semanticParseTRANSPOSE_MATRIX");
    // Matrix should exist
    if (matrixCatalogue.isMatrix(parsedQuery.exportRelationName))
        return true;
    cout << "SEMANTIC ERROR: No such relation exists" << endl;
    return false;
}

void executeTRANSPOSE_MATRIX()
{
    logger.log("executeTRANSPOSE_MATRIX");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.exportRelationName);
    matrix->transpose();
    return;
}