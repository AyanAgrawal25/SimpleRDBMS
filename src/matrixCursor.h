#include"bufferManager.h"
/**
 * @brief The cursor is an important component of the system. To read from a
 * matrix, you need to initialize a cursor. The cursor reads rows from a page one
 * at a time.
 *
 */
class MatrixCursor{
    private:
    MatrixPage page;
    int pageIndex;
    string matrixName;
    int pagePointer;

    public:
    MatrixCursor(string matrixName, int pageIndex);
    vector<int> getNext();
    uint getPageRowCount();
    uint getPageColumnCount();
    void nextPage(int pageIndex, int pagePointer);
    void nextPage(int pageIndex);
    int getPageIndex() const;
    int getPagePointer() const;
};