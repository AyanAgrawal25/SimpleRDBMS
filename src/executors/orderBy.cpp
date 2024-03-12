#include "global.h"

/**
 * @brief
 * SYNTAX: <new_table> <- ORDER BY <ordering_attribute> ASC|DESC ON <table_name>
 */

bool syntacticParseORDER_BY()
{
	logger.log("syntacticParseORDER_BY");
	if (tokenizedQuery.size() != 8 || tokenizedQuery[3] != "BY" ||  
    tokenizedQuery[6] != "ON" || (tokenizedQuery[5] != "ASC" && tokenizedQuery[5] != "DESC"))
	{
		cout << "SYNTAX ERROR" << endl;
		return false;
	}

	parsedQuery.queryType = ORDER_BY;
	parsedQuery.orderByResultRelationName = tokenizedQuery[0];
	parsedQuery.orderByColumnName = tokenizedQuery[4];
    if (tokenizedQuery[5] == "ASC") {
        parsedQuery.orderBySortingStrategy = ASC;
    } else if (tokenizedQuery[5] == "DESC") {
        parsedQuery.orderBySortingStrategy = DESC;
    } else {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
	parsedQuery.orderByRelationName = tokenizedQuery[7];
	return true;
}

bool semanticParseORDER_BY()
{
	logger.log("semanticParseORDER_BY");

	if (tableCatalogue.isTable(parsedQuery.orderByResultRelationName))
	{
		cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
		return false;
	}

	if (!tableCatalogue.isTable(parsedQuery.orderByRelationName))
	{
		cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
		return false;
	}

	Table table = *tableCatalogue.getTable(parsedQuery.orderByRelationName);

	if (!table.isColumn(parsedQuery.orderByColumnName))
	{
		cout << "SEMANTIC ERROR: Column doesn't exist in relation";
		return false;
	}

	return true;
}

void executeORDER_BY()
{
	logger.log("executeORDER_BY");
    Table *table = tableCatalogue.getTable(parsedQuery.orderByRelationName);
	Table *resultantTable = table->getTableOrderBy(parsedQuery.orderByResultRelationName, parsedQuery.orderByColumnName, parsedQuery.orderBySortingStrategy);
	tableCatalogue.insertTable(resultantTable);
	return;
}