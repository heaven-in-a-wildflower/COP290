from flask import Flask, request, jsonify
from datetime import datetime
from jugaad_data.nse import stock_df, NSELive
from flask_cors import CORS
import yfinance as yf
import json

app = Flask(__name__)
CORS(app, resources={r"/api/*": {"origins": "*"}})
    
#Function to extract stock data to be plotted
def extract_historic_data(Symbol, time_period, frequency):
    try:
        Symbol_ns = f"{Symbol}.NS"
        df = yf.download(Symbol_ns, period=time_period, interval=frequency)
        # Convert Timestamp index to string
        df.index = df.index.astype(str)
        # Convert DataFrame to dictionary with 'index' orientation
        df_dict = df.to_dict(orient='index')
        return json.dumps(df_dict, indent=4)
    except Exception as e:
        print(f"Failed to download {Symbol_ns}: {str(e)}")
        return json.dumps({})
    
#Decorator function to extract stock data for different time intervals
def decorator(Symbol, time_period):
    if time_period == '1d':
        return extract_historic_data(Symbol, '1d', '1m')
    elif time_period == '1w':
        return extract_historic_data(Symbol, '1wk', '5m')
    elif time_period == '1m':
        return extract_historic_data(Symbol, '1mo', '1h')
    elif time_period == '3m':
        return extract_historic_data(Symbol, '3mo', '1d')
    elif time_period == '6m':
        return extract_historic_data(Symbol, '6mo', '1d')
    elif time_period == '1y':
        return extract_historic_data(Symbol, '1y', '1d')
    elif time_period == '5y':
        return extract_historic_data(Symbol, '5y', '5d')
    elif time_period == '10y':
        return extract_historic_data(Symbol, '10y', '5d')
    else:
        return extract_historic_data(Symbol, '1mo', '1d')

#Function to fetch the top five gainers and losers based on percent change in stock value
def top_five_gainers_losers():
    n = NSELive()
    stonks = ['ADANIENT', 'ADANIPORTS', 'APOLLOHOSP', 'ASIANPAINT', 'AXISBANK', 'BAJAJ-AUTO', 'BAJFINANCE', 'BAJAJFINSV', 'BPCL', 'BHARTIARTL', 'BRITANNIA', 'CIPLA', 'COALINDIA', 'DIVISLAB', 'DRREDDY', 'EICHERMOT', 'GRASIM', 'HCLTECH', 'HDFCBANK', 'HDFCLIFE', 'HEROMOTOCO', 'HINDALCO', 'HINDUNILVR', 'ICICIBANK', 'ITC', 'INDUSINDBK', 'INFY', 'JSWSTEEL', 'KOTAKBANK', 'LTIM', 'LT', 'MARUTI', 'NTPC', 'NESTLEIND', 'ONGC', 'POWERGRID', 'RELIANCE', 'SBILIFE', 'SBIN', 'SUNPHARMA', 'TCS', 'TATACONSUM', 'TATAMOTORS', 'TATASTEEL', 'TECHM', 'TITAN', 'UPL', 'ULTRACEMCO', 'WIPRO']
    result = []
    for stock in stonks:
        q = n.stock_quote(stock)
        price_info = q.get('priceInfo', {})
        pChange = price_info.get('pChange')
        result.append({'stock': stock, 'pChange': pChange})

    sorted_stocks = sorted(result, key=lambda x: x['pChange'] or 0, reverse=True)
    top_five_winners = sorted_stocks[:5]
    top_five_losers = sorted_stocks[-5:][::-1]
    return top_five_winners, top_five_losers

#Function to extract data of main nifty sector indices
def extracted_index_data(index_symbol):
    n = NSELive()
    nifty = n.live_index(index_symbol)
    data = nifty['metadata']
    keys_to_extract = ['last', 'percChange']
    extracted_data = {key: data[key] for key in keys_to_extract if key in data}
    extracted_data['indexName'] = index_symbol  # Add index name to the extracted data
    return extracted_data

#Function to fetch extra information to be displayed alongside graph
def extra_stock_info(stock_symbol):
    stock_symbol_ns = f"{stock_symbol}.NS"
    stock_data = yf.Ticker(stock_symbol_ns).info
    keys_mapping = {
        'previousClose': 'Previous Close',
        'open': 'Open',
        'dayLow': 'Day Low',
        'dayHigh': 'Day High',
        'dividendRate': 'Dividend Rate',
        'dividendYield': 'Dividend Yield',
        'payoutRatio': 'Payout Ratio',
        'marketCap': 'Market Cap',
        'debtToEquity': 'Debt To Equity Ratio',
        'revenuePerShare': 'Revenue Per Share',
        'returnOnAssets': 'Return On Assets',
        'returnOnEquity': 'Return On Equity',
        'earningsGrowth': 'Earnings Growth',
        'revenueGrowth': 'Revenue Growth',
        'grossMargins': 'Gross Margins',
        'pegRatio': 'PEG Ratio',
        'trailingPE':'P/E Ratio'
    }
    
    # Extract key-value pairs for the specified keys
    extracted_data = {keys_mapping[key]: stock_data.get(key, None) for key in keys_mapping}
    
    return extracted_data

#List of stocks
stock_list = [ 
    'ADANIENT', 'ADANIPORTS', 'APOLLOHOSP', 'ASIANPAINT', 'AXISBANK', 'BAJAJ-AUTO', 'BAJFINANCE',
    'BAJAJFINSV', 'BPCL', 'BHARTIARTL', 'BRITANNIA', 'CIPLA', 'COALINDIA', 'DIVISLAB', 'DRREDDY',
    'EICHERMOT', 'GRASIM', 'HCLTECH', 'HDFCBANK', 'HDFCLIFE', 'HEROMOTOCO', 'HINDALCO', 'HINDUNILVR',
    'ICICIBANK', 'ITC', 'INDUSINDBK', 'INFY', 'JSWSTEEL', 'KOTAKBANK', 'LTIM', 'LT', 'MARUTI', 'NTPC',
    'NESTLEIND', 'ONGC', 'POWERGRID', 'RELIANCE', 'SBILIFE', 'SBIN', 'SUNPHARMA', 'TCS', 'TATACONSUM',
    'TATAMOTORS', 'TATASTEEL', 'TECHM', 'TITAN', 'UPL', 'ULTRACEMCO', 'WIPRO'
]

#Function to fetch values of specific filtering criteria
def apply_filters(stock_symbol, filter_key):
    stock_symbol_ns = f"{stock_symbol}.NS"
    stock_data = yf.Ticker(stock_symbol_ns).info
    filters = [
        'marketCap', 'debtToEquity', 'revenuePerShare', 'returnOnAssets',
        'returnOnEquity', 'earningsGrowth', 'revenueGrowth', 'grossMargins', 'pegRatio','trailingPE']

    if filter_key not in filters:
        raise ValueError(f"Invalid filter key. Available filters: {', '.join(filters)}")

    # Check if data for the filter is available
    if filter_key in stock_data:
        # Extract key-value pair for the specified key
        extracted_data = {filter_key: stock_data[filter_key]}
        return extracted_data
    else:
        # print(f"Data not available for filter '{filter_key}' for stock '{stock_symbol}'")
        return {}

#Function to rank stocks based on filter
def rank_stocks(filter_key):
    stock_ranks = {}
    for stock_symbol in stock_list:
        try:
            stock_info = apply_filters(stock_symbol, filter_key)
            if stock_info:
                stock_ranks[stock_symbol] = stock_info[filter_key]
        except ValueError as e:
            print(f"Skipping {stock_symbol}: {e}")

    # Sort stocks based on the specified filter in descending order
    sorted_stocks = sorted(stock_ranks.items(), key=lambda x: x[1], reverse=True)

    # Get the top 10 stocks
    top_10_stocks = dict(sorted_stocks[:10])
    return json.dumps(top_10_stocks, indent=4)

#Routes
@app.route('/api/stock_info', methods=['GET'])
def get_stock_info():
    stock_symbol = request.args.get('symbol')
    if not stock_symbol:
        return jsonify({'error': 'Missing stock symbol'}), 400

    try:
        stock_info = extra_stock_info(stock_symbol)
        return jsonify(stock_info), 200
    except Exception as e:
        return jsonify({'error': 'Failed to fetch stock info', 'details': str(e)}), 500
    
@app.route('/api/rank_stocks', methods=['GET'])
def get_ranked_stocks():
    filter_key = request.args.get('filter_key')
    if not filter_key:
        return jsonify({'error': 'Filter key is required'}), 400
    try:
        ranked_stocks = rank_stocks(filter_key)
        return ranked_stocks
    except ValueError as e:
        return jsonify({'error': str(e)}), 400

@app.route('/api/gainers-losers')
def gainers_and_losers():
    winners, losers = top_five_gainers_losers()
    return jsonify({"winners": winners, "losers": losers})

@app.route('/api/nse_data', methods=['GET'])
def get_nse_data():
    main_indices = ["NIFTY 50", "NIFTY BANK", "NIFTY IT", "NIFTY FMCG", "NIFTY PHARMA"]
    final_list = [extracted_index_data(index) for index in main_indices]
    return jsonify(final_list)

@app.route('/api/stock_data', methods=['GET'])
def get_stock_data():
    symbol = request.args.get('symbol')
    time_period = request.args.get('time_period')
    data_json = decorator(symbol, time_period)
    
    return jsonify(data_json)

if __name__ == '__main__':
    app.run(debug=True)
