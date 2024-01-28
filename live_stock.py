from datetime import date
from jugaad_data.nse import stock_csv, stock_df,derivatives_csv, derivatives_df, index_df, index_csv
from jugaad_data.nse import NSELive
import pandas as pd

# Function to return stock data
def fetch_historic_stock_data(Symbol, start_date, end_date):
    df = stock_df(symbol= Symbol, from_date=start_date, to_date=end_date, series="EQ")
    list_of_dicts = df.to_dict(orient='records')
    # stock_csv(symbol=Symbol, from_date=start_date,to_date=end_date, output="index_scv.csv")
    return list_of_dicts

def fetch_historic_index_data(Symbol, start_date, end_date):
    df = stock_df(symbol= Symbol, from_date=start_date, to_date=end_date, series="EQ")
    list_of_dicts = df.to_dict(orient='records')
    # index_csv(symbol=Symbol, from_date=start_date,to_date=end_date, output="index_scv.csv")
    return list_of_dicts

# print(fetch_historic_stock_data("SBIN", date(2020, 1, 1), date(2020, 1, 30)))

output_path = "historic_stock_futures_data.csv"
def fetch_historic_stock_futures(Symbol, start_date, end_date, Expiry_date):
    df = derivatives_df(symbol=Symbol, from_date=start_date, to_date=end_date, 
                        expiry_date= Expiry_date, instrument_type="FUTSTK")
    df.to_csv(output_path, index=False)
    # list_of_dicts = df.to_dict(orient = 'records')
    # return list_of_dicts
print(fetch_historic_stock_futures("SBIN", date(2020, 1, 1), date(2020, 1, 30), date(2022, 1, 30)))

def fetch_historic_stock_options(Symbol, start_date, end_date, Expiry_date, Strike_price):
    df = derivatives_df(symbol=Symbol, from_date=start_date, to_date=end_date, 
                        expiry_date= Expiry_date, instrument_type="OPTSTK",option_type = "CE", strike_price=Strike_price)
    list_of_dicts = df.to_dict(orient = 'records')
    return list_of_dicts
# print(fetch_historic_stock_options("SBIN", date(2020, 1, 1), date(2020, 1, 30),date(2022, 1, 30),300))

def fetch_historic_index_futures(Symbol, start_date, end_date, Expiry_date):
    df = derivatives_df(symbol=Symbol, from_date=start_date, to_date=end_date, 
                        expiry_date= Expiry_date, instrument_type="FUTIDX")
    list_of_dicts = df.to_dict(orient = 'records')
    return list_of_dicts
# print(fetch_historic_index_futures("SBIN", date(2020, 1, 1), date(2020, 1, 30),date(2022, 1, 30)))

def fetch_historic_index_options(Symbol, start_date, end_date, Expiry_date, Strike_price):
    df = derivatives_df(symbol=Symbol, from_date=start_date, to_date=end_date, 
                        expiry_date= Expiry_date, instrument_type="FUTSTK", option_type = "CE", strike_price=Strike_price)
    list_of_dicts = df.to_dict(orient = 'records')
    return list_of_dicts
# print(fetch_historic_stock_options("SBIN", date(2020, 1, 1), date(2020, 1, 30),date(2022, 1, 30),300))
#Live data
n = NSELive()
status = n.market_status()
all_indices = n.all_indices()

def fetch_live_stock_data():
    q = n.stock_quote("HDFC")
    return q['priceinfo']
#print fetch_live_stock_data("HDFC")