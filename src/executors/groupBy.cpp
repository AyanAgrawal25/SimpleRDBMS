#include "global.h"
#include<stdexcept>

/**
 * @brief
 * SYNTAX: <new_table> <- GROUP BY <grouping_attribute> FROM <table_name> 
 * 							HAVING <aggregate(attribute)> <bin_op> <attribute_value> RETURN MAX|MIN|SUM|AVG(<attribute>)
 */

bool syntacticParseGROUP_BY()
{
	logger.log("syntacticParseGROUP_BY");
	if (tokenizedQuery.size() != 13 || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "HAVING" ||
		tokenizedQuery[11] != "RETURN")
	{
		cout << "SYNTAX ERROR" << endl;
		return false;
	}

	parsedQuery.queryType = GROUP_BY;
	parsedQuery.groupByResultRelationName = tokenizedQuery[0];
	parsedQuery.groupByGroupingAttribute = tokenizedQuery[4];
	parsedQuery.groupByRelationName = tokenizedQuery[6];

	if (!ParsedQuery::getAggregateOperatorAndAttribute(tokenizedQuery[8], 
		parsedQuery.groupByHavingAggregateOperator, parsedQuery.groupByHavingAggregateAttribute))
	{
		cout << "SYNTAX ERROR" << endl;
		return false;
	}

	parsedQuery.groupByHavingBinaryOperator = ParsedQuery::getBinaryOperator(tokenizedQuery[9]);
	try { 
		parsedQuery.groupByHavingValue = stoi(tokenizedQuery[10]);
	} catch(std::invalid_argument const &ex) {
		cout << "SYNTAX ERROR: Invalid comparison value." << endl;
		cout << ex.what() << endl;
		return false;
	}

	if (!ParsedQuery::getAggregateOperatorAndAttribute(tokenizedQuery.back(), 
	parsedQuery.groupByAggregateOperator, parsedQuery.groupByAggregateAttribute))
	{
		cout << "SYNTAX ERROR" << endl;
		return false;
	}
	parsedQuery.groupByAggregateOperatorLiteral = tokenizedQuery.back().substr(0, 3);

	return true;
}

bool semanticParseGROUP_BY()
{
	logger.log("semanticParseGROUP_BY");

	if (tableCatalogue.isTable(parsedQuery.groupByResultRelationName))
	{
		cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
		return false;
	}

	if (!tableCatalogue.isTable(parsedQuery.groupByRelationName))
	{
		cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
		return false;
	}

	Table table = *tableCatalogue.getTable(parsedQuery.groupByRelationName);

	if (!table.isColumn(parsedQuery.groupByGroupingAttribute) || 
		!table.isColumn(parsedQuery.groupByHavingAggregateAttribute) ||
		!table.isColumn(parsedQuery.groupByAggregateAttribute))
	{
		cout << "SEMANTIC ERROR: Column doesn't exist in relation";
		return false;
	}

	return true;
}

void aggregateOperation(int &groupingKey, long long int &aggregation, const int groupingValue, const bool isNewGroupingKey,
						const int aggregateValue, const AggregateOperator aggrOperator) {

	const long long int aggrValue = (long long int)aggregateValue;
	if (isNewGroupingKey) {
		groupingKey = (groupingValue);
		aggregation = (aggrValue);
	} else {
		if (aggrOperator == AggregateOperator::SUM || aggrOperator == AggregateOperator::AVG) {
			aggregation += aggregateValue;
		} else {
			if (aggrOperator == AggregateOperator::MAX) {
				aggregation = max(aggregation, aggrValue);
			} else if (aggrOperator == AggregateOperator::MIN) {
				aggregation = min(aggregation, aggrValue);
			}
		}
	}
	return;
}

void executeGROUP_BY()
{
	logger.log("executeGROUP_BY");

	Table table = *tableCatalogue.getTable(parsedQuery.groupByRelationName);

	table.externalSort(vector<string>{parsedQuery.groupByGroupingAttribute}, vector<SortingStrategy>{SortingStrategy::ASC});

	Cursor cursor = table.getCursor();
	vector<int> row = cursor.getNext();

	const int groupingAttributeIndex = table.getColumnIndex(parsedQuery.groupByGroupingAttribute);
	const int havingAttributeIndex = table.getColumnIndex(parsedQuery.groupByHavingAggregateAttribute);
	const int aggregateAttributeIndex = table.getColumnIndex(parsedQuery.groupByAggregateAttribute);

	vector<string> columnList;
	columnList.push_back(parsedQuery.groupByGroupingAttribute);
	columnList.push_back(parsedQuery.groupByAggregateOperatorLiteral + parsedQuery.groupByAggregateAttribute);
	Table *resultantTable = new Table(parsedQuery.groupByResultRelationName, columnList);

	bool emptyTable = true;
	int groupingKey;
	long long int resultAggregation = 0, havingAggregation = 0, groupedRowsCount = 0;

	vector<vector<int>> resultantRows(resultantTable->maxRowsPerBlock, vector<int>(resultantTable->columnCount, 0));
	uint rowCounter = 0;

	function<bool (const int &, const int &)> comp = getBinaryComparator<int>(parsedQuery.groupByHavingBinaryOperator);

	while (!row.empty())
	{
		const int groupingValue = row[groupingAttributeIndex]; 
		const int havingValue = row[havingAttributeIndex];
		const int aggregateValue = row[aggregateAttributeIndex];

		const bool isNotEmptyAndNewGroupingKey = (groupedRowsCount != 0 && groupingKey != groupingValue);

		if (isNotEmptyAndNewGroupingKey) {
			if (parsedQuery.groupByAggregateOperator == AggregateOperator::AVG) {
				resultAggregation /= groupedRowsCount;
			}
			if (parsedQuery.groupByHavingAggregateOperator == AggregateOperator::AVG) {
				havingAggregation /= groupedRowsCount;
			}
			if (comp(havingAggregation, parsedQuery.groupByHavingValue)) {
				resultantRows[rowCounter++] = vector<int>{groupingKey, (int)resultAggregation};
				if (rowCounter == resultantTable->maxRowsPerBlock) {
					resultantTable->updateRowsInPage(resultantRows, rowCounter);
				}
			} 
			groupedRowsCount = 0;
			resultAggregation = 0, havingAggregation = 0, groupedRowsCount = 0;
		}

		// debug 
		// cout << "groupingValue: " << groupingValue << " havingValue: " << havingValue << " returnValue: " << aggregateValue << endl;
		const bool isNewGroupingKey = groupedRowsCount == 0;
		if (isNewGroupingKey) {
			groupedRowsCount = 1;
		} else {
			groupedRowsCount++;
		}

		aggregateOperation(groupingKey, resultAggregation, groupingValue, isNewGroupingKey, aggregateValue, parsedQuery.groupByAggregateOperator);
		aggregateOperation(groupingKey, havingAggregation, groupingValue, isNewGroupingKey, havingValue, parsedQuery.groupByHavingAggregateOperator);		


		row = cursor.getNext();
	}

	const bool isNotEmpty = groupedRowsCount != 0;

	if (isNotEmpty) {
		if (parsedQuery.groupByAggregateOperator == AggregateOperator::AVG) {
			resultAggregation /= groupedRowsCount;
		}
		if (parsedQuery.groupByHavingAggregateOperator == AggregateOperator::AVG) {
			havingAggregation /= groupedRowsCount;
		}
		if (comp(havingAggregation, parsedQuery.groupByHavingValue)) {
			resultantRows[rowCounter++] = vector<int>{groupingKey, (int)resultAggregation};
		} 
		if (rowCounter)
			resultantTable->updateRowsInPage(resultantRows, rowCounter);
	}

	if (resultantTable->rowCount) {
		tableCatalogue.insertTable(resultantTable);
	} else {
		resultantTable->unload();
		delete resultantTable;
		cout << "Empty Table" << endl;
	}

    table.columns.clear();
    table.columnCount = 0;
    table.rowCount = 0;
    table.blockCount = 0;
    table.maxRowsPerBlock = 0;
    table.rowsPerBlockCount.clear();
	// table.unload();
	table.load();

	return;
}