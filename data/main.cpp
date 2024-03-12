#include <iostream>
#include <fstream>
#include <cstdlib> // For generating random values

int main()
{
    // Open a file for writing
    std::ofstream outputFile("EMP.csv");

    // Check if the file opened successfully
    if (!outputFile.is_open())
    {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    // Write the header row
    outputFile << "Ssn, Bdate, Sex, Salary, Super_ssn, Dno" << std::endl;

    // Generate and write 10,000 rows of data
    for (int i = 1; i <= 10000; ++i)
    {
        int Ssn = i;
        int Bdate = 500000 + rand() % 500000; // Random Bdate within a range
        int Sex = rand() % 2;                 // Random 0 or 1 for Sex
        int Salary = 20000 + rand() % 40000;  // Random Salary within a range
        int Super_ssn = rand() % 9;           // Random Super_ssn between 0 and 8
        int Dno = rand() % 6;                 // Random Dno between 0 and 5

        // Write the generated values to the file
        outputFile << Ssn << ", " << Bdate << ", " << Sex << ", " << Salary << ", " << Super_ssn << ", " << Dno << std::endl;
    }

    // Close the file
    outputFile.close();

    std::cout << "Data generation completed." << std::endl;

    return 0;
}
