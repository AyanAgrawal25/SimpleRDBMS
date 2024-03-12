#include "cursor.h"
#include "syntacticParser.h"

enum IndexingStrategy
{
    BTREE,
    HASH,
    NOTHING
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

/**
 * @brief The Table class holds all information related to a loaded table. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a table object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT). 
 *
 */
class Table
{
    vector<unordered_set<int>> distinctValuesInColumns;

public:
    string sourceFileName = "";
    string tableName = "";
    vector<string> columns;
    vector<uint> distinctValuesPerColumnCount;
    uint columnCount = 0;
    long long int rowCount = 0;
    uint blockCount = 0;
    uint maxRowsPerBlock = 0;
    vector<uint> rowsPerBlockCount;
    bool indexed = false;
    string indexedColumn = "";
    IndexingStrategy indexingStrategy = NOTHING;
    
    bool extractColumnNames(string firstLine);
    bool blockify();
    void updateStatistics(vector<int> row);
    Table();
    Table(string tableName);
    Table(string tableName, vector<string> columns);
    Table(const Table &table, string resultantTableName);
    bool load();
    bool isColumn(string columnName);
    void renameColumn(string fromColumnName, string toColumnName);
    void print();
    void makePermanent();
    void updateRowsInPage(vector<vector<int>> &rows, uint &rowCounter);
    bool isPermanent();
    bool getNextPage(Cursor *cursor);
    Cursor getCursor();
    int getColumnIndex(string columnName);
    void unload();

    /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row, ostream &fout)
{
    logger.log("Table::writeRow<T>");
    for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
    {
        if (columnCounter != 0)
            fout << ", ";
        fout << row[columnCounter];
    }
    fout << endl;
}

/**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row)
{
    logger.log("Table::printRow");
    ofstream fout(this->sourceFileName, ios::app);
    this->writeRow(row, fout);
    fout.close();
}

template<typename _Tp>
class SortComparator {
protected:
    const vector<uint> sortColumnIndices;
    const vector<SortingStrategy> sortStrategies;
    const size_t compareColumnCount;
public:
    SortComparator(const std::vector<uint> &indices, const std::vector<SortingStrategy> strategies) : 
        sortColumnIndices(indices), sortStrategies(strategies), compareColumnCount(indices.size()) {}

    SortComparator(const SortComparator<_Tp> &sortComparator) : 
        sortColumnIndices(sortComparator.sortColumnIndices), sortStrategies(sortComparator.sortStrategies), 
        compareColumnCount(sortComparator.compareColumnCount) {}

    virtual bool operator()(const vector<_Tp> &a, const vector<_Tp> &b) const {
        for (uint i = 0u; i < compareColumnCount; ++i) {
            const uint idx = sortColumnIndices[i];
            const SortingStrategy &sortingStrategy = sortStrategies[i]; 
            if (a[idx] != b[idx]) {
                if (sortingStrategy == SortingStrategy::ASC) {
                    return (a[idx] < b[idx]);
                } else if(sortingStrategy == SortingStrategy::DESC) {
                    return (a[idx] > b[idx]);
                }
            }
        }
        return false;
    }
};

template<typename _Tp>
class HeapComparator : public SortComparator<_Tp> {
public:
using tuple_row_cursor = tuple<vector<_Tp>, int, int>;

    HeapComparator(const std::vector<uint> &indices, const std::vector<SortingStrategy> strategies) :
        SortComparator<_Tp>(indices, strategies) {}
    
    HeapComparator(const SortComparator<_Tp> &sortComparator) : 
        SortComparator<_Tp>(sortComparator) {}

    bool operator()(const tuple_row_cursor &a, const tuple_row_cursor &b) const {
        for (uint i = 0u; i < this->compareColumnCount; ++i) {
            const uint idx = SortComparator<_Tp>::sortColumnIndices[i];
            const SortingStrategy &sortingStrategy = SortComparator<_Tp>::sortStrategies[i]; 
            const vector<_Tp> &aVec = get<0>(a), &bVec = get<0>(b);
            if (aVec[idx] != bVec[idx]) {
                if (sortingStrategy == SortingStrategy::ASC) {
                    return (aVec[idx] > bVec[idx]);
                } else if(sortingStrategy == SortingStrategy::DESC) {
                    return (aVec[idx] < bVec[idx]);
                }
            }
        }
        return false;
    }
};

    void externalSort(const vector<string> &sortColumnNames, const vector<SortingStrategy> &sortingStrategies);
    template<typename _Tp>
    void executeSORT_sortingPhase(Table*, const SortComparator<_Tp>&, uint);
    template<typename _Tp>
    void executeSORT_mergingPhase(Table*, const HeapComparator<_Tp>&, uint);

    Table* getTableOrderBy(string newTableName, string columnName, SortingStrategy sortingStrategy);
    static Table* sortJoin(string tableName, Table *table1, Table *table2, string columnName1, string columnName2, int binaryOperator);

};