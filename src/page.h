#pragma once
#include"logger.h"
/**
 * @brief The Page object is the main memory representation of a physical page
 * (equivalent to a block). The page class and the page.h header file are at the
 * bottom of the dependency tree when compiling files. 
 *<p>
 * Do NOT modify the Page class. If you find that modifications
 * are necessary, you may do so by posting the change you want to make on Moodle
 * or Teams with justification and gaining approval from the TAs. 
 *</p>
 */

class Page{
protected:
    string tableName;
    int pageIndex;
    uint columnCount;
    uint rowCount;
    uint runsThisPass;

public:
    vector<vector<int>> rows;

    string pageName = "";
    Page();
    Page(string tableName, int pageIndex);
    Page(string tableName, uint runsThisPass, int pageIndex);
    Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount);
    Page(string tableName, uint runsThisPass, int pageIndex, vector<vector<int>> rows, int rowCount);
    vector<int> getRow(int rowIndex);
    void writePage();

    virtual ~Page() = default;
};

class MatrixPage : public Page {

    string matrixName;
    
public:

    string pageName = "";
    MatrixPage();
    MatrixPage(string matrixName, int pageIndex);
    MatrixPage(string matrixName, int pageIndex, vector<vector<int>> &rows);
    void load();
    uint getRowCount();
    uint getColumnCount();
    void transpose();
    void transpose(MatrixPage *page);
    void compute();
    void compute(MatrixPage* page);
    MatrixPage* createPageForCompute(MatrixPage* page);
    bool isSymmetric();
    bool isSymmetric(MatrixPage *page);
    void writePage();
    void writePage(string pageName);
};