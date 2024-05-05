#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <sstream>
#include <cmath>
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

double spread(vector<PriceData> &vec1, vector<PriceData> &vec2, int j)
{
    return vec1[j].close - vec2[j].close;
}

double rolling_mean(vector<PriceData> &vec1, vector<PriceData> &vec2, int j, int n)
{
    double sum = 0.0;
    for (int i = j - n + 1; i <= j; i++)
    {
        sum += spread(vec1, vec2, i);
    }
    return sum / n;
}

double rolling_standard_deviation(vector<PriceData> &vec1, vector<PriceData> &vec2, int j, int n)
{
    double rolling_mean_val = rolling_mean(vec1, vec2, j, n);
    double rolling_variance = 0.0;
    for (int i = j - n + 1; i <= j; i++)
    {
        rolling_variance += (rolling_mean_val - spread(vec1, vec2, i)) * (rolling_mean_val - spread(vec1, vec2, i));
    }
    rolling_variance = rolling_variance / n;
    return sqrt(rolling_variance);
}

double z_score(vector<PriceData> &vec1, vector<PriceData> &vec2, int j, int n)
{
    return (spread(vec1, vec2, j) - rolling_mean(vec1, vec2, j, n)) / rolling_standard_deviation(vec1, vec2, j, n);
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

double square_off(std::vector<PriceData> &vec, double total, int position)
{
    double earning = total + position * vec.back().close;
    return earning;
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
    string actions_file_1;
    string actions_file_2;
    string pnl_file;
    string cashflow_file;
    if (best_of_all_check == true)
    {
        actions_file_1 = "order_statistics_1_pairs.csv";
        actions_file_2 = "order_statistics_2_pairs.csv";
        pnl_file = "final_pnl_pairs.txt";
        cashflow_file = "daily_cashflow_pairs.csv";
    }
    else
    {
        actions_file_1 = "order_statistics_1.csv";
        actions_file_2 = "order_statistics_2.csv";
        pnl_file = "final_pnl.txt";
        cashflow_file = "daily_cashflow.csv";
    }

    // Extract required info from make command
    const std::string symbol1 = argv[1];
    const std::string symbol2 = argv[2];
    const std::string start_date = argv[6];
    const std::string end_date = argv[7];
    int threshold = std::stoi(argv[5]);
    int n = std::stoi(argv[4]);
    int x = std::stoi(argv[3]);
    int position = 0;

    // Invoke python scripts to fetch data
    // fs::path current_dir = fs::current_path();
    // fs::path bin_dir = current_dir / "venv" / "bin";
    std::string command1 = "python3 fetch_data.py" + std::string(" ") +
                           symbol1 + " " +
                           start_date + " " +
                           end_date + " " + to_string(n) + " " + "pair1";

    std::string command2 = "python3 fetch_data.py" + std::string(" ") +
                           symbol2 + " " +
                           start_date + " " +
                           end_date + " " + to_string(n) + " " + "pair2";

    int result1 = system(command1.c_str());
    int result2 = system(command2.c_str());

    // Parse the csv file to populate the priceData vector for the first file
    std::ifstream inputFile1("prices1.csv");
    std::vector<PriceData> priceData1;
    std::string line1;
    // Skip the header line
    std::getline(inputFile1, line1);
    while (std::getline(inputFile1, line1))
    {
        std::istringstream iss(line1);
        PriceData entry1;
        std::getline(iss, entry1.date, ',');
        iss >> entry1.close;
        priceData1.push_back(entry1);
    }
    inputFile1.close();

    // Parse the csv file to populate the priceData vector for the second file
    std::ifstream inputFile2("prices2.csv");
    std::vector<PriceData> priceData2;
    std::string line2;
    // Skip the header line
    std::getline(inputFile2, line2);
    while (std::getline(inputFile2, line2))
    {
        std::istringstream iss(line2);
        PriceData entry2;
        std::getline(iss, entry2.date, ',');
        iss >> entry2.close;
        priceData2.push_back(entry2);
    }
    inputFile2.close();

    double total = 0;

    for (int j = n; j < priceData1.size(); j++)
    {
        // Buy signal-buy s1,sell s2
        if (z_score(priceData1, priceData2, j, n) < -1 * threshold)
        {
            if (position < x)
            {
                position++;
                total = total - priceData1[j].close + priceData2[j].close;
                write_to_order_statistics_csv(priceData1, j, 1, "BUY", actions_file_1);
                write_to_order_statistics_csv(priceData2, j, 1, "SELL", actions_file_2);
            }
        }

        // Sell signal-sell s1,buy s2
        if (z_score(priceData1, priceData2, j, n) > threshold)
        {
            if (position > -x)
            {
                position--;
                total = total + priceData1[j].close - priceData2[j].close;
                write_to_order_statistics_csv(priceData1, j, 1, "SELL", actions_file_1);
                write_to_order_statistics_csv(priceData2, j, 1, "BUY", actions_file_2);
            }
        }
        write_to_daily_cashflow_csv(priceData1, j, total, cashflow_file);
        cout << priceData1[j].date << " " << position << endl;
    }
    // Square off
    cout << "DMA Timely square off" << endl;

    double earning1 = square_off(priceData1, total, position);
    cout << earning1 << endl;
    double earning2 = square_off(priceData2, total, -1 * position);
    cout << earning2 << endl;
    double earning = earning1 + earning2 - total;
    cout << priceData1.back().close << endl;
    cout << priceData2.back().close << endl;

    std::ofstream outputFile; // Declare outputFile here
    outputFile.open(pnl_file);
    outputFile << earning;
    outputFile.close();
    std::cout << "PAIRS Profit/Loss written to " << pnl_file << std::endl;
}