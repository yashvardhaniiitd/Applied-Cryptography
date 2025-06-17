#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdint>

using namespace std;

// Function to read CSV file and convert data to uint16_t
vector<vector<uint16_t>> readCSV(const string& fileName) {
    vector<vector<uint16_t>> data;
    ifstream file(fileName);

    if (!file.is_open()) {
        cerr << "Error opening file: " << fileName << endl;
        return data;
    }

    string line;
    // Read each line in the file
    while (getline(file, line)) {
        vector<uint16_t> row;
        stringstream lineStream(line);
        string cell;

        // Split line into cells separated by commas
        while (getline(lineStream, cell, ',')) {
            try {
                // Convert the cell to uint16_t and add to the row
                uint16_t value = static_cast<uint16_t>(stoi(cell, nullptr, 16));
                row.push_back(value);
            } catch (const invalid_argument& e) {
                cerr << "Invalid value found in CSV: " << cell << endl;
                row.push_back(0);  // Default to 0 on error
            } catch (const out_of_range& e) {
                cerr << "Value out of range for uint16_t: " << cell << endl;
                row.push_back(0);  // Default to 0 on error
            }
        }
        data.push_back(row);  // Add the row to the data
    }

    file.close();
    return data;
}

// int main() {
//     // Read the CSV file
//     string fileName = "data.csv";
//     cout << "Reading file: " << fileName << endl;

//     vector<vector<uint16_t>> csvData = readCSV(fileName);

//     // Print the contents of the CSV file
//     cout << "CSV Data:" << endl;
//     for (const auto& row : csvData) {
//         for (const auto& cell : row) {
//             cout <<hex << cell << " ";
//         }
//         cout << endl;
//     }

//     return 0;
// }
