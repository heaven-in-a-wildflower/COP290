#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <sstream>
#include <cmath>
using namespace std;
namespace fs = std::filesystem;

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

double average_gain(vector<PriceData> &vec, int j, int n)
{
    double total_gain = 0.0;
    for (int i = j - n + 1; i <= j; i++)
    {
        total_gain += max(0.0, vec[i].close - vec[i - 1].close);
    }
    return total_gain / n;
}

double average_loss(vector<PriceData> &vec, int j, int n)
{
    double total_loss = 0.0;
    for (int i = j - n + 1; i <= j; i++)
    {
        total_loss += max(0.0, vec[i - 1].close - vec[i].close);
    }
    return total_loss / n;
}

double RSI(vector<PriceData> &vec, int j, int n)
{
    double rs = average_gain(vec, j, n) / average_loss(vec, j, n);
    double rsi = 100 - 100 / (1 + rs);
    return rsi;
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
    std::cout << "RSI Profit/Loss written to " << pnl_file << std::endl;
}

void raiseErrorIfLarger(double var1, double var2)
{
    if (var1 > var2)
        throw std::logic_error("Error: Value of overbought_threshold < oversold_threshold");
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
        actions_file = "order_statistics_rsi.csv";
        pnl_file = "final_pnl_rsi.txt";
        cashflow_file = "daily_cashflow_rsi.csv";
    }
    else
    {
        actions_file = "order_statistics.csv";
        pnl_file = "final_pnl.txt";
        cashflow_file = "daily_cashflow.csv";
    }

    // Extract required info from make command
    const std::string symbol = argv[1];
    const std::string start_date = argv[6];
    const std::string end_date = argv[7];
    int x = stoi(argv[2]);
    int n = stoi(argv[3]);
    double oversold_threshold = stod(argv[4]);
    double overbought_threshold = stod(argv[5]);
    int position = 0;

    // Throw error if overbought_threshold < oversold_threshold.
    try
    {
        raiseErrorIfLarger(oversold_threshold, overbought_threshold);
    }
    catch (const std::logic_error &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // Invoke python script to fetch data
    // fs::path current_dir = fs::current_path();
    // fs::path bin_dir = current_dir / "venv" / "bin";
    std::string command = "python3 fetch_data.py" + std::string(" ") +
                          symbol + " " +
                          start_date + " " +
                          end_date + " " + to_string(n) + " " + "false";

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
        if (RSI(priceData, j, n) <= oversold_threshold)
        {
            if (position < x)
            {
                position++;
                total -= priceData[j].close;
                write_to_order_statistics_csv(priceData, j, 1, "BUY", actions_file);
            }
        }

        if (RSI(priceData, j, n) >= overbought_threshold)
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
    cout << "RSI Timely square off" << endl;
    cout << "RSI Actions written to " << actions_file << endl;
    square_off(priceData, total, position, pnl_file);
}
