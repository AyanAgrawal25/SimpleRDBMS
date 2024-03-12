#include "global.h"
/**
 * @brief 
 * SYNTAX: RENAME_MATRIX matrix_name TO new_matrix_name FROM relation_name
 */
bool syntacticParseRENAME_MATRIX()
{
    logger.log("syntacticParseRENAME_MATRIX");
    if (tokenizedQuery.size() != 4 || tokenizedQuery[1] != "MATRIX")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = RENAME_MATRIX;
    parsedQuery.renameFromMatrixName = tokenizedQuery[2];
    parsedQuery.renameToMatrixName = tokenizedQuery[3];
    return true;
}

bool semanticParseRENAME_MATRIX()
{
    logger.log("semanticParseRENAME_MATRIX");

    if (!matrixCatalogue.isMatrix(parsedQuery.renameFromMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }

    if (matrixCatalogue.isMatrix(parsedQuery.renameToMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix with name (to be renamed to) already exists" << endl;
        return false;
    }

    return true;
}

void executeRENAME_MATRIX()
{
    logger.log("executeRENAME_MATRIX");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.renameFromMatrixName);
    if (matrix->rename(parsedQuery.renameToMatrixName)) {
        cout << "Matrix renamed." << endl;
    }
    return;
}