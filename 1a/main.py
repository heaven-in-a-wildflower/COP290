from datetime import date,datetime
from dateutil.relativedelta import relativedelta
import numpy as np
import pandas as pd
from jugaad_data.nse import stock_df
import os
import time
import matplotlib.pyplot as plt
import zipfile

import json
import pyarrow as pa
import pyarrow.parquet as pq
import pyarrow.orc as orc
import pyarrow.feather as feather
import tabulate
import sys

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 SYMBOL num_years")
        sys.exit(1)

    symbol = sys.argv[1]
    time_period = int(sys.argv[2])

    # Calculate the start and end dates based on the time period
    end_date = datetime.today().date()
    start_date = end_date - relativedelta(years=time_period)

    # Get the directory of the current script
    current_dir = os.path.dirname(os.path.abspath(__file__))

    # Specify the output file paths
    file_extensions = ['txt', 'csv', 'json', 'md', 'parquet','feather','orc','.bin']
    output_paths = [os.path.join(current_dir, f"{symbol}.{ext}") for ext in file_extensions]

    # Clear the contents of the info file
    info_path = os.path.join(current_dir, "info.txt")
    with open(info_path, 'w') as info_file:
        info_file.write(f"")

    # Lists to store data
    writing_times = []
    file_sizes_kb = []

    df = stock_df(symbol=symbol, from_date=start_date, to_date=end_date, series="EQ")
    removal_list = ['PREV. CLOSE' , 'SERIES' , '52W H' , 'SYMBOL', '52W L', 'VWAP']

    data = df.drop(columns=removal_list)
    data_df = pd.DataFrame(data)
    
    def download_and_save(start_date, end_date, output_path):
        try:
            start_time = time.time()

            # Extract file extension
            _, file_extension = os.path.splitext(output_path)

            # Save the DataFrame to different file formats
            if output_path.endswith('.txt'):
                data_df.to_csv(output_path, sep=',', index=False)
            
            elif output_path.endswith('.csv'):
                data_df.to_csv(output_path, index=False)
            
            elif output_path.endswith('.json'):
                data_df.to_json(output_path, orient='records', lines=True)
            
            elif output_path.endswith('.md'):
                data_df.to_markdown(output_path,index=False)
                
            elif output_path.endswith('.parquet'):
                data_df.to_parquet(output_path, index=False)
            
            elif output_path.endswith('.feather'):
                data_df.to_feather(output_path)
            
            elif output_path.endswith('.bin'):
                data_df.to_pickle(output_path)
                        
            # elif output_path.endswith('.log'):
            #     data_df.to_csv(output_path, mode='a', header=False, index=False)
            
            # h5 is too bad
            # elif output_path.endswith('.h5'):
            #     data_df.to_hdata_df(output_path, key='my_key',mode = 'w')
                
            elif output_path.endswith('.orc'):
                table = pa.Table.from_pandas(data_df)
                with pa.OSFile(output_path, 'wb') as orc_file:
                    orc.write_table(table, orc_file)
            else:
                raise ValueError(f"Unsupported file format: {output_path}")

            end_time = time.time()
            file_size = os.path.getsize(os.path.join(current_dir, output_path))/1e3
            elapsed_time = end_time - start_time

            info_path = os.path.join(current_dir, "info.txt")
            writing_times.append(elapsed_time)
            file_sizes_kb.append(file_size)
            
            print(f"Data for {symbol} saved to {output_path}")
        except Exception as e:
            print(f"Error saving data to {output_path}: {e}")

    # Download and save to respective files
    for output_path in output_paths:
        download_and_save(start_date, end_date, output_path)

    # print(f"Historic data for {symbol} has been saved to {symbol} folder in ../hist_data")
            
    # Normalize data
    max_writing_time = max(writing_times)
    max_file_size = max(file_sizes_kb)

    normalized_writing_times = [time / max_writing_time for time in writing_times]
    normalized_file_sizes = [size / max_file_size for size in file_sizes_kb]

    # Graph Layout
    plt.style.use('bmh')
    plt.rcParams.update({'figure.autolayout': True})

    figure_1, ax = plt.subplots()

    mean_writing_time = np.mean(normalized_writing_times)
    mean_file_size = np.mean(normalized_file_sizes)
    bar_width = 0.35

    bar1 = ax.bar(np.arange(len(file_extensions)), normalized_writing_times, bar_width, color='blue', label='Normalized Writing Times')

    bar2 = ax.bar(np.arange(len(file_extensions)) + bar_width, normalized_file_sizes, bar_width, color='orange', label='Normalized File Sizes')

    ax.set_xlabel('File Format')
    ax.set_ylabel('Normalized Values')
    ax.set_title('Double Bar Graph - Normalized Writing Times and File Sizes')
    ax.set_xticks(np.arange(len(file_extensions)) + bar_width / 2)
    ax.set_xticklabels(file_extensions)
    ax.legend()

    # Annotate 
    for i, rect in enumerate(bar1):
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width() / 2, height, f'{height:.2f}', ha='center', va='bottom')

    for i, rect in enumerate(bar2):
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width() / 2, height, f'{height:.2f}', ha='center', va='bottom')

    figure_1.savefig(os.path.join(current_dir, 'figure_1.png'))
    print('Saved figure_1')

    #*******************************************************************************************************
    read_times = []

    def txt_md_read_time(output_path):
        start_time = time.time()
        # with open(output_path, 'r') as file:
        #     content = file.read()
        data_df = pd.read_csv(output_path)
        end_time = time.time()
        time_taken = end_time - start_time
        return time_taken

    def csv_read_time(output_path):
        start_time = time.time()
        data_df = pd.read_csv(output_path)
        end_time = time.time()
        time_taken = end_time - start_time
        return time_taken

    def json_read_time(output_path):
        start_time = time.time()
        data_list = []
        with open(output_path, 'r') as file:
            for line in file:
                data = json.loads(line)
                data_list.append(data)
        end_time = time.time()
        time_taken = end_time - start_time
        return time_taken

    def parquet_read_time(output_path):
        start_time = time.time()
        table = pq.read_table(output_path)
        # data_df = table.to_pandas()
        end_time = time.time()
        time_taken = end_time - start_time 
        return time_taken
        
    def feather_read_time(output_path):
        start_time = time.time()
        data_df = feather.read_feather(output_path)
        end_time = time.time()
        time_taken = end_time - start_time 
        return time_taken

    def bin_read_time(output_path):
        start_time = time.time()
        data_df = pd.read_pickle(output_path)
        end_time = time.time()
        time_taken = end_time - start_time 
        return time_taken
        

    # def yaml_read_time(output_path):
    #     start_time = time.time()
    #     data_df = pd.read_csv(output_path)  # Assuming CSV format, adjust if needed
    #     data_dict = data_df.to_dict(orient='records')
    #     with open(output_path, 'w') as file:
    #         yaml.dump(data_dict, file)
    #     end_time = time.time()
    #     time_taken = end_time - start_time
    #     return time_taken

    def orc_read_time(output_path):
        start_time = time.time()
        with open(output_path, 'rb') as file:
            table = orc.read_table(file)
        # data_df = table.to_pandas()
        end_time = time.time()
        time_taken = end_time - start_time
        return time_taken

    def get_file_extension(file_path):
        _, file_extension = os.path.splitext(file_path)
        return file_extension[1:]

    def measure_read_time(output_path):
        if output_path.endswith(('.txt', '.md')):
            t = txt_md_read_time(output_path)
        else:
            ext = get_file_extension(output_path)
            function_name = f"{ext}_read_time"
            t = globals().get(function_name, lambda _: None)(output_path)
        return t

    # Loop through each file extension
    for ext in file_extensions:
        # Construct the file path
        file_path = os.path.join(current_dir, f'{symbol}.{ext}')
        elapsed_time = measure_read_time(file_path)
        read_times.append(elapsed_time)

    # Graph 2
    figure_2, ax = plt.subplots()
    mean_read_time = np.mean(read_times)
    bar_width = 0.35

    ax.axhline(mean_read_time, ls='--', color='r', label=f'Mean Time: {mean_read_time:.4f} seconds')
    ax.bar(file_extensions, read_times, width=bar_width, color='blue')
    ax.set_xlabel('File Format')
    ax.set_ylabel('Time Taken (seconds)')
    ax.set_title('Read Times')
    ax.legend()

    # Save the third figure
    figure_2.savefig(os.path.join(current_dir, 'figure_2.png'))
    print('Saved figure_3')
    #*******************************************************************************************************
    def decompress_file(zip_path, output_path):
        start_time = time.time()

        # Decompress the zip archive
        with zipfile.ZipFile(zip_path, 'r') as zipf:
            zipf.extractall(output_path)

        end_time = time.time()

        # Calculate the elapsed time
        decompression_time = end_time - start_time
        print(decompression_time)
        return decompression_time

    # Lists to store file formats, decompression times, and compression ratios
    decompression_times = []

    for ext in file_extensions:
        # Construct the file path
        file_path = os.path.join(current_dir, f'{symbol}.{ext}')

        # Construct the zip file path
        zip_path = os.path.join(current_dir, f'{symbol}_{ext}.zip')

        # Compress file
        with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
            zipf.write(file_path, os.path.basename(file_path))
        print(f"Compressed {file_path} to {zip_path}")
        
        # Construct the decompressed file path
        unzip_path = os.path.join(current_dir, f'{symbol}_{ext}_unzip')

        decompression_time = decompress_file(zip_path, unzip_path)

        # Append data to lists
        decompression_times.append(decompression_time)


    # Graph 3
    figure_3, ax = plt.subplots()
    mean_decompression_time = np.mean(decompression_times)
    bar_width = 0.35

    ax.axhline(mean_decompression_time, ls='--', color='r', label=f'Mean Time: {mean_decompression_time:.5f} seconds')
    ax.bar(file_extensions, decompression_times, width=bar_width, color='blue')
    ax.set_xlabel('File Format')
    ax.set_ylabel('Decompression Time (seconds)')
    ax.set_title('Decompression Times')
    ax.legend()

    figure_3.savefig(os.path.join(current_dir, 'figure_4.png'))
    print('Saved figure_3')

    #Display the plots
    #plt.tight_layout()
    #plt.show()
if __name__ == "__main__":
    main()

    #*******************************************************************************************************
