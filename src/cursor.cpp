#include "global.h"

Cursor::Cursor(string tableName, int pageIndex)
{
    logger.log("Cursor::Cursor");
    this->page = bufferManager.getTablePage(tableName, pageIndex);
    this->pagePointer = 0;
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->runsThisPass = 1u;
}

Cursor::Cursor(string tableName, uint runsThisPass, int pageIndex) {
    logger.log("Cursor::Cursor");
    this->runsThisPass = runsThisPass;
    this->page = bufferManager.getTablePage(tableName, runsThisPass, pageIndex);
    logger.log("Cursor::Cursor -> Retried table page");
    this->pagePointer = 0;
    this->tableName = tableName;
    this->pageIndex = pageIndex;
}


/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int> 
 */
vector<int> Cursor::getNext()
{
    logger.log("Cursor::getNext");
    vector<int> result = this->getNextOfCurrentPage();
    if(result.empty()){
        tableCatalogue.getTable(this->tableName)->getNextPage(this);
        if(!this->pagePointer){
            result = this->page.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}

/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to). If there is no next row in the current page,
 * this does not move the page pointer forward.
 *
 * @return vector<int> 
 */
vector<int> Cursor::getNextOfCurrentPage() {
    logger.log("Cursor::getNextOfCurrentPage");
    vector<int> result = (this->page).getRow(this->pagePointer++);
    if (result.empty()) {
        this->pagePointer--;
    }
    return result;
}

/**
 * @brief Function that loads Page indicated by pageIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pageIndex 
 */
void Cursor::nextPage(int pageIndex)
{
    logger.log("Cursor::nextPage");
    this->page = bufferManager.getTablePage(this->tableName, this->runsThisPass, pageIndex);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}

void Cursor::goToRowOfCurrentPage(int pagePointer) {
    logger.log("Cursor::goToRowOfCurrentPage");
    this->pagePointer = pagePointer;
}


int Cursor::getPageIndex() const {
    return pageIndex;
}

int Cursor::getPagePointer() const {
    return pagePointer;
}