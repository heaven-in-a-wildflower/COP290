import json
from jugaad_data.nse import NSELive

n = NSELive()
all_indices = n.all_indices()

def write_to_json(data, filename):
    with open(filename, 'w') as json_file:
        json.dump(data, json_file, indent=4)

def fetch_main_indices():
    data_list = []
    for idx in all_indices['data']:
        if idx['index'] in ['NIFTY 50', 'NIFTY NEXT 50', 'NIFTY BANK']:
            data_list.append({'index': idx['index'], 'last': idx['last'], 'percentChange': idx['percentChange']})
    
    write_to_json(data_list, 'main_indices.json')
    print('Data written to main_indices.json')

def fetch_all_indices():
    data_list = []
    for idx in all_indices['data']:
        data_list.append({'index': idx['index'], 'last': idx['last'], 'percentChange': idx['percentChange']})
    
    write_to_json(data_list, 'all_indices.json')
    print('Data written to all_indices.json')

# Call the functions to fetch data and write to JSON files
fetch_main_indices()
fetch_all_indices()
