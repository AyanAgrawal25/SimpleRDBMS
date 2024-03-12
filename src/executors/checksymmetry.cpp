#include "global.h"
/**
 * @brief 
 * SYNTAX: CHECKSYMMETRY <matrix_name>
 */
bool syntacticParseCHECKSYMMETRY()
{
    logger.log("syntacticParseCHECKSYMMETRY");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.checksymmetryMatrixName = tokenizedQuery[1];
    return true;
}

bool semanticParseCHECKSYMMETRY()
{
    logger.log("semanticParseCHECKSYMMETRY");

    if (!matrixCatalogue.isMatrix(parsedQuery.checksymmetryMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }

    return true;
}

void executeCHECKSYMMETRY()
{
    logger.log("executeCHECKSYMMETRY");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.checksymmetryMatrixName);
    if (matrix->isSymmetric()) {
        cout << "TRUE" << endl;
    } else {
        cout << "FALSE" << endl;
    }
    return;
}