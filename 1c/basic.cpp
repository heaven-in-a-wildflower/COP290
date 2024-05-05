#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <sstream>
using namespace std;
namespace fs = std::filesystem;
string filename;

struct PriceData
{
    std::string date;
    double close;
};

std::string convertDateFormat(const std::string &date)
{
    // Initialize string stream
    std::stringstream ss(date);

    // Temporary variables for year, month, and day
    int year, month, day;

    // Parse the input string
    char delim;
    ss >> year >> delim >> month >> delim >> day;

    // Build the new date string in the required format
    std::stringstream result;
    result << std::setw(2) << std::setfill('0') << day << "/"
           << std::setw(2) << std::setfill('0') << month << "/"
           << year;

    return result.str();
}

bool isMonotonicIncr(const std::vector<PriceData> &data, int j, int n)
{
    for (int i = j - n; i < j; i++)
    {
        if (data[i].close >= data[i + 1].close)
        {
            return false; // Not monotonically increasing
        }
    }
    return true; // Monotonically increasing
}

bool isMonotonicDecr(const std::vector<PriceData> &data, int j, int n)
{
    for (int i = j - n; i < j; i++)
    {
        if (data[i].close <= data[i + 1].close)
        {
            return false; // Not monotonically decreasing
        }
    }
    return true; // Monotonically decreasing
}

void write_to_order_statistics_csv(const std::vector<PriceData> &data, int j, int quantity, std::string direction, std::string actions_file)
{
    // Open file in append mode
    std::ofstream outputFile(actions_file, std::ios::app);

    // Check if the file is empty
    outputFile.seekp(0, std::ios::end);
    bool is_empty = outputFile.tellp() == 0;
    outputFile.seekp(0, std::ios::beg);

    // Write column headers if the file is empty
    if (is_empty)
    {
        outputFile << "Date,Order_dir,Quantity,Price" << std::endl;
    }

    // Write data
    PriceData entry = data[j];
    outputFile << convertDateFormat(entry.date) << "," << direction << ","
               << std::to_string(std::abs(quantity))
               << "," << entry.close << std::endl;
    outputFile.close();
}

void write_to_daily_cashflow_csv(const std::vector<PriceData> &data, int j, double cashflow, std::string cashflow_file)
{
    // Open file in append mode
    std::ofstream outputFile(cashflow_file, std::ios::app);

    // Check if the file is empty
    outputFile.seekp(0, std::ios::end);
    bool is_empty = outputFile.tellp() == 0;
    outputFile.seekp(0, std::ios::beg);

    // Write column headers if the file is empty
    if (is_empty)
    {
        outputFile << "Date,Cashflow" << std::endl;
    }

    // Write data
    PriceData entry = data[j];
    outputFile << convertDateFormat(entry.date) << "," << std::to_string(cashflow) << std::endl;
    outputFile.close();
}

void square_off(std::vector<PriceData> &vec, double total, int position, string pnl_file)
{
    double earning;
    std::ofstream outputFile; // Declare outputFile here

    earning = total + position * vec.back().close;

    outputFile.open(pnl_file);
    outputFile << earning;
    outputFile.close();
    std::cout << "BASIC Profit/Loss written to " << pnl_file << std::endl;
}

int main(int argc, char *argv[])
{
    // Fetch environment variable for individual execution:
    bool best_of_all_check = false;
    const char *is_best_of_all = std::getenv("BEST_OF_ALL_CHECK");
    if (string(is_best_of_all) == "true")
    {
        best_of_all_check = true;
    }

    // Set names of target files
    string actions_file;
    string pnl_file;
    string cashflow_file;
    if (best_of_all_check == true)
    {
        actions_file = "order_statistics_basic.csv";
        pnl_file = "final_pnl_basic.txt";
        cashflow_file = "daily_cashflow_basic.csv";
    }
    else
    {
        actions_file = "order_statistics.csv";
        pnl_file = "final_pnl.txt";
        cashflow_file = "daily_cashflow.csv";
    }

    // Extract required info from make command
    const std::string symbol = argv[1];
    const std::string start_date = argv[4];
    const std::string end_date = argv[5];
    int n = std::stoi(argv[2]);
    int x = std::stoi(argv[3]);
    int position = 0;

    // Invoke python script to fetch data
    // fs::path current_dir = fs::current_path();
    // fs::path bin_dir = current_dir / "venv" / "bin";
     std::string command = "python3 fetch_data.py" +
                           std::string(" ") +
                           symbol + " " +
                           start_date + " " +
                           end_date + " " +
                           std::to_string(n) + " " +
                           "false";

    // Execute the command
    int result = system(command.c_str());

    // Parse the csv file to populate the priceData vector
    std::ifstream inputFile("prices.csv");
    std::vector<PriceData> priceData;
    std::string line;

    // Skip the header line
    std::getline(inputFile, line);

    while (std::getline(inputFile, line))
    {
        std::istringstream iss(line);
        PriceData entry;
        std::getline(iss, entry.date, ',');
        iss >> entry.close;
        priceData.push_back(entry);
    }
    inputFile.close();

    double total = 0;
    // Execute strategy
    for (int j = n; j < priceData.size(); j++)
    {
        if (isMonotonicIncr(priceData, j, n))
        {
            if (position < x)
            {
                position++;
                total -= priceData[j].close;
                write_to_order_statistics_csv(priceData, j, 1, "BUY", actions_file);
            }
        }

        if (isMonotonicDecr(priceData, j, n))
        {
            if (position > -x)
            {
                position--;
                total += priceData[j].close;
                write_to_order_statistics_csv(priceData, j, 1, "SELL", actions_file);
            }
        }
        write_to_daily_cashflow_csv(priceData, j, total, cashflow_file);
    }

    // In case of square-off at end_date
    cout << "BASIC Timely square off" << endl;
    cout << "BASIC Actions written to " << actions_file << endl;
    square_off(priceData, total, position, pnl_file);
}
