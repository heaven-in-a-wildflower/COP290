#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <deque>
#include <iomanip>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

struct PriceData
{
    std::string date;
    double close;
    bool faulty;
};

struct stored_stock
{
    double price;
    int age;
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

void calculate_adaptive_moving_averages(vector<PriceData> &vec, int n, double c1, double c2, vector<double> &ama, vector<double> &sf)
{
    ama[0] = vec[n].close;
    sf[0] = 0.5;
    for (int j = n + 1; j < vec.size(); j++)
    {
        int k = j - n;
        double change_over_n_days = vec[j].close - vec[j - n].close;
        double sum_of_absolute_price_change_over_n_days = 0.0;
        for (int i = j - n + 1; i <= j; i++)
        {
            sum_of_absolute_price_change_over_n_days += abs(vec[i].close - vec[i - 1].close);
        }
        if (sum_of_absolute_price_change_over_n_days != 0)
        {
            double efficiency_ratio = change_over_n_days / sum_of_absolute_price_change_over_n_days;
            double alpha = (2 * efficiency_ratio) / (1 + c2);
            double frac = (alpha - 1) / (alpha + 1);
            sf[k] = sf[k - 1] + c1 * (frac - sf[k - 1]);
        }
        else
        {
            vec[j].faulty = true;
            sf[k] = sf[k - 1];
        }
        ama[k] = ama[k - 1] + sf[k] * (vec[j].close - ama[k - 1]);
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

void write_to_daily_cashflow_csv(const std::vector<PriceData> &data, int j, double cashflow, std::string cashflow_file, vector<double> &ama_vec, vector<double> &sf_vec, int n)
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
        outputFile << "Date,Cashflow,AMA,Price,SF" << std::endl;
    }

    double ama = ama_vec[j - n];
    double sf = sf_vec[j - n];
    // Write data
    PriceData entry = data[j];
    outputFile << convertDateFormat(entry.date) << "," << std::to_string(cashflow) << "," << std::to_string(ama) << "," << std::to_string(entry.close) << "," << std::to_string(sf) << std::endl;
    outputFile.close();
}

void square_off(std::vector<PriceData> &vec, double total, int position, string pnl_file)
{
    double earning;
    std::ofstream outputFile(pnl_file); // Declare outputFile here

    earning = total + position * vec.back().close;

    outputFile << earning;
    outputFile.close();
    cout << "DMA++ Profit/Loss written to " << pnl_file << std::endl;
}

int main(int argc, char *argv[])
{
    // Fetch environment variable for individual execution:
    bool best_of_all_check = false;
    const char *is_best_of_all = std::getenv("BEST_OF_ALL_CHECK");
    if (std::string(is_best_of_all) == "true")
    {
        best_of_all_check = true;
    }

    // Set names of target files
    string actions_file;
    string pnl_file;
    string cashflow_file;
    if (best_of_all_check == true)
    {
        actions_file = "order_statistics_dma++.csv";
        pnl_file = "final_pnl_dma++.txt";
        cashflow_file = "daily_cashflow_dma++.csv";
    }
    else
    {
        actions_file = "order_statistics.csv";
        pnl_file = "final_pnl.txt";
        cashflow_file = "daily_cashflow.csv";
    }

    // Extract required info from make command
    const std::string symbol = argv[1];
    const std::string start_date = argv[8];
    const std::string end_date = argv[9];
    int position = 0;

    // Check that n and x are correctly provided

    int x = std::stoi(argv[2]);
    int p = std::stoi(argv[3]);
    int n = std::stoi(argv[4]);
    int max_hold_days = std::stoi(argv[5]);
    double c1 = std::stod(argv[6]);
    double c2 = std::stod(argv[7]);

    // Invoke python script to fetch data
    // fs::path current_dir = fs::current_path();
    // fs::path bin_dir = current_dir / "venv" / "bin";
    std::string command = "python3 fetch_data.py" + std::string(" ") +
                          symbol + " " +
                          start_date + " " +
                          end_date + " " + std::to_string(n) + " " + "false";

    // Execute the command
    int result = system(command.c_str());

    std::ifstream inputFile("prices.csv");
    // Parse the csv file to populate the priceData vector
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
        entry.faulty = false;
        priceData.push_back(entry);
    }
    inputFile.close();

    double total = 0;
    bool flag1;
    bool flag2;

    // Max-hold-days
    deque<stored_stock> stocks_bought;
    deque<stored_stock> stocks_sold;

    // AMA strategy
    vector<double> ama(priceData.size() - n, 0);
    vector<double> sf(priceData.size() - n, 0);

    // cout << "wow" << endl;

    calculate_adaptive_moving_averages(priceData, n, c1, c2, ama, sf);

    for (int j = n; j < priceData.size(); j++)
    {
        flag1 = false;
        flag2 = false;
        int psedu_position = 0;
        if (priceData[j].faulty == false)
        {
            int k = j - n;
            if (priceData[j].close >= 0.01 * (100 + p) * ama[k])
            {
                if (position < x)
                {
                    position++;
                    psedu_position++;
                    flag1 = true;
                    total -= priceData[j].close;

                    stored_stock s;
                    s.price = priceData[j].close;
                    s.age = 0;
                    stocks_bought.push_back(s);
                }
            }

            if (priceData[j].close <= 0.01 * (100 - p) * ama[k])
            {
                if (position > -x)
                {
                    position--;
                    psedu_position--;
                    flag2 = true;
                    total += priceData[j].close;
                    // if (priceData[j].date == "2023-01-18")
                    // {
                    //     // cout << "yay 18" << endl;
                    // }
                    stored_stock s;
                    s.price = priceData[j].close;
                    s.age = 0;
                    stocks_sold.push_back(s);
                }
            }

            if (!stocks_bought.empty())
            {
                for (auto it = stocks_bought.begin(); it != stocks_bought.end(); ++it)
                {
                    if (it != std::prev(stocks_bought.end()))
                    {
                        it->age++;
                    }
                }
                if (flag1 == false)
                {
                    prev(stocks_bought.end())->age++;
                }

                // cout << priceData[j].date << " "
                //      << "stock bought front age: " << stocks_bought.front().age << endl;
                if (stocks_bought.front().age == max_hold_days)
                {
                    // Sell the earliest stock in stocks_bought
                    if (position > -x)
                    {
                        position--;
                        psedu_position--;
                        total += priceData[j].close;
                        stocks_bought.pop_front();
                    }
                }
            }

            if (!stocks_sold.empty())
            {
                for (auto it = stocks_sold.begin(); it != stocks_sold.end(); ++it)
                {
                    if (it != std::prev(stocks_sold.end()))
                    {
                        it->age++;
                    }
                }
                if (flag2 == false)
                {
                    prev(stocks_sold.end())->age++;
                }

                // cout << priceData[j].date << " "
                //      << "stock sold front age: " << stocks_sold.front().age << endl;
                if (stocks_sold.front().age == max_hold_days)
                {
                    // cout << "front age" << stocks_sold.front().age << endl;
                    // cout << "front price" << stocks_sold.front().price << endl;
                    // Buy the earliest stock in stocks_sold
                    if (position < x)
                    {
                        // if (priceData[j].date == "2023-01-18")
                        // {
                        //     cout << "damn2" << endl;
                        // }
                        position++;
                        psedu_position++;
                        total -= priceData[j].close;
                        stocks_sold.pop_front();
                    }
                }
            }
            if (psedu_position > 0)
            {
                write_to_order_statistics_csv(priceData, j, psedu_position, "BUY", actions_file);
            }
            else if (psedu_position < 0)
            {
                write_to_order_statistics_csv(priceData, j, -1 * psedu_position, "SELL", actions_file);
            }
            write_to_daily_cashflow_csv(priceData, j, total, cashflow_file, ama, sf, n);
            //cout << priceData[j].date << "  " << position << endl;
        }
    }
    // In case of square-off at end_date
    cout << "DMA++ Actions written to " << actions_file << endl;
    cout << "DMA++ Timely square off" << endl;
    square_off(priceData, total, position, pnl_file);
}
