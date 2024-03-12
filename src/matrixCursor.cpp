#include "global.h"

MatrixCursor::MatrixCursor(string matrixName, int pageIndex)
{
    logger.log("MatrixCursor::MatrixCursor");
    this->page = bufferManager.getMatrixPage(matrixName, pageIndex);
    this->page.load();
    this->pagePointer = 0;
    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
}

/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int> 
 */
vector<int> MatrixCursor::getNext()
{
    logger.log("Cursor::getNext");
    vector<int> result = this->page.getRow(this->pagePointer);
    matrixCatalogue.getMatrix(this->matrixName)->getNextPointer(this);
    return result;
}

uint MatrixCursor::getPageRowCount() {
    return this->page.getRowCount();
}

uint MatrixCursor::getPageColumnCount() {
    return this->page.getColumnCount();
}

/**
 * @brief Function that loads Page indicated by pageIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pageIndex 
 */
void MatrixCursor::nextPage(int pageIndex)
{
    logger.log("Cursor::nextPage");
    if (this->pageIndex != pageIndex) {
        this->page = bufferManager.getMatrixPage(this->matrixName, pageIndex);
        this->page.load();
        this->pageIndex = pageIndex;
    }
    this->pagePointer = 0;
}

/**
 * @brief Function that loads Page indicated by pageIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pageIndex 
 */
void MatrixCursor::nextPage(int pageIndex, int pagePointer)
{
    logger.log("Cursor::nextPage");
    if (this->pageIndex != pageIndex) {
        this->page = bufferManager.getMatrixPage(this->matrixName, pageIndex);
        this->page.load();
        this->pageIndex = pageIndex;
    }
    this->pagePointer = pagePointer;
}

int MatrixCursor::getPageIndex() const {
    return pageIndex;
}

int MatrixCursor::getPagePointer() const {
    return pagePointer;
}