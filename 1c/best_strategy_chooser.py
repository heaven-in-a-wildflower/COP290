import os

# List of files to check for
files_to_check = [
    "final_pnl_basic.txt",
    "final_pnl_dma.txt",
    "final_pnl_dma++.txt",
    "final_pnl_macd.txt",
    "final_pnl_rsi.txt",
    "final_pnl_adx.txt",
    "final_pnl_lr.txt"
]

# Dictionary to store the values read from files
file_values = {}

# Read values from files
for filename in files_to_check:
    if os.path.exists(filename):
        with open(filename, "r") as file:
            try:
                value = float(file.read().strip())
                file_values[filename] = value
            except ValueError:
                print(f"Error: {filename} does not contain a valid floating-point number.")

# Find the file with the highest value
if file_values:
    highest_value_file = max(file_values, key=file_values.get)
    highest_value = file_values[highest_value_file]

    # Retain the file with the highest value and delete others
    for filename, value in file_values.items():
        if filename != highest_value_file:
            os.remove(filename)

    print(f"The file '{highest_value_file}' with value {highest_value} is retained.")
    
    # Rename the file with the highest value to final_pnl.txt
    highest_suffix = highest_value_file.split("_")[2].split(".")[0]
    old_order_stats = f"order_statistics_{highest_suffix}.csv"
    old_daily_cashflow = f"daily_cashflow_{highest_suffix}.csv"
    os.rename(highest_value_file, f"final_pnl.txt")
    os.rename(old_order_stats, f"order_statistics.csv")
    os.rename(old_daily_cashflow, f"daily_cashflow.csv")
    
    
    # Retain corresponding order statistics and daily cashflow files
    suffixes = set()
    for filename in files_to_check:
        suffixes.add(filename.split("_")[2].split(".")[0])
    
    suffixes.remove(highest_suffix)
    
    for suffix in suffixes:
        order_statistics_file = f"order_statistics_{suffix}.csv"
        daily_cashflow_file = f"daily_cashflow_{suffix}.csv"
        pnl_file = f"final_pnl_{suffix}.txt"
        
        if os.path.exists(order_statistics_file):
            os.remove(order_statistics_file)
            print(f"Deleted {order_statistics_file}")
        if os.path.exists(daily_cashflow_file):
            os.remove(daily_cashflow_file)
            print(f"Deleted {daily_cashflow_file}")
        if os.path.exists(pnl_file):
            os.remove(pnl_file)
            print(f"Deleted {pnl_file}")
else:
    print("No valid files found.")
