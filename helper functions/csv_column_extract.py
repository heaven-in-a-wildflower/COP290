import csv
import json

def extract_columns(csv_file_path, columns):
    extracted_data = []

    with open(csv_file_path, 'r') as file:
        csv_reader = csv.reader(file)
        
        # Assuming the first row contains headers, skip it
        next(csv_reader)
        
        for row in csv_reader:
            # Extract data from specified columns
            extracted_row = {'first_column': row[0], 'third_column': row[2]}
            extracted_data.append(extracted_row)

    return extracted_data

# Replace 'your_file.csv' with the actual path to your CSV file
csv_file_path = 'ind_nifty50list.csv'
result = extract_columns(csv_file_path, columns=[0, 2])

# Replace 'output.json' with the desired name for the JSON file
output_json_path = 'output.json'
with open(output_json_path, 'w') as output_json_file:
    json.dump(result, output_json_file, indent=2)

print(f'Data written to {output_json_path}')
