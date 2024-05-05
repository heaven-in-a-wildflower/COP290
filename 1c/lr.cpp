#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <sstream>
#include <string>
using namespace std;
namespace fs = std::filesystem;
string filename;

string oneyearless (string s) {
    int n = stoi(s.substr(6,4));
    n--;
    return s.substr(0,6) + to_string(n);
}

struct PriceData
{
    std::string date;
    double open, close, low, high, number, vwap;
};

struct DataPoint
{
    vector<double> input;
    double output;
};

vector<DataPoint> converter(vector<PriceData> d)
{
    int n = d.size();
    vector<DataPoint> res(n - 1);
    for (int i = 1; i < n; i++)
    {
        res[i - 1].output = d[i].close;
        vector<double> a = {double(1),d[i-1].close,d[i-1].open,d[i-1].vwap,d[i-1].low,d[i-1].high,d[i-1].number,d[i].open};
        res[i-1].input = a;
    }
    return res;
}

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

void inverter(vector<vector<double>> &m)
{
    float a[20][20] = {0},d;
    int n = 8;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            a[i][j] = m[i-1][j-1];
        }
    }    
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= 2 * n; j++) {
            if (j == (i + n)) {
                a[i][j] = 1;
            }
        }
    }
    for (int i = n; i > 1; i--) {
        if (a[i-1][1] < a[i][1]) {
            for(int j = 1; j <= n * 2; j++) {
                d = a[i][j];
                a[i][j] = a[i-1][j];
                a[i-1][j] = d;
            }
        }
    }
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n * 2; j++) {
            if (j != i) {
                d = a[j][i] / a[i][i];
                for (int k = 1; k <= n * 2; k++) {
                    a[j][k] = a[j][k] - (a[i][k] * d);
                }
            }
        }
    }
    for (int i = 1; i <= n; i++) {
        d=a[i][i];
        for (int j = 1; j <= n * 2; j++) {
            a[i][j] = a[i][j] / d;
        }
    }
    for (int i = 1; i <= n; i++) {
        for (int j = n + 1; j <= n * 2; j++) {
            m[i-1][j-n-1] = a[i][j];
        }
    }
}

vector<double> trainer(vector<vector<double>> datax, vector<double> datay) {
    for (auto x : datax) {
        for (auto y : x) {
            //cout<<y<<" ";
        }
        //cout<<"\n";
    }
    vector<double> meow(8,0), x_y(8,0), res(8,0);
    vector<vector<double>> x_x;
    for (int i = 0; i < 8; i++) {
        x_x.push_back(meow);
    }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            for (int k = 0; k < datax.size(); k++) {
                x_x[i][j]+=datax[k][i]*datax[k][j];
            }
            // cout<<x_x[i][j]<<" ";
        }
        // cout<<"\n";
    }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < datax.size(); j++) {
            x_y[i]+=datay[j]*datax[j][i];
        }
        // cout<<x_y[i]<<"\n";
    }
    inverter(x_x);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            res[i]+=x_x[i][j]*x_y[j];
        }
    }
    return res;
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
    outputFile << entry.date << "," << direction << "," << std::to_string(std::abs(quantity)) << "," << entry.close << std::endl;
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
    outputFile << entry.date << "," << std::to_string(cashflow) << std::endl;
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
    std::cout << "LINEAR_REGRESSION Profit/Loss written to " << pnl_file << std::endl;
}

int main(int argc, char *argv[])
{
    // cout<<"wow"<<endl;
    //Fetch environment variable for individual execution:
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
        actions_file = "order_statistics_lr.csv";
        pnl_file = "final_pnl_lr.txt";
        cashflow_file = "daily_cashflow_lr.csv";
    }
    else
    {
        actions_file = "order_statistics.csv";
        pnl_file = "final_pnl.txt";
        cashflow_file = "daily_cashflow.csv";
    }

    // Extract required info from make command
    std::string symbol = argv[1];
    std::string train_start_date, train_end_date, start_date, end_date;
    if (best_of_all_check == false)
    {
        train_start_date = argv[4];
        train_end_date = argv[5];
        start_date = argv[6];
        end_date = argv[7];
    }
    else
    {
        start_date = argv[4];
        end_date = argv[5];
        train_start_date = oneyearless(start_date);
        train_end_date = oneyearless(end_date);
    }
    int x = std::stoi(argv[2]);
    int p = std::stoi(argv[3]);
    int position = 0;

    // Invoke python script to fetch data
    fs::path current_dir = fs::current_path();
    fs::path bin_dir = current_dir / "venv" / "bin";
    std::string command = (bin_dir / "python fetch_data.py").string() + " " +
                          symbol + " " +
                          start_date + " " +
                          end_date + " 0 " + "lr";
    // Execute the command
    int result = system(command.c_str());

    // Parse the csv file to populate the priceData vector
    std::ifstream inputFile("prices.csv");
    std::vector<PriceData> trainData, priceData;
    std::string line;

    // Skip the header line
    std::getline(inputFile, line);

    while (std::getline(inputFile, line))
    {
        std::istringstream iss(line);
        PriceData entry;
        string word;
        getline(iss, word, ',');
        entry.date = word;
        getline(iss, word, ',');
        entry.open = stod(word);
        getline(iss, word, ',');
        entry.high = stod(word);
        getline(iss, word, ',');
        entry.low = stod(word);
        getline(iss, word, ',');
        entry.close = stod(word);
        getline(iss, word, ',');
        entry.vwap = stod(word);
        getline(iss, word, ',');
        entry.number = stod(word);
        priceData.push_back(entry);
    }
    inputFile.close();

    command = (bin_dir / "python fetch_data.py").string() + " " +
                          symbol + " " +
                          train_start_date + " " +
                          train_end_date + " 0 " + "lr";

    result = system(command.c_str());

    inputFile.open("prices.csv");

    std::getline(inputFile, line);

    while (std::getline(inputFile, line))
    {
        std::istringstream iss(line);
        PriceData entry;
        string word;
        getline(iss, word, ',');
        entry.date = word;
        getline(iss, word, ',');
        entry.open = stod(word);
        getline(iss, word, ',');
        entry.high = stod(word);
        getline(iss, word, ',');
        entry.low = stod(word);
        getline(iss, word, ',');
        entry.close = stod(word);
        getline(iss, word, ',');
        entry.vwap = stod(word);
        getline(iss, word, ',');
        entry.number = stod(word);
        trainData.push_back(entry);
    }
    inputFile.close();

    vector<DataPoint> trained = converter(trainData);
    vector<DataPoint> worker = converter(priceData);

    vector<vector<double>> datax;
    vector<double> datay;

    for (int i = 0; i < trained.size(); i++) {
        if (i!=0 && trained[i].input[1]==trained[i-1].input[1]) {
            continue;
        }
        else {
            datax.push_back(trained[i].input);
            datay.push_back(trained[i].output);
        }
    }

    vector<double> coefficents = trainer(datax, datay);

    double total = 0;

    // open,close,low,high,number,vwap

    for (int j = 0; j < priceData.size(); j++)
    {
        double expectation = 0;
        if (j != 0)
        {
            for (int i = 0; i < 8; i++)
            {
                expectation += worker[j - 1].input[i] * coefficents[i];
            }
            if (worker[j - 1].output >= expectation * (1 + p / 100) && position < x)
            {
                position++;
                total -= worker[j - 1].output;
                write_to_order_statistics_csv(priceData, j, 1, "BUY", actions_file);
            }
            else if (worker[j - 1].output <= expectation * (1 - p / 100) && position > -x)
            {
                position--;
                total += worker[j - 1].output;
                write_to_order_statistics_csv(priceData, j, 1, "SELL", actions_file);
            }
        }
        write_to_daily_cashflow_csv(priceData, j, total, cashflow_file);
    }

    // In case of square-off at end_date
    cout << "LINEAR_REGRESSION Timely square off" << endl;
    cout << "LINEAR_REGRESSION Actions written to " << actions_file << endl;
    square_off(priceData, total, position, pnl_file);
}