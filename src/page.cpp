#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->tableName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName 
 * @param pageIndex 
 */
Page::Page(string tableName, int pageIndex)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
    Table table = *tableCatalogue.getTable(tableName);
    this->columnCount = table.columnCount;
    uint maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);

    ifstream fin(pageName, ios::in);
    this->rowCount = table.rowsPerBlockCount[pageIndex];
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName 
 * @param pageIndex 
 */
Page::Page(string tableName, uint runsThisPass, int pageIndex)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_nRuns-" + to_string(runsThisPass) + + "_Page" + to_string(pageIndex);
    this->runsThisPass = runsThisPass;
    Table table = *tableCatalogue.getTable(tableName);
    this->columnCount = table.columnCount;
    uint maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);

    ifstream fin(pageName, ios::in);
    this->rowCount = table.rowsPerBlockCount[pageIndex];
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}


/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> Page::getRow(int rowIndex)
{
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}

Page::Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
}

Page::Page(string tableName, uint runsThisPass, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->runsThisPass = runsThisPass;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->pageName = "../data/temp/"+this->tableName + "_nRuns-" + to_string(runsThisPass) + "_Page" + to_string(pageIndex);
    logger.log("pageName: " + this->pageName);
}

/**
 * @brief writes current page contents to file.
 * 
 */
void Page::writePage()
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0) 
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
    bufferManager.updatePageInQueue(this);
}

/**
 * @brief Construct a new MatrixPage object. Never used as part of the code
 *
 */
MatrixPage::MatrixPage() : Page()
{
    this->matrixName = "";
}

/**
 * @brief Construct a new MatrixPage:: MatrixPage object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<matrixname>_<pageindex>". For example, If the MatrixPage being loaded is of
 * matrix "M" and the pageIndex is 2 then the file name is "M_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param matrixName 
 * @param pageIndex 
 */
MatrixPage::MatrixPage(string matrixName, int pageIndex)
{
    logger.log("MatrixPage::MatrixPage");
    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(pageIndex);
    Matrix matrix = *matrixCatalogue.getMatrix(matrixName);
    this->columnCount = matrix.columnsPerBlockCount[pageIndex];
    this->rowCount = matrix.rowsPerBlockCount[pageIndex];
    vector<int> row(columnCount, 0);
    this->rows.assign(rowCount, row);
}

MatrixPage::MatrixPage(string matrixName, int pageIndex, vector<vector<int>> &rows)
{
    logger.log("MatrixPage::MatrixPage");
    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(pageIndex);
    Matrix matrix = *matrixCatalogue.getMatrix(matrixName);
    this->columnCount = matrix.columnsPerBlockCount[pageIndex];
    this->rowCount = matrix.rowsPerBlockCount[pageIndex];
    this->rows = rows;
}


void MatrixPage::load() {
    ifstream fin(this->pageName, ios::in);
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}

uint MatrixPage::getRowCount() {
    return this->rowCount;
}

uint MatrixPage::getColumnCount() {
    return this->columnCount;
}

/**
 * @brief Function transposes rows and columns of a single matrix page
 *
 */
void MatrixPage::transpose()
{
    logger.log("MatrixPage::transpose");
    for (int i = 0; i < this->rowCount; i++)
    {
        for (int j = i + 1; j < this->columnCount; j++)
        {
            swap(this->rows[i][j], this->rows[j][i]);
        }
    }
}

void MatrixPage::transpose(MatrixPage *page) {
    logger.log("MatrixPage::transpose");
    for (int i = 0; i < this->rowCount; i++) {
        for (int j = 0; j < this->columnCount; j++) {
            swap(page->rows[j][i], this->rows[i][j]);
        }
    }
}

bool MatrixPage::isSymmetric() {
    logger.log("MatrixPage::isSymmetric");
    bool isSymmetric = true;
    for (int i = 0; i < this->rowCount; i++)
    {
        for (int j = 0; j < this->columnCount; j++)
        {
            isSymmetric = isSymmetric && (this->rows[i][j] == this->rows[j][i]);
            if (!isSymmetric)
                return false;
        }
        if (!isSymmetric) {
            return false;
        }
    }  
    return true;
} 

bool MatrixPage::isSymmetric(MatrixPage *page) {
    logger.log("MatrixPage::isSymmetric");
    bool isSymmetric = true;
    for (int i = 0; i < this->rowCount; i++)
    {
        for (int j = i + 1; j < this->columnCount; j++)
        {
            isSymmetric = isSymmetric && (this->rows[i][j] == page->rows[j][i]);
            if (!isSymmetric)
                return false;
        }
        if (!isSymmetric) {
            return false;
        }
    }  
    return true;
} 

/**
 * @brief 
 * 
 */
void MatrixPage::compute()
{
    logger.log("MatrixPage::compute");
    for (int i = 0; i < this->rowCount; i++)
    {
        for (int j = i; j < this->columnCount; j++)
        {
            this->rows[i][j] = this->rows[i][j] - this->rows[j][i];
            this->rows[j][i] = -this->rows[i][j];
        }
    }
}

void MatrixPage::compute(MatrixPage *page)
{
    logger.log("MatrixPage::compute");
    // MatrixPage resultMatrix(this->matrixName + "_Result", this->rowCount, this->columnCount);
    for (int i = 0; i < this->rowCount; i++)
    {
        for (int j = 0; j < this->columnCount; j++)
        {
            page->rows[j][i] -= this->rows[i][j];
            this->rows[i][j] = -page->rows[j][i];
        }
    }
}

MatrixPage* MatrixPage::createPageForCompute(MatrixPage *page) {
    MatrixPage *matrixPage = new MatrixPage(this->matrixName + "_RESULT", page->pageIndex, this->rows);
    return matrixPage;
}

/**
 * @brief writes current page contents to file.
 * 
 */
void MatrixPage::writePage()
{
    logger.log("MatrixPage::writePage");
    if (trackBlocks)
        blocksWritten++;
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}

/**
 * @brief 
 * 
 * @param pageName 
 */

void MatrixPage::writePage(string pageName)
{
    logger.log("MatrixPage::writePage");
    if (trackBlocks)
        blocksWritten++;
    ofstream fout(pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}
