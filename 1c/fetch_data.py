import sys
from datetime import datetime, timedelta
from jugaad_data.nse import stock_df
import pandas as pd

def fetch_data(symbol, start_date, end_date, str_n, misc):
    n = int(str_n)
    # Convert string dates to datetime objects with "dd/mm/yyyy" format
    start_date_obj = datetime.strptime(start_date, '%d/%m/%Y')
    actual_start_date_obj = start_date_obj - timedelta(days=2 * n)
    end_date_obj = datetime.strptime(end_date, '%d/%m/%Y')
    
    # Fetch the dataframe containing the data
    df = stock_df(symbol=symbol, from_date=actual_start_date_obj,
                  to_date=end_date_obj, series="EQ")
    
    # Reverse the order of rows and reset index. If indices are not reset,then it would lead to a bug as rows would get reversed but the row-indices would be same.
    reversed_df = df.iloc[::-1].reset_index(drop=True)
    
    # Find the index of the given date or the next greater date if start_date is absent
    
    #Pattern to be searched is in yyyy-mm-dd format.    
    start_date_pattern = start_date_obj.strftime('%Y-%m-%d')
    greater_than_equal_date = reversed_df['DATE'][pd.to_datetime(reversed_df['DATE']) >= start_date_pattern].min()
    start_date_index = reversed_df.index[reversed_df['DATE'] == greater_than_equal_date][0]

    # Calculate the index range for fetching n entries before begin_date
    begin_index = start_date_index - n
    end_index = len(reversed_df)  # Set end_index to the length of the dataframe

    # Extract the desired subset of the dataframe
    selected_data = reversed_df.iloc[begin_index:end_index]

    # Write the selected data to a CSV file
    if misc == "adx":  # Corrected the condition here
        selected_columns = selected_data.iloc[:, [0,3,4,5,7]]
    elif misc == "lr":
        selected_columns = selected_data.iloc[:, [0,2,3,4,7,8,13]]
    else:
        selected_columns = selected_data.iloc[:, [0, 7]]
    
    if misc == "pair1":
        selected_columns.to_csv('prices1.csv', index=False)
    elif misc == "pair2":
        selected_columns.to_csv('prices2.csv', index=False)
    else:
        selected_columns.to_csv('prices.csv', index=False)

def main():

    # Extract start_date and end_date from command-line arguments
    symbol = sys.argv[1]
    start_date = sys.argv[2]
    end_date = sys.argv[3]
    str_n = sys.argv[4]
    misc = sys.argv[5]

    try:
        fetch_data(symbol, start_date, end_date, str_n, misc)
        # print("Data written to csv file")

    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
