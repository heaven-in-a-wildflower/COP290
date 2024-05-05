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
    double high;
    double low;
    double prev_close;
    double close;
    bool faulty;
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

void calculate_measures(vector<PriceData> &vec, int n, vector<double> &true_range, vector<double> &dm_plus, vector<double> &dm_minus, vector<double> &atr, vector<double> &di_plus, vector<double> &di_minus, vector<double> &dx, vector<double> &adx)
{
    // Initial values
    true_range[0] = max(vec[1].high - vec[1].low, max(vec[1].high - vec[1].prev_close, vec[1].low - vec[1].prev_close));

    dm_plus[0] = max(0.0, vec[1].high - vec[0].high);

    dm_minus[0] = max(0.0, vec[1].low - vec[0].low);

    atr[0] = true_range[0];

    di_plus[0] = dm_plus[0] / atr[0];

    di_minus[0] = dm_minus[0] / atr[0];

    if (di_plus[0] + di_minus[0] != 0)
    {
        dx[0] = (di_plus[0] - di_minus[0]) / (di_plus[0] + di_minus[0]);
    }
    else
    {
        vec[1].faulty = true;
        dx[0] = 0;
    }

    adx[0] = dx[0];

    for (int j = 2; j < vec.size(); j++)
    {
        int k = j - 1;

        true_range[k] = max(vec[j].high - vec[j].low, max(vec[j].high - vec[j].prev_close, vec[j].low - vec[j].prev_close));

        dm_plus[k] = max(0.0, vec[j].high - vec[j - 1].high);
        dm_minus[k] = max(0.0, vec[j].low - vec[j - 1].low);

        double alpha = 2.0 / (n + 1);

        atr[k] = alpha * (true_range[k] - atr[k - 1]) + atr[k - 1];

        di_plus[k] = alpha * ((dm_plus[k] / atr[k]) - di_plus[k - 1]) + di_plus[k - 1];
  
        di_minus[k] = alpha * ((dm_minus[k] / atr[k]) - di_minus[k - 1]) + di_minus[k - 1];

        if (di_plus[k] + di_minus[k] != 0)
        {
            dx[k] = (di_plus[k] - di_minus[k]) / (di_plus[k] + di_minus[k]);
        }
        else
        {
            vec[j].faulty = true;
            // cout << "Fault at j= " << j << endl;
            dx[k] = 0;
        }

        adx[k] = alpha * (dx[k] - adx[k - 1]) + adx[k - 1];
    }
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

void write_to_daily_cashflow_csv(const std::vector<PriceData> &data, int j, double cashflow, std::string cashflow_file, vector<double> &adx_vec, vector<double> &tr_vec, vector<double> &atr_vec, vector<double> &dx_vec, vector<double> &dm_plus_vec, vector<double> &dm_minus_vec, vector<double> &di_plus_vec, vector<double> &di_minus_vec)
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
    outputFile << convertDateFormat(entry.date) << " " << to_string(tr_vec[j - 1]) << " " << to_string(atr_vec[j - 1]) << " " << to_string(dx_vec[j - 1]) << "," << to_string(adx_vec[j - 1]) << "," << to_string(dm_plus_vec[j - 1]) << "," << to_string(dm_minus_vec[j - 1]) << "," << to_string(di_plus_vec[j - 1]) << "," << to_string(di_minus_vec[j - 1]) << "," << std::to_string(cashflow) << std::endl;
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
    std::cout << "ADX Profit/Loss written to " << pnl_file << std::endl;
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
        actions_file = "order_statistics_adx.csv";
        pnl_file = "final_pnl_adx.txt";
        cashflow_file = "daily_cashflow_adx.csv";
    }
    else
    {
        actions_file = "order_statistics.csv";
        pnl_file = "final_pnl.txt";
        cashflow_file = "daily_cashflow.csv";
    }

    // Extract required info from make command
    const std::string symbol = argv[1];
    const std::string start_date = argv[5];
    const std::string end_date = argv[6];
    int x = stoi(argv[2]);
    int n = stoi(argv[3]);
    double adx_threshold = stod(argv[4]);
    int position = 0;

    // Invoke python script to fetch data
    // fs::path current_dir = fs::current_path();
    // fs::path bin_dir = current_dir / "venv" / "bin";
    std::string command = "python3 fetch_data.py" + std::string(" ") +
                          symbol + " " +
                          start_date + " " +
                          end_date + " " + "1" + " " + "adx";

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
        std::string high, low, prev_close, close;

        // Read the parameters separated by commas
        std::getline(iss, entry.date, ',');
        std::getline(iss, high, ',');
        std::getline(iss, low, ',');
        std::getline(iss, prev_close, ',');
        std::getline(iss, close);

        // Convert parameters to appropriate types
        entry.high = std::stod(high);
        entry.low = std::stod(low);
        entry.prev_close = std::stod(prev_close);
        entry.close = std::stod(close);
        entry.faulty = false;
        priceData.push_back(entry);
    }
    inputFile.close();

    // Execute strategy
    int m = priceData.size() - 1;
    vector<double> true_range(m);
    vector<double> dm_plus(m);
    vector<double> dm_minus(m);
    vector<double> atr(m);
    vector<double> di_plus(m);
    vector<double> di_minus(m);
    vector<double> dx(m);
    vector<double> adx(m);

    calculate_measures(priceData, n, true_range, dm_plus, dm_minus, atr, di_plus, di_minus, dx, adx);
    double total = 0;
    for (int j = 1; j < m; j++)
    {
        if (priceData[j].faulty == false)
        {
            if (adx[j] >= adx_threshold)
            {
                if (position < x)
                {
                    position++;
                    total -= priceData[j].close;
                    write_to_order_statistics_csv(priceData, j, 1, "BUY", actions_file);
                }
            }

            if (adx[j] < adx_threshold)
            {
                if (position > -x)
                {
                    position--;
                    total += priceData[j].close;
                    write_to_order_statistics_csv(priceData, j, 1, "SELL", actions_file);
                }
            }
            // cout << priceData[j].date << endl;
        }
        write_to_daily_cashflow_csv(priceData, j, total, cashflow_file, adx, true_range, atr, dx, dm_plus, dm_minus, di_plus, di_minus);
    }
    // In case of square-off at end_date
    cout << "ADX Timely square off" << endl;
    cout << "ADX Actions written to " << actions_file << endl;
    square_off(priceData, total, position, pnl_file);
}