#include "global.h"

BufferManager::BufferManager()
{
    logger.log("BufferManager::BufferManager");
}

BufferManager::~BufferManager()
{
    logger.log("BufferManager::~BufferManager");
    for (auto page : this->pages)
        delete page;
    
}

/**
 * @brief Function called to read a page from the buffer manager. If the page is
 * not present in the pool, the page is read and then inserted into the pool.
 *
 * @param tableName 
 * @param pageIndex 
 * @return Page*
 */
Page* BufferManager::getPage(string tableName, int pageIndex)
{
    logger.log("BufferManager::getPage");
    string pageName = "../data/temp/"+tableName + "_Page" + to_string(pageIndex);
    if (this->inPool(pageName))
        return this->getFromPool(pageName);
    if (tableCatalogue.isTable(tableName))
        return this->insertTablePageIntoPool(tableName, pageIndex);
    if (trackBlocks)
        blocksRead++;
    return this->insertMatrixPageIntoPool(tableName, pageIndex);
}

/**
 * @brief Function called to read a page from the buffer manager. If the page is
 * not present in the pool, the page is read and then inserted into the pool.
 *
 * @param tableName 
 * @param pageIndex 
 * @return Page 
 */
Page BufferManager::getTablePage(string tableName, int pageIndex)
{
    logger.log("BufferManager::getTablePage");
    return *(this->getPage(tableName, pageIndex));
}

/**
 * @brief Function called to read a page from the buffer manager. If the page is
 * not present in the pool, the page is read and then inserted into the pool.
 *
 * @param tableName 
 * @param runsThisPass
 * @param pageIndex 
 * @return Page 
 */
Page BufferManager::getTablePage(string tableName, uint runsThisPass, int pageIndex)
{
    logger.log("BufferManager::getTablePage");
    if (runsThisPass == 1)
        return (this->getTablePage(tableName, pageIndex));

    string pageName = "../data/temp/"+tableName + "_nRuns-" + to_string(runsThisPass) + "_Page" + to_string(pageIndex);
    if (this->inPool(pageName))
        return *(this->getFromPool(pageName));
    else
        return *(this->insertTablePageIntoPool(tableName, runsThisPass, pageIndex));

}

/**
 * @brief Function called to read a page from the buffer manager. If the page is
 * not present in the pool, the page is read and then inserted into the pool.
 *
 * @param matrixName
 * @param pageIndex 
 * @return MatrixPage
 */
MatrixPage BufferManager::getMatrixPage(string matrixName, int pageIndex)
{
    logger.log("BufferManager::getMatrixPage");
    Page *page = this->getPage(matrixName, pageIndex);
    return *(dynamic_cast<MatrixPage *>(page));
}

/**
 * @brief Checks to see if a page exists in the pool
 *
 * @param pageName 
 * @return true 
 * @return false 
 */
bool BufferManager::inPool(string pageName)
{
    logger.log("BufferManager::inPool");
    for (auto page : this->pages)
    {
        if (pageName == page->pageName)
            return true;
    }
    return false;
}

/**
 * @brief If the page is present in the pool, then this function returns the
 * page. Note that this function will fail if the page is not present in the
 * pool.
 *
 * @param pageName 
 * @return Page* 
 */
Page* BufferManager::getFromPool(string pageName)
{
    logger.log("BufferManager::getFromPool");
    Page *foundPage = nullptr;
    for (auto page : this->pages) {
        if (pageName == page->pageName)
            foundPage = page;
    }
    return foundPage;
}

/**
 * @brief Inserts page indicated by tableName and pageIndex into pool. If the
 * pool is full, the pool ejects the oldest inserted page from the pool and adds
 * the current page at the end. It naturally follows a queue data structure. 
 *
 * @param tableName 
 * @param pageIndex 
 * @return Page 
 */
Page* BufferManager::insertTablePageIntoPool(string tableName, int pageIndex)
{
    logger.log("BufferManager::insertTablePageIntoPool");
    Page* page = new Page(tableName, pageIndex);
    if (this->pages.size() >= BLOCK_COUNT) {
        delete pages.front();
        pages.pop_front();
    }
    pages.push_back(page);
    return page;
}

/**
 * @brief Inserts page indicated by tableName and pageIndex into pool. If the
 * pool is full, the pool ejects the oldest inserted page from the pool and adds
 * the current page at the end. It naturally follows a queue data structure. 
 *
 * @param tableName 
 * @param runsThisPass
 * @param pageIndex 
 * @return Page 
 */
Page* BufferManager::insertTablePageIntoPool(string tableName, uint runsThisPass, int pageIndex)
{
    logger.log("BufferManager::insertTablePageIntoPool");
    Page* page = new Page(tableName, runsThisPass, pageIndex);
    if (this->pages.size() >= BLOCK_COUNT) {
        delete pages.front();
        pages.pop_front();
    }
    pages.push_back(page);
    return page;
}


/**
 * @brief Inserts page indicated by tableName and pageIndex into pool. If the
 * pool is full, the pool ejects the oldest inserted page from the pool and adds
 * the current page at the end. It naturally follows a queue data structure. 
 *
 * @param tableName 
 * @param pageIndex 
 * @return Page 
 */
Page* BufferManager::insertMatrixPageIntoPool(string matrixName, int pageIndex)
{
    logger.log("BufferManager::insertMatrixPageIntoPool");
    MatrixPage* page = new MatrixPage(matrixName, pageIndex);
    page->load();
    if (this->pages.size() >= BLOCK_COUNT)
        pages.pop_front();
    pages.push_back(page);
    return page;
}

Page* BufferManager::insertMatrixPageIntoPool(MatrixPage *page)
{
    logger.log("BufferManager::insertMatrixPageIntoPool");
    if (this->pages.size() >= BLOCK_COUNT)
        pages.pop_front();
    pages.push_back(dynamic_cast<Page*>(page));
    return page;
}

/**
 * @brief The buffer manager is also responsible for writing pages. This is
 * called when new tables are created using assignment statements.
 *
 * @param tableName 
 * @param pageIndex 
 * @param rows 
 * @param rowCount 
 */
void BufferManager::writePage(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("BufferManager::writePage");
    Page page(tableName, pageIndex, rows, rowCount);
    // cout << "After pageconstructor" << endl;
    // for (auto row : page.rows) {
    //     for (auto element : row) {
    //         cout << element << " ";
    //     }
    //     cout << endl;
    // }
    // cout << "Writing Page\n";
    page.writePage();
}

/**
 * @brief The buffer manager is also responsible for writing pages. This is
 * called when pages of individual runs are written during external sorting. 
 *
 * @param tableName 
 * @param runsThisPass Number of blocks in the current run while external sorting
 * @param pageIndex 
 * @param rows 
 * @param rowCount 
 */
void BufferManager::writePage(string tableName, uint runsThisPass, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("BufferManager::writePage");
    Page *page;
    // if this is the last pass, no. of runs = 1
    if (runsThisPass == 1u) {
        page = new Page(tableName, pageIndex, rows, rowCount);
    } else {
        page = new Page(tableName, runsThisPass, pageIndex, rows, rowCount);
    }
    page->writePage();
}

/**
 * @brief Deletes file names fileName
 *
 * @param fileName 
 */
void BufferManager::deleteFile(string fileName)
{
    
    if (remove(fileName.c_str()))
        logger.log("BufferManager::deleteFile: Err");
        else logger.log("BufferManager::deleteFile: Success");
}

bool BufferManager::copyFile(string sourceFileName, string destinationFileName) {
    logger.log("BufferManager::copyFile");
    std::filebuf sourceFileBuf;
    std::filebuf destFileBuf;

    // Open the source file
    if (sourceFileBuf.open(sourceFileName, fstream::in | fstream::binary) &&
        destFileBuf.open(destinationFileName, fstream::out | fstream::binary)) {

        // Copy the data from source to destination
        char buffer[1024];
        std::streamsize bytesRead;
        while ((bytesRead = sourceFileBuf.sgetn(buffer, sizeof(buffer))) > 0) {
            destFileBuf.sputn(buffer, bytesRead);
        }
        // destFileBuf.pubsync();

        // Close the files
        sourceFileBuf.close();
        destFileBuf.close();
        logger.log("File copied successfully.");
    } else {
        std::cerr << "Error opening files." << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Overloaded function that calls deleteFile(fileName) by constructing
 * the fileName from the tableName and pageIndex.
 *
 * @param tableName 
 * @param pageIndex 
 */
void BufferManager::deleteFile(string tableName, int pageIndex)
{
    logger.log("BufferManager::deleteFile");
    string fileName = "../data/temp/"+tableName + "_Page" + to_string(pageIndex);
    this->deleteFile(fileName);
}

/**
 * @brief Deletes files of pages in table made during external sorting
 * of a particular run and pageIndex
 *
 * @param fileName 
 * @param runsThisPass No. of runs this current pass in the sorting phase
 * @param pageIndex 
 */
void BufferManager::deleteFile(string tableName, uint runsThisPass, int pageIndex) {
    logger.log("BufferManager::deleteFile");
    string fileName;
    // If this was the last run
    if (runsThisPass == 1u) {
        fileName = "../data/temp/"+tableName + "_Page" + to_string(pageIndex);
    } else {
        fileName = "../data/temp/"+tableName + "_nRuns-" + to_string(runsThisPass) + "_Page" + to_string(pageIndex);
    }
    this->deleteFile(fileName);
}

    
void BufferManager::updatePageInQueue(Page* updatedPage) {
    logger.log("BufferManager::updatePageInQueue");
    pages.clear();
}