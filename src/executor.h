#include"semanticParser.h"

void executeCommand();

void executeCLEAR();
void executeCROSS();
void executeDISTINCT();
void executeEXPORT();
void executeEXPORT_MATRIX();
void executeINDEX();
void executeJOIN();
void executeORDER_BY();
void executeGROUP_BY();
void executeLIST();
void executeLOAD();
void executeLOAD_MATRIX();
void executeTRANSPOSE_MATRIX();
void executeCOMPUTE_MATRIX();
void executePRINT();
void executePRINT_MATRIX();
void executePROJECTION();
void executeRENAME();
void executeRENAME_MATRIX();
void executeCHECKSYMMETRY();
void executeSELECTION();
void executeSORT();
void executeSOURCE();

bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);
template<typename T>
function<bool(const T&, const T&)> getBinaryComparator(BinaryOperator binaryOperator);
void printRowCount(int rowCount);
void printMatrixRowAndColumnCount(int rowCount, int columnCount);
void printBlockInformation();
