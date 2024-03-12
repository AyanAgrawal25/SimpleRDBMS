#include "global.h"

bool syntacticParse()
{
    logger.log("syntacticParse");
    string possibleQueryType = tokenizedQuery[0];

    if (tokenizedQuery.size() < 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    if (possibleQueryType == "CLEAR")
        return syntacticParseCLEAR();
    else if (possibleQueryType == "INDEX")
        return syntacticParseINDEX();
    else if (possibleQueryType == "LIST")
        return syntacticParseLIST();
    else if (possibleQueryType == "LOAD") {
        if (tokenizedQuery.size() > 2)
            return syntacticParseLOAD_MATRIX();
        return syntacticParseLOAD();
    }
    else if (possibleQueryType == "TRANSPOSE") {
        return syntacticParseTRANSPOSE_MATRIX();
    }
    else if (possibleQueryType == "COMPUTE"){
        return syntacticParseCOMPUTE_MATRIX();
    }
    else if (possibleQueryType == "PRINT") {
        if (tokenizedQuery.size() > 2)
            return syntacticParsePRINT_MATRIX();
        return syntacticParsePRINT();
    } else if (possibleQueryType == "RENAME") {
        if (tokenizedQuery.size() < 6)
            return syntacticParseRENAME_MATRIX();
        return syntacticParseRENAME();
    } else if (possibleQueryType == "CHECKSYMMETRY") {
        return syntacticParseCHECKSYMMETRY();
    } else if(possibleQueryType == "EXPORT") {
        if(tokenizedQuery.size() > 2)
            return syntacticParseEXPORT_MATRIX();
        return syntacticParseEXPORT();
    } else if (possibleQueryType == "SORT") {
        return syntacticParseSORT();
    } else if(possibleQueryType == "SOURCE")
        return syntacticParseSOURCE();
    else
    {
        string resultantRelationName = possibleQueryType;
        if (tokenizedQuery[1] != "<-" || tokenizedQuery.size() < 3)
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        possibleQueryType = tokenizedQuery[2];
        if (possibleQueryType == "PROJECT")
            return syntacticParsePROJECTION();
        else if (possibleQueryType == "SELECT")
            return syntacticParseSELECTION();
        else if (possibleQueryType == "JOIN")
            return syntacticParseJOIN();
        else if (possibleQueryType == "ORDER")
            return syntacticParseORDER_BY();
        else if (possibleQueryType == "GROUP")
            return syntacticParseGROUP_BY();
        else if (possibleQueryType == "CROSS")
            return syntacticParseCROSS();
        else if (possibleQueryType == "DISTINCT")
            return syntacticParseDISTINCT();
        else
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }
    return false;
}

ParsedQuery::ParsedQuery()
{
}

void ParsedQuery::clear()
{
    logger.log("ParseQuery::clear");
    this->queryType = UNDETERMINED;

    this->clearRelationName = "";

    this->crossResultRelationName = "";
    this->crossFirstRelationName = "";
    this->crossSecondRelationName = "";

    this->distinctResultRelationName = "";
    this->distinctRelationName = "";

    this->exportRelationName = "";

    this->indexingStrategy = NOTHING;
    this->indexColumnName = "";
    this->indexRelationName = "";

    this->joinBinaryOperator = NO_BINOP_CLAUSE;
    this->joinResultRelationName = "";
    this->joinFirstRelationName = "";
    this->joinSecondRelationName = "";
    this->joinFirstColumnName = "";
    this->joinSecondColumnName = "";

    this->orderByRelationName = "";
    this->orderByColumnName = "";
    this->orderByResultRelationName = "";
    this->orderBySortingStrategy = NO_SORT_CLAUSE;

    this->groupByRelationName = "";
    this->groupByGroupingAttribute = "";
    this->groupByResultRelationName = "";
    this->groupByHavingAggregateAttribute = "";
    this->groupByAggregateOperatorLiteral = "";
    this->groupByHavingAggregateOperator = NO_AGGR_CLAUSE;
    this->groupByHavingBinaryOperator = NO_BINOP_CLAUSE;
    this->groupByHavingValue = 0;
    this->groupByAggregateAttribute = "";
    this->groupByAggregateOperator = NO_AGGR_CLAUSE;   

    this->loadRelationName = "";

    this->printRelationName = "";

    this->projectionResultRelationName = "";
    this->projectionColumnList.clear();
    this->projectionRelationName = "";

    this->renameFromColumnName = "";
    this->renameToColumnName = "";
    this->renameRelationName = "";
    
    this->renameFromColumnName = "";
    this->renameToColumnName = "";

    this->selectType = NO_SELECT_CLAUSE;
    this->selectionBinaryOperator = NO_BINOP_CLAUSE;
    this->selectionResultRelationName = "";
    this->selectionRelationName = "";
    this->selectionFirstColumnName = "";
    this->selectionSecondColumnName = "";
    this->selectionIntLiteral = 0;

    this->sortingStrategies.clear();
    this->sortResultRelationName = "";
    this->sortColumnNames.clear();
    this->sortRelationName = "";

    this->sourceFileName = "";
}

/**
 * @brief Checks to see if source file exists. Called when LOAD command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isFileExists(string tableName)
{
    string fileName = "../data/" + tableName + ".csv";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

/**
 * @brief Checks to see if source file exists. Called when SOURCE command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isQueryFile(string fileName){
    fileName = "../data/" + fileName + ".ra";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

bool ParsedQuery::getAggregateOperatorAndAttribute(const string &token, AggregateOperator &aggOpr, string &aggAttr)
{
	logger.log("ParsedQuery::getAggregateOperatorAndAttribute");
	if (token.length() < 6 || token[3] != '(' || token.back() != ')')
	{
		return false;
	}

	aggAttr = token.substr(4, token.length() - 5);
	const string aggregateOperatorLiteral = token.substr(0, 3);

	if (aggregateOperatorLiteral == "MIN") {
		aggOpr = MIN;
	} else if (aggregateOperatorLiteral == "MAX") {
		aggOpr = MAX;
	} else if (aggregateOperatorLiteral == "SUM") {
		aggOpr = SUM;
	} else if (aggregateOperatorLiteral == "AVG") {
		aggOpr = AVG;
	} else {
		cout << "SYNTAX ERROR" << endl;
		return false;
	}
	return true;
}

BinaryOperator ParsedQuery::getBinaryOperator(const string &token) {
	logger.log("ParsedQuery::getBinaryOperator");
    if (token == "==") {
        return EQUAL;
    } else if (token == "!=") {
        return NOT_EQUAL;
    } else if (token == ">") {
        return GREATER_THAN;
    } else if (token == "<") {
        return LESS_THAN;
    } else if (token == ">=") {
        return GEQ;
    } else if (token == "<=") {
        return LEQ;
    }
    return NO_BINOP_CLAUSE;
}