#pragma once
#include "tableCatalogue.h"
#include "matrixCatalogue.h"
#include<functional>

using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    EXPORT_MATRIX,
    INDEX,
    JOIN,
    ORDER_BY,
    GROUP_BY,
    LIST,
    LOAD,
    LOAD_MATRIX,
    TRANSPOSE_MATRIX,
    COMPUTE_MATRIX,
    PRINT,
    PRINT_MATRIX,
    PROJECTION,
    RENAME,
    RENAME_MATRIX,
    CHECKSYMMETRY,
    SELECTION,
    SORT,
    SOURCE,
    UNDETERMINED
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum AggregateOperator
{
    MIN,
    MAX,
    SUM,
    AVG,
    NO_AGGR_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string orderByRelationName = "";
    string orderByColumnName = "";
    string orderByResultRelationName = "";
    SortingStrategy orderBySortingStrategy = NO_SORT_CLAUSE;

    string groupByRelationName = "";
    string groupByGroupingAttribute = "";
    string groupByResultRelationName = "";
    string groupByHavingAggregateAttribute = "";
    AggregateOperator groupByHavingAggregateOperator = NO_AGGR_CLAUSE;
    BinaryOperator groupByHavingBinaryOperator = NO_BINOP_CLAUSE;
    int groupByHavingValue = 0;
    string groupByAggregateAttribute = "";
    string groupByAggregateOperatorLiteral = "";
    AggregateOperator groupByAggregateOperator = NO_AGGR_CLAUSE;   

    string loadRelationName = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    string renameFromMatrixName = "";
    string renameToMatrixName = "";

    string checksymmetryMatrixName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    vector<SortingStrategy> sortingStrategies;
    string sortResultRelationName = "";
    vector<string> sortColumnNames;
    string sortRelationName = "";

    string sourceFileName = "";

    ParsedQuery();
    void clear();

    static BinaryOperator getBinaryOperator(const string &token);
    static bool getAggregateOperatorAndAttribute(const string &token, AggregateOperator &aggOpr, string &aggAttr);
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseEXPORT_MATRIX();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParseLOAD_MATRIX();
bool syntacticParseTRANSPOSE_MATRIX();
bool syntacticParseCOMPUTE_MATRIX();
bool syntacticParsePRINT();
bool syntacticParsePRINT_MATRIX();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseRENAME_MATRIX();
bool syntacticParseCHECKSYMMETRY();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();
bool syntacticParseGROUP_BY();
bool syntacticParseORDER_BY();

bool isFileExists(string tableName);
bool isQueryFile(string fileName);