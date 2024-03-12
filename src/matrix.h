#include "matrixCursor.h"
#include <cstdio>

/**
 * @brief The Matrix class holds all information related to a loaded matrix. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. 
 *
 */
class Matrix
{
    /***--------------------------- utility functions ----------------------***/
    bool isLastBlockInRow(uint blockIndex);
    bool readRowInBlock(ifstream &fin, const uint columnsThisBlock, const bool isLastBlock, vector<int> &row);
    void writeRowInPage(uint pageIndex, const vector<int> &rowInBlock);
public:
    string sourceFileName = "";
    string matrixName = "";
    uint columnCount = 0;
    uint rowCount = 0;
    uint maxDimPerBlock = 0;
    uint blocksPerRow = 0;
    uint blockCount = 0;
    vector<uint> rowsPerBlockCount;
    vector<uint> columnsPerBlockCount;
    
    bool calculateDimensions();
    bool blockify();
    Matrix();
    Matrix(string matrixName);
    bool load();
    void print();
    // export 
    bool isExported();
    void makeExportation();

    void transpose();
    void getNextPage(MatrixCursor *cursor);
    void getNextPointer(MatrixCursor *cursor);
    // MatrixCursor getCursor();
    
    // compute
    void compute();
    void insertMatrixForCompute();
    void unload();
    bool rename(string renameToMatrixName);
    bool isSymmetric();

    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row
     */
    template <typename T>
    void writeRow(vector<T> row, ostream &fout, bool firstEntryInMatrixRow, bool lastPage)
    {
        logger.log("Matrix::printRow");
        for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
        {
            if (!firstEntryInMatrixRow || columnCounter != 0)
                fout << ", ";
            fout << row[columnCounter];
        }
        if (lastPage)
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
void writeRow(vector<T> row, ostream &fout)
{
    logger.log("Matrix::printRow");
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
    logger.log("Matrix::printRow");
    ofstream fout(this->sourceFileName, ios::app);
    this->writeRow(row, fout);
    fout.close();
}
};