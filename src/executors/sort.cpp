#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");
    if(tokenizedQuery.size() < 6 || tokenizedQuery[2] != "BY") {
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = SORT;
    parsedQuery.sortRelationName = tokenizedQuery[1];
    vector<string> columnNames;
    const int tokenizedQueryLength = (int)tokenizedQuery.size();
    int idx = 3;
    for (idx = 3; idx < tokenizedQueryLength && tokenizedQuery[idx] != "IN"; idx++) {
        columnNames.emplace_back(tokenizedQuery[idx]);
    }
    if (idx == tokenizedQueryLength) {
        cout << "SYNTAX ERROR: Sorting Strategies not found." << endl;
        return false;
    }
    parsedQuery.sortColumnNames = columnNames;
    const int nc = columnNames.size();
    idx = idx + 1; // skip "IN"
    vector<SortingStrategy> sortingStrategies;
    for (; idx < tokenizedQueryLength; ++idx) {
        const string &sortingStrategyString = tokenizedQuery[idx];
        SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
        if(sortingStrategyString == "ASC")
            sortingStrategy = ASC;
        else if(sortingStrategyString == "DESC")
            sortingStrategy = DESC;
        else{
            cout<<"SYNTAX ERROR: Invalid Sorting Strategy."<<endl;
            return false;
        }
        sortingStrategies.emplace_back(sortingStrategy);
    }
    if ((int)sortingStrategies.size() != nc) {
        cout<<"SYNTAX ERROR: Size of column names list is not equal to size of sorting strategies."<<endl;
        return false;
    }
    parsedQuery.sortingStrategies = sortingStrategies;
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
        cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
        return false;
    }

    for (string &sortColumnName : parsedQuery.sortColumnNames) {
        if(!tableCatalogue.isColumnFromTable(sortColumnName, parsedQuery.sortRelationName)){
            cout<<"SEMANTIC ERROR: Column \"" << sortColumnName << "\" doesn't exist in relation " << parsedQuery.sortRelationName << "." << endl;
            return false;
        }
    }
    return true;
}

void executeSORT(){
    logger.log("executeSORT");
    
    Table *table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    table->externalSort(parsedQuery.sortColumnNames, parsedQuery.sortingStrategies);
    return;
}