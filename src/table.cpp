#include "global.h"

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName 
 * @param columns 
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
    // this->writeRow<string>(columns);
}

Table::Table(const Table &table, string resultantTableName) {
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + resultantTableName + ".csv";
    this->tableName = resultantTableName;
    this->columns = table.columns;
    this->columnCount = table.columnCount;
    this->maxRowsPerBlock = table.maxRowsPerBlock;
    this->blockCount = table.blockCount;
    this->rowsPerBlockCount = table.rowsPerBlockCount;
    this->indexed = table.indexed;
    this->indexedColumn = table.indexedColumn;
    this->indexingStrategy = table.indexingStrategy;
    this->rowCount = table.rowCount;
    // copy all page files
    for (int pageCounter = 0; pageCounter < table.blockCount; pageCounter++)
    {
        const string srcFileName = "../data/temp/"+ table.tableName + "_Page" + to_string(pageCounter);
        const string destFileName = "../data/temp/" + resultantTableName + "_Page" + to_string(pageCounter);
        bufferManager.copyFile(srcFileName, destFileName);
    }
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    this->columns.clear();
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    this->rowCount = 0;
    getline(fin, line);
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName 
 * @return true 
 * @return false 
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}



/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
bool Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNextPage");
    if (cursor->getPageIndex() < (int)(this->blockCount) - 1)
    {
        cursor->nextPage(cursor->getPageIndex()+1);
        return true;
    }
    return false;
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload(){
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 * 
 * @return Cursor 
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 * 
 * @param columnName 
 * @return int 
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
    return -1;
}

void Table::externalSort(const vector<string> &sortColumnNames, const vector<SortingStrategy> &sortingStrategies) {
    logger.log("Table::externalSort");
    const int nCols = sortColumnNames.size();

    vector<uint> sortingColumnIndices(nCols);
    for (int i = 0; i < nCols; ++i) {
        sortingColumnIndices[i] = (uint)this->getColumnIndex(sortColumnNames[i]);
    }

    const int nb = SORT_BUFFER_SIZE - 1;
    uint nr = (uint)ceil((double)this->blockCount / (double)nb);

    const SortComparator<int> sortComparator(sortingColumnIndices, sortingStrategies);
    const HeapComparator<int> heapComparator(sortComparator);

    executeSORT_sortingPhase(this, sortComparator, nr);
    executeSORT_mergingPhase(this, heapComparator, nr);
    return;
}



template<typename _Tp>
void Table::executeSORT_sortingPhase(Table *table, const SortComparator<_Tp> &sortComparator, uint nr) {
    logger.log("Table::executeSORT_sortingPhase");

    Cursor cursor = table->getCursor();

    const int nb = SORT_BUFFER_SIZE - 1;
    const int maxRowsPerBlock = table->maxRowsPerBlock;

    vector<vector<_Tp>> currentPageSortedRows(maxRowsPerBlock, vector<_Tp>(table->columnCount));

    vector<vector<_Tp>> rows;
    rows.reserve((nb + 1) * maxRowsPerBlock);
    vector<_Tp> row;
    vector<uint> rowsPerBlockCount;
    
    uint blockCount = 0;

    while (blockCount < table->blockCount) {
        rows.clear();

        for (int i = blockCount; i < min(blockCount + nb, table->blockCount); i++)
        {
            vector<_Tp> row;
            while (!(row  = cursor.getNextOfCurrentPage()).empty()) {
                rows.emplace_back(row);
            }
            const bool isNextBlock = table->getNextPage(&cursor);
            if (!isNextBlock)
                break;
        }
        
        try {
            sort(rows.begin(), rows.end(), sortComparator);
        } catch (const std::exception& e) {
            logger.log(string{"While sorting rows in Sorting Phase -> "} + string{e.what()});
        }

        const uint sortedRowsSize = rows.size(); 

        for (uint rowCounter = 0; rowCounter < sortedRowsSize; rowCounter += maxRowsPerBlock) {
            const typename vector<vector<_Tp>>::iterator first = rows.begin() + rowCounter;
            const typename vector<vector<_Tp>>::iterator last = (rows.begin() + min(sortedRowsSize, rowCounter + maxRowsPerBlock));
            std::copy(first, last, currentPageSortedRows.begin());
            const uint rowCount = std::distance(first, last);
            rowsPerBlockCount.emplace_back(rowCount);
            bufferManager.writePage(table->tableName, nr, blockCount++, currentPageSortedRows, rowCount);
        }
    }
    table->blockCount = blockCount;
    table->rowsPerBlockCount = rowsPerBlockCount;
    return;
}

template<typename _Tp>
void Table::executeSORT_mergingPhase(Table *table, const HeapComparator<_Tp> &heapComparator, uint nr) {
    logger.log("Table::executeSORT_mergingPhase");

    Cursor cursor = table->getCursor();
  
    const int nb = SORT_BUFFER_SIZE - 1;
    uint pnr = nb;

    const uint maxRowsPerBlock = table->maxRowsPerBlock;
    const uint blockCount = table->blockCount;

    vector<vector<_Tp>> mergedRows(maxRowsPerBlock, vector<_Tp>(table->columnCount));
    uint rowCounter = 0u;
    vector<_Tp> row;

    while (nr != 1)
    {
        uint blockCounter = 0u;
        vector<uint> rowsPerBlockCount;
        uint next_nr = ceil((double)nr / (double)nb);
        
        Cursor *cursor;
        for (uint i = 0u; i < nr; i += nb)
        {
            using queueType = typename HeapComparator<_Tp>::tuple_row_cursor;
            priority_queue<queueType, vector<queueType>, HeapComparator<_Tp>> pq(heapComparator);

            for (uint j = i; j < min(nr, i + nb); j++)
            {
                cursor = new Cursor(table->tableName, nr, j * pnr);
                row = cursor->getNextOfCurrentPage();
                pq.emplace(row, cursor->getPageIndex(), cursor->getPagePointer());
                delete cursor;
            }

            int pageIndex, pagePtr;
            while (!pq.empty())
            {
                tie(row, pageIndex, pagePtr) = pq.top();
                pq.pop();
                cursor = new Cursor(table->tableName, nr, pageIndex);
                cursor->goToRowOfCurrentPage(pagePtr);

                mergedRows[rowCounter++] = row;
                if (rowCounter == maxRowsPerBlock)
                {
                    bufferManager.writePage(table->tableName, next_nr, blockCounter++, mergedRows, rowCounter);
                    rowsPerBlockCount.emplace_back(rowCounter);
                    rowCounter = 0;
                }
                row = cursor->getNextOfCurrentPage();
                if (row.empty() && (cursor->getPageIndex() + 1) % pnr != 0
                    && (cursor->getPageIndex() + 1) != blockCount)
                {
                    cursor->nextPage(cursor->getPageIndex() + 1);
                    row = cursor->getNextOfCurrentPage();
                }
                if (!row.empty())
                {
                    pq.emplace(row, cursor->getPageIndex(), cursor->getPagePointer());
                }
                delete cursor;
            }
            if (rowCounter)
            {
                bufferManager.writePage(table->tableName, next_nr, blockCounter++, mergedRows, rowCounter);
                rowsPerBlockCount.emplace_back(rowCounter);
                rowCounter = 0;
            }
        }

        for (int pageCounter = 0; pageCounter < blockCount; pageCounter++)
        {
            bufferManager.deleteFile(table->tableName, nr, pageCounter);
        }
        table->rowsPerBlockCount = rowsPerBlockCount;
        nr = next_nr;
        pnr = pnr * nb;
    }
}

void Table::updateRowsInPage(vector<vector<int>> &rows, uint &rowCounter) {
    logger.log("Table::updateRowsInPage");
    bufferManager.writePage(this->tableName, this->blockCount, rows, (int)rowCounter);
    this->rowCount += rowCounter;
    this->blockCount++;
    this->rowsPerBlockCount.emplace_back(rowCounter);
    rowCounter = 0;
    return;
}

Table* Table::getTableOrderBy(string newTableName, string columnName, SortingStrategy sortingStrategy) {
    logger.log("Table::getTableOrderBy");
    Table *resultantTable = new Table(*this, newTableName);
	tableCatalogue.insertTable(resultantTable);
    const vector<string> sortColumnNames ({columnName});
    const vector<SortingStrategy> sortingStrategies ({sortingStrategy});
    resultantTable->externalSort(sortColumnNames, sortingStrategies);
    return resultantTable;
}

Table* Table::sortJoin(string tableName, Table *table1, Table *table2, string columnName1, string columnName2, int binaryOperator) {
    logger.log("Table::sortJoin");
    table1 = table1->getTableOrderBy(table1->tableName + "_sorted_by_" + columnName1, columnName1, SortingStrategy::ASC);
    table2 = table2->getTableOrderBy(table2->tableName + "_sorted_by_" + columnName2, columnName2, SortingStrategy::ASC);
    
    vector<string> columns; columns.reserve(table1->columnCount + table2->columnCount);
    columns.insert(columns.end(), table1->columns.begin(), table1->columns.end());
    columns.insert(columns.end(), table2->columns.begin(), table2->columns.end());
    Table *resultantTable = new Table(tableName, columns);

    Cursor cursor1 = table1->getCursor();

    const int columnIndex1 = table1->getColumnIndex(columnName1);
    const int columnIndex2 = table2->getColumnIndex(columnName2);

    vector<vector<int>> resultantRows(resultantTable->maxRowsPerBlock, vector<int>(resultantTable->columnCount, 0));
    uint rowCounter = 0;

    function<bool (const int &, const int &)> comp = getBinaryComparator<int>(BinaryOperator(binaryOperator));

    for (vector<int> row1 = cursor1.getNext(); !row1.empty(); row1 = cursor1.getNext()) {
        Cursor cursor2 = table2->getCursor();
        for (vector<int> row2 = cursor2.getNext(); !row2.empty(); row2 = cursor2.getNext()) {
            if (comp(row1[columnIndex1], row2[columnIndex2])) {
                std::copy(row1.begin(), row1.end(), resultantRows[rowCounter].begin());
                std::copy(row2.begin(), row2.end(), resultantRows[rowCounter].begin() + table1->columnCount);
                rowCounter++;
                if (rowCounter == resultantTable->maxRowsPerBlock) {
                    resultantTable->updateRowsInPage(resultantRows, rowCounter);
                }
            }
        }
    }
    if (rowCounter) {
        resultantTable->updateRowsInPage(resultantRows, rowCounter);
    }
    tableCatalogue.deleteTable(table1->tableName);
    tableCatalogue.deleteTable(table2->tableName);
    return resultantTable;
}