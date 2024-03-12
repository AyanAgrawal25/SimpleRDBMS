#include "global.h"

/**
 * @brief Construct a new Matrix:: Matrix object
 *
 */
Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD MATRIX command has been called. This command should be
 * followed by calling the load function;
 *
 * @param matrixName 
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}

void Matrix::insertMatrixForCompute()
{
    logger.log("Matrix::insertMatrixForCompute");
    Matrix *matrix = new Matrix(this->matrixName + "_RESULT");
    matrix->rowCount = this->rowCount;
    matrix->columnCount = this->columnCount;
    matrix->maxDimPerBlock = this->maxDimPerBlock;
    matrix->blocksPerRow = this->blocksPerRow;
    matrix->blockCount = this->blockCount;
    matrix->rowsPerBlockCount = this->rowsPerBlockCount;
    matrix->columnsPerBlockCount = this->columnsPerBlockCount;
    matrixCatalogue.insertMatrix(matrix);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates matrix
 * statistics.
 *
 * @return true if the matrix has been successfully loaded 
 * @return false if an error occurred 
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    if (this->calculateDimensions() && 
        this->blockify()) {
        return true;
    }
    return false;
}

/**
 * @brief Function calculates implementation specific dimensions from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if successfully calculated
 * @return false otherwise
 */
bool Matrix::calculateDimensions()
{
    logger.log("Matrix::calculateDimensions");
    ifstream fin(this->sourceFileName, ios::in);
    string firstLine, word;
    if (!getline(fin, firstLine)) {
        return false;
    }
    stringstream ss(firstLine);
    while (getline(ss, word, ','))
    {
        ++this->columnCount;
    }
    this->rowCount = this->columnCount;
    this->maxDimPerBlock = (uint)sqrt((BLOCK_SIZE * 1024) / sizeof(int));
    this->blocksPerRow = (uint)ceil((double)this->columnCount / (double)this->maxDimPerBlock);
    this->blockCount = this->blocksPerRow * this->blocksPerRow;
    this->rowsPerBlockCount.assign(this->blockCount, 0);
    this->columnsPerBlockCount.assign(this->blockCount, 0);
    fin.close();
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");

    ifstream fin(this->sourceFileName, ios::in);

    for (uint row = 0U; row < this->rowCount; ++row) {
        for (uint block = 0U; block < this->blocksPerRow; ++block) {
            int columnsThisBlock = this->maxDimPerBlock;
            const bool isLastBlockInThisRow = this->isLastBlockInRow(block);
            if (isLastBlockInThisRow) {
                columnsThisBlock = this->columnCount % this->maxDimPerBlock;
            }
            const uint pageIndex = (row / this->maxDimPerBlock) * this->blocksPerRow + block;
            if (this->columnsPerBlockCount[pageIndex] == 0) {
                this->columnsPerBlockCount[pageIndex] = columnsThisBlock;
            }
            vector<int> rowInBlock(columnsThisBlock, 0); 
            if (!this->readRowInBlock(fin, columnsThisBlock, isLastBlockInThisRow, rowInBlock)) {
                return false;
            }
            this->writeRowInPage(pageIndex, rowInBlock);
            this->rowsPerBlockCount[pageIndex] += 1;
        }
    }

    fin.close();
    return true;
}

/**
 * @brief Function to check if the block is the last block in the row
 * 
 * @param bool indicating if the block is the last block in the row
*/
bool Matrix::isLastBlockInRow(uint block) {
    logger.log("Matrix::isLastBlockInRow");
    return (block == this->blocksPerRow - 1);
}

/**
 * @brief Function to read the row from the file for a particular block
 * 
 * @return true if successfully read
*/
bool Matrix::readRowInBlock(ifstream &fin, const uint columnsThisBlock, const bool isLastBlock, vector<int> &rowInBlock) {
    logger.log("Matrix::readRowInBlock");
    string word;
    for (int columnCounter = 0; columnCounter < columnsThisBlock; columnCounter++)
    {
        const char delim = (columnCounter == columnsThisBlock - 1 && isLastBlock) ? '\n' : ',';
        if (!getline(fin, word, delim)) 
            return false;
        rowInBlock[columnCounter] = stoi(word);
    }
    return true;
}

/**
 * @brief Function to write a row in a page
 * @param pageIndex The index of tha page to write to
 * @param rowInBlock The row to write
*/
void Matrix::writeRowInPage(uint pageIndex, const vector<int> &rowInBlock) {
    logger.log("Matrix::writeRowInPage");
    string pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(pageIndex);
    ofstream fout(pageName, ios::app);
    for (uint columnCounter = 0; columnCounter < rowInBlock.size(); columnCounter++)
    {
        if (columnCounter != 0)
            fout << " ";
        fout << rowInBlock[columnCounter];
    }
    fout << endl;
    fout.close();
}

/**
 * @brief Function prints the first few rows and columns of the matrix. If the matrix contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed. Same for columns.
 *
 */
void Matrix::print()
{
    logger.log("Matrix::print");
    uint count = min(PRINT_COUNT, this->rowCount);

    MatrixCursor cursor(this->matrixName, 0);
    for (uint rowCounter = 0; rowCounter < count; rowCounter++)
    {
        uint remaining = count, block = 0;
        bool last = false;

        for (block = 0; remaining != 0 && block < this->blocksPerRow; block++)
        {
            vector<int> segmentInPage = cursor.getNext();
            if (segmentInPage.size() >= remaining)
            {
                segmentInPage.resize(remaining);
                last = true;
            }

            remaining -= segmentInPage.size();
            const bool firstColumn = block == 0;
            this->writeRow(segmentInPage, cout, firstColumn, last);
        }
        const bool isLastRowPrinted = (rowCounter == count - 1);
        const bool reachedlastBlock = (block == this->blocksPerRow);
        if (!isLastRowPrinted && !reachedlastBlock)
        {
            const bool shiftToNextPageRow = (cursor.getPagePointer() == this->rowsPerBlockCount[cursor.getPageIndex()] - 1); 
            const int nextPageIndex = cursor.getPageIndex() - cursor.getPageIndex() % this->blocksPerRow + (shiftToNextPageRow * this->blocksPerRow);
            const int nextPagePointer = (shiftToNextPageRow ? 0 : cursor.getPagePointer() + 1);
            cursor.nextPage(nextPageIndex, nextPagePointer);
        }

    }
    printMatrixRowAndColumnCount(this->rowCount, this->columnCount);
}

/**
 * @brief This function moves to the next page of the matrix in the cursor object.
 *
 * @param cursor The cursor object to be moved to the next page
 */
void Matrix::getNextPage(MatrixCursor *cursor)
{
    logger.log("Table::getNextPage");
    if (cursor->getPageIndex() < this->blockCount - 1)
    {
        cursor->nextPage(cursor->getPageIndex() + 1);
    }
}

/**
 * @brief This function moves to the next pointer of the matrix in the cursor object.
 *
 * @param cursor The cursor object to be moved to the next pointer
 */
void Matrix::getNextPointer(MatrixCursor *cursor)
{
    logger.log("Table::getNextPointer");
    bool isLastPageInRow = (cursor->getPageIndex() + 1) % this->blocksPerRow == 0;
    bool isLastRowPointerInPage = cursor->getPagePointer() == cursor->getPageRowCount() - 1;
    int nextPageIndex, nextPagePointer;
    if (isLastPageInRow)
    {
        if (isLastRowPointerInPage) {
            nextPageIndex = cursor->getPageIndex() + 1;
            nextPagePointer = 0;
        }
        else {
            nextPageIndex = cursor->getPageIndex() + 1 - this->blocksPerRow; 
            nextPagePointer = cursor->getPagePointer() + 1;
        }
    } else {
        nextPageIndex = cursor->getPageIndex() + 1;
        nextPagePointer = cursor->getPagePointer();
    }
    if (nextPageIndex < this->blockCount - 1)
    {
        cursor->nextPage(nextPageIndex, nextPagePointer);
    }
}

// /**
//  * @brief called when EXPORT command is invoked to move source file to "data"
//  * folder.
//  *
//  */
// void Matrix::makePermanent()
// {
//     logger.log("Matrix::makePermanent");
//     if(!this->isPermanent())
//         bufferManager.deleteFile(this->sourceFileName);
//     string newSourceFile = "../data/" + this->matrixName + ".csv";
//     ofstream fout(newSourceFile, ios::out);

//     //print headings
//     this->writeRow(this->columns, fout);

//     Cursor cursor(this->matrixName, 0);
//     vector<int> row;
//     for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
//     {
//         row = cursor.getNext();
//         this->writeRow(row, fout);
//     }
//     fout.close();
// }

/**
 * @brief Function to check if matrix is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isExported()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

void Matrix::makeExportation(){
    logger.log("Matrix::makeExportation");
    if(!this->isExported()){
        bufferManager.deleteFile(this->sourceFileName);
    }
    string newSourceFileName = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFileName, ios::out);

    MatrixCursor cursor(this->matrixName, 0);
    for (int rowCounter = 0; rowCounter < this->rowCount; ++rowCounter)
    {
        for (int segNum = 0; segNum < this->blocksPerRow; ++segNum){
            bool isFirstSegment = (segNum == 0);
            bool isLastSegment = (segNum == this->blocksPerRow - 1);
            this->writeRow(cursor.getNext(), fout, isFirstSegment, isLastSegment);
        }
    }
    fout.close();
}

/**
 * @brief The unload function removes the matrix from the database by deleting
 * all temporary files created as part of this matrix
 *
 */
void Matrix::unload(){
    logger.log("Matrix::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->matrixName, pageCounter);
    if (!isExported())
        bufferManager.deleteFile(this->sourceFileName);

}

/**
 * @brief 
 * 
 */
void Matrix::transpose()
{
    logger.log("Matrix::Transpose");
    function<MatrixPage*(Page*)> castToMatrixPage = [](Page* page) -> MatrixPage* { return dynamic_cast<MatrixPage*>(page); };
    blocksRead = 0; blocksWritten = 0;
    trackBlocks = true;
    for (uint block_x = 0; block_x < this->blocksPerRow; ++block_x){
        for (uint block_y = block_x; block_y < this->blocksPerRow; ++block_y){
            if(block_x == block_y){
                int block_xy = this->blocksPerRow * block_x + block_y;
                MatrixPage *page_xy = castToMatrixPage(bufferManager.getPage(this->matrixName, block_xy));
                page_xy->transpose();
                page_xy->writePage();
            }
            else
            {
                int block_xy = this->blocksPerRow * block_x + block_y;
                int block_yx = this->blocksPerRow * block_y + block_x;
                MatrixPage *page_xy = castToMatrixPage(bufferManager.getPage(this->matrixName, block_xy));
                MatrixPage *page_yx = castToMatrixPage(bufferManager.getPage(this->matrixName, block_yx));
                page_xy->transpose(page_yx);
                page_xy->writePage();
                page_yx->writePage();
            }
        }
    }
    printBlockInformation();
    trackBlocks = false;
    blocksRead = 0; blocksWritten = 0;
}

/**
 * @brief 
 * 
 */
void Matrix::compute()
{
    logger.log("Matrix::Compute");
    function<MatrixPage *(Page *)> castToMatrixPage = [](Page *page) -> MatrixPage *
    { return dynamic_cast<MatrixPage *>(page); };
    const string newMatrixName = this->matrixName + "_RESULT";
    this->insertMatrixForCompute();
    
    for (uint block_x = 0; block_x < this->blocksPerRow; ++block_x)
    {
        for (uint block_y = block_x; block_y < this->blocksPerRow; ++block_y)
        {
            if (block_x == block_y)
            {
                int block_xy = this->blocksPerRow * block_x + block_y;
                MatrixPage *page_xy = castToMatrixPage(bufferManager.getPage(this->matrixName, block_xy));
                MatrixPage *page_xy_compute = page_xy->createPageForCompute(page_xy);
                bufferManager.insertMatrixPageIntoPool(page_xy_compute);
                page_xy_compute->compute();
                page_xy_compute->writePage();
            } else {

                int block_xy = this->blocksPerRow * block_x + block_y;
                int block_yx = this->blocksPerRow * block_y + block_x;
                MatrixPage *page_xy_compute, *page_yx_compute;
                {
                    MatrixPage *page_xy = castToMatrixPage(bufferManager.getPage(this->matrixName, block_xy));
                    page_xy_compute = page_xy->createPageForCompute(page_xy);
                    bufferManager.insertMatrixPageIntoPool(page_xy_compute);
                    page_xy_compute->writePage();
                }
                {
                    MatrixPage *page_yx = castToMatrixPage(bufferManager.getPage(this->matrixName, block_yx));
                    MatrixPage *page_yx_compute = page_yx->createPageForCompute(page_yx);
                    bufferManager.insertMatrixPageIntoPool(page_yx_compute);
                    page_yx_compute->writePage();
                }
                page_xy_compute = castToMatrixPage(bufferManager.getPage(newMatrixName, block_xy));
                page_yx_compute = castToMatrixPage(bufferManager.getPage(newMatrixName, block_yx));
                page_xy_compute->compute(page_yx_compute);
                page_xy_compute->writePage();
                page_yx_compute->writePage();
            }
        }
    }
}

                
bool Matrix::rename(string renameToMatrixName) {
    logger.log("Matrix::rename");
    string oldMatrixName = this->matrixName;
    this->matrixName = renameToMatrixName;
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++) {
        const string oldPageName = "../data/temp/" + oldMatrixName + "_Page" + to_string(pageCounter);
        const string newPageName = "../data/temp/" + this->matrixName + "_Page" + to_string(pageCounter);
        if (std::rename(oldPageName.c_str(), newPageName.c_str())) {
            logger.log("Matrix::rename: Error renaming page \"" + oldPageName + "\" to \"" + newPageName + "\"");
            return false;
        }
    }
    matrixCatalogue.renameMatrix(oldMatrixName, this->matrixName);
    return true;
}

bool Matrix::isSymmetric() {
    logger.log("Matrix::isSymmetric");
    trackBlocks = true;
    blocksRead = 0; blocksWritten = 0;
    bool isSymmetric = true;
    function<MatrixPage*(Page*)> castToMatrixPage = [](Page* page) -> MatrixPage* { return dynamic_cast<MatrixPage*>(page); };
    for (uint block_x = 0; block_x < this->blocksPerRow; ++block_x) {
        for (uint block_y = 0; block_y < this->blocksPerRow; ++block_y) {
            if(block_x == block_y) {
                int block_xy = this->blocksPerRow * block_x + block_y;
                MatrixPage *page_xy = castToMatrixPage(bufferManager.getPage(this->matrixName, block_xy));
                isSymmetric = isSymmetric && page_xy->isSymmetric();
            }
            else
            {
                int block_xy = this->blocksPerRow * block_x + block_y;
                int block_yx = this->blocksPerRow * block_y + block_x;
                MatrixPage *page_xy = castToMatrixPage(bufferManager.getPage(this->matrixName, block_xy));
                MatrixPage *page_yx = castToMatrixPage(bufferManager.getPage(this->matrixName, block_yx));

                isSymmetric = isSymmetric && page_xy->isSymmetric(page_yx);
            }
            if (!isSymmetric) {
                return false;
            }
        }
        if (!isSymmetric) {
            return false;
        }
    }
    printBlockInformation();
    trackBlocks = false;
    blocksRead = 0; blocksWritten = 0;
    return true;
}
