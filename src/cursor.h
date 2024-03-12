#pragma once
#include"bufferManager.h"
/**
 * @brief The cursor is an important component of the system. To read from a
 * table, you need to initialize a cursor. The cursor reads rows from a page one
 * at a time.
 *
 */
class Cursor{
    private:
    Page page;
    int pageIndex;
    string tableName;
    int pagePointer;
    uint runsThisPass;

    public:
    Cursor(string tableName, int pageIndex);
    Cursor(string tableName, uint runsThisPass, int pageIndex);
    vector<int> getNext();
    vector<int> getNextOfCurrentPage();
    void nextPage(int pageIndex);
    void goToRowOfCurrentPage(int pagePointer);
    int getPageIndex() const;
    int getPagePointer() const;
};