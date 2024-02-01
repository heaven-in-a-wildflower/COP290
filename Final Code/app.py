from flask import Flask, request, jsonify
from datetime import datetime
from dateutil.relativedelta import relativedelta
from jugaad_data.nse import stock_df, NSELive
from flask_cors import CORS
import yfinance as yf
import json

app = Flask(__name__)
CORS(app, resources={r"/api/*": {"origins": "*"}})

def adjust_start_date(time_range):
    end_date = datetime.today().date()
    if time_range == '1w':
        return end_date - relativedelta(weeks=1)
    elif time_range == '1m':
        return end_date - relativedelta(months=1)
    elif time_range == '3m':
        return end_date - relativedelta(months=3)
    elif time_range == '6m':
        return end_date - relativedelta(months=6)
    elif time_range == '1y':
        return end_date - relativedelta(years=1)
    elif time_range == '5y':
        return end_date - relativedelta(years=5)
    elif time_range == '10y':
        return end_date - relativedelta(years=10)
    elif time_range == 'all':
        return datetime(1996, 4, 22).date()
    else:
        return end_date - relativedelta(days=7)  # Default to 1 week
    
def extract_volume_data(Symbol, time_period, frequency):
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
    
def decorator(Symbol, time_period):
    if time_period == '1d':
        return extract_volume_data(Symbol, '1d', '1h')
    elif time_period == '1w':
        return extract_volume_data(Symbol, '1d', '5m')
    elif time_period == '1m':
        return extract_volume_data(Symbol, '1mo', '1h')
    elif time_period == '3m':
        return extract_volume_data(Symbol, '3mo', '1d')
    elif time_period == '6m':
        return extract_volume_data(Symbol, '6mo', '1d')
    elif time_period == '1y':
        return extract_volume_data(Symbol, '1y', '1d')
    elif time_period == '5y':
        return extract_volume_data(Symbol, '5y', '1d')
    elif time_period == '10y':
        return extract_volume_data(Symbol, '10y', '1d')
    else:
        return extract_volume_data(Symbol, '1mo', '1d')

def fetch_historic_stock_data(symbol, start_date, end_date):
    df = stock_df(symbol=symbol, from_date=start_date, to_date=end_date, series="EQ")
    return df.to_dict(orient='records')

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

def fetch_main_indices():
    n = NSELive()
    result = []
    index_list = ["NIFTY 50", "NIFTY BANK", "NIFTY NEXT 50"]

    for index in index_list:
        index_data = n.live_index(index)
        market_status = index_data.get('marketStatus', {})
        
        p_change = market_status.get('pChange')
        last_price = market_status.get('lastPrice')

        result.append({
            'index': index,
            'pChange': p_change,
            'lastPrice': last_price
        })

    return result

def extracted_index_data(index_symbol):
    n = NSELive()
    nifty = n.live_index(index_symbol)
    data = nifty['metadata']
    keys_to_extract = ['last', 'percChange']
    extracted_data = {key: data[key] for key in keys_to_extract if key in data}
    extracted_data['indexName'] = index_symbol  # Add index name to the extracted data
    return extracted_data

def extra_stock_info(stock_symbol):
    stock_symbol_ns = f"{stock_symbol}.NS"
    stock_data = yf.Ticker(stock_symbol_ns).info
    keys_to_extract = [
        'previousClose', 'open', 'dayLow', 'dayHigh', 'dividendRate', 'dividendYield',
        'payoutRatio', 'marketCap', 'debtToEquity', 'revenuePerShare', 'returnOnAssets',
        'returnOnEquity', 'earningsGrowth', 'revenueGrowth', 'grossMargins', 'pegRatio'
    ]
    # Extract key-value pairs for the specified keys
    extracted_data = {key: stock_data.get(key, None) for key in keys_to_extract}
    return extracted_data

stock_list = [ 
    'ADANIENT', 'ADANIPORTS', 'APOLLOHOSP', 'ASIANPAINT', 'AXISBANK', 'BAJAJ-AUTO', 'BAJFINANCE',
    'BAJAJFINSV', 'BPCL', 'BHARTIARTL', 'BRITANNIA', 'CIPLA', 'COALINDIA', 'DIVISLAB', 'DRREDDY',
    'EICHERMOT', 'GRASIM', 'HCLTECH', 'HDFCBANK', 'HDFCLIFE', 'HEROMOTOCO', 'HINDALCO', 'HINDUNILVR',
    'ICICIBANK', 'ITC', 'INDUSINDBK', 'INFY', 'JSWSTEEL', 'KOTAKBANK', 'LTIM', 'LT', 'MARUTI', 'NTPC',
    'NESTLEIND', 'ONGC', 'POWERGRID', 'RELIANCE', 'SBILIFE', 'SBIN', 'SUNPHARMA', 'TCS', 'TATACONSUM',
    'TATAMOTORS', 'TATASTEEL', 'TECHM', 'TITAN', 'UPL', 'ULTRACEMCO', 'WIPRO'
]

def extra_stock_info(stock_symbol, filter_key):
    stock_symbol_ns = f"{stock_symbol}.NS"
    stock_data = yf.Ticker(stock_symbol_ns).info
    filters = [
        'marketCap', 'debtToEquity', 'revenuePerShare', 'returnOnAssets',
        'returnOnEquity', 'earningsGrowth', 'revenueGrowth', 'grossMargins', 'pegRatio']

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

def rank_stocks(filter_key):
    stock_ranks = {}
    for stock_symbol in stock_list:
        try:
            stock_info = extra_stock_info(stock_symbol, filter_key)
            if stock_info:
                stock_ranks[stock_symbol] = stock_info[filter_key]
        except ValueError as e:
            print(f"Skipping {stock_symbol}: {e}")

    # Sort stocks based on the specified filter in descending order
    sorted_stocks = sorted(stock_ranks.items(), key=lambda x: x[1], reverse=True)

    # Get the top 10 stocks
    top_10_stocks = dict(sorted_stocks[:10])
    return json.dumps(top_10_stocks, indent=4)

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

@app.route('/api/main_indices', methods=['GET'])
def get_main_indices():
    indices_data = fetch_main_indices()
    return jsonify(indices_data)

@app.route('/api/stock-data/<symbol>/<time_range>')
def stock_data(symbol, time_range):
    start_date = adjust_start_date(time_range)
    end_date = datetime.today().date()
    data = fetch_historic_stock_data(symbol, start_date, end_date)
    return jsonify(data)

@app.route('/api/index-data/<symbol>/<time_range>')
def index_data(symbol, time_range):
    start_date = adjust_start_date(time_range)
    end_date = datetime.today().date()
    data = fetch_historic_stock_data(symbol, start_date, end_date)  
    return jsonify(data)

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
