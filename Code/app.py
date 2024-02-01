from flask import Flask, jsonify, render_template
from datetime import datetime
from dateutil.relativedelta import relativedelta
from jugaad_data.nse import stock_df, NSELive
from flask_cors import CORS

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

# def indices():
#     n = NSELive()
#     all_indices = n.all_indices()

#     all_indices['data'][0]['indexName'] = 'NIFTY 50'
#     all_indices['data'][1]['indexName'] = 'NIFTY NEXT 50'
#     all_indices['data'][2]['indexName'] = 'NIFTY MIDCAP 50'

#     return all_indices['data']

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
    return render_template("index.html",winners = winners, losers = losers)

if __name__ == '__main__':
    app.run(debug=True)
