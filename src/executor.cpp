#include"global.h"

void executeCommand(){

    switch(parsedQuery.queryType){
        case CLEAR: executeCLEAR(); break;
        case CROSS: executeCROSS(); break;
        case DISTINCT: executeDISTINCT(); break;
        case EXPORT: executeEXPORT(); break;
        case EXPORT_MATRIX: executeEXPORT_MATRIX();break;
        case INDEX:
            executeINDEX();
            break;
        case JOIN: executeJOIN(); break;
        case GROUP_BY: executeGROUP_BY(); break;
        case ORDER_BY: executeORDER_BY(); break;
        case LIST: executeLIST(); break;
        case LOAD: executeLOAD(); break;
        case LOAD_MATRIX: executeLOAD_MATRIX(); break;
        case TRANSPOSE_MATRIX: executeTRANSPOSE_MATRIX(); break;
        case COMPUTE_MATRIX: executeCOMPUTE_MATRIX(); break;
        case PRINT:
            executePRINT();
            break;
        case PRINT_MATRIX: executePRINT_MATRIX(); break;
        case PROJECTION: executePROJECTION(); break;
        case RENAME: executeRENAME(); break;
        case RENAME_MATRIX: executeRENAME_MATRIX(); break;
        case CHECKSYMMETRY: executeCHECKSYMMETRY(); break;
        case SELECTION: executeSELECTION(); break;
        case SORT: executeSORT(); break;
        case SOURCE: executeSOURCE(); break;
        default: cout<<"PARSING ERROR"<<endl;
    }

    return;
}


/**
 * @brief Returns the binary comparator function for the given binary operator
 * 
 * @tparam T the typename of the operands
 * @param binaryOperator the binary operator
 * 
 * @return function<bool(const T&, const T&)> the binary comparator function
 */
template std::function<bool(const int&, const int&)> getBinaryComparator<int>(BinaryOperator binaryOperator);
template<typename T = int>
function<bool(const T&, const T&)> getBinaryComparator(BinaryOperator binaryOperator) {
    function<bool(const T&, const T&)> comp;
    switch (binaryOperator)
    {
    case BinaryOperator::EQUAL :
        comp = equal_to<T>();
        break;
    case BinaryOperator::GREATER_THAN :
        comp = greater<T>();
        break;
    case BinaryOperator::GEQ :
        comp = greater_equal<T>();
        break;
    case BinaryOperator::LESS_THAN :
        comp = less<T>();
        break;
    case BinaryOperator::LEQ :
        comp = less_equal<T>();
        break;
    case BinaryOperator::NOT_EQUAL :
        comp = not_equal_to<T>();
        break;
    default:
        throw std::runtime_error("Invalid binary operator");
        break;
    }
    return comp;
}

void printRowCount(int rowCount){
    cout<<"\n\nRow Count: "<<rowCount<<endl;
    return;
}

void printMatrixRowAndColumnCount(int rowCount, int columnCount){
    cout<<"\n\nRow Count: "<< rowCount << ", Column Count: " << columnCount << endl;
    return;
}

void printBlockInformation() {
    const uint blocksAccessed = blocksRead + blocksWritten;
    cout << "\n\n";
    cout << "Number of blocks read: " << blocksRead << "\n";
    cout << "Number of blocks written: " << blocksWritten << "\n";
    cout << "Number of blocks accessed: " << blocksAccessed << endl;
}