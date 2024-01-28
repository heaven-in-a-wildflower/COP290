from jugaad_data.nse import NSELive

n = NSELive()

stonks = ['ADANIENT', 'ADANIPORTS', 'APOLLOHOSP', 'ASIANPAINT', 'AXISBANK', 'BAJAJ-AUTO', 'BAJFINANCE', 'BAJAJFINSV', 'BPCL', 'BHARTIARTL', 'BRITANNIA', 'CIPLA', 'COALINDIA', 'DIVISLAB', 'DRREDDY', 'EICHERMOT', 'GRASIM', 'HCLTECH', 'HDFCBANK', 'HDFCLIFE', 'HEROMOTOCO', 'HINDALCO', 'HINDUNILVR', 'ICICIBANK', 'ITC', 'INDUSINDBK', 'INFY', 'JSWSTEEL', 'KOTAKBANK', 'LTIM', 'LT', 'MARUTI', 'NTPC', 'NESTLEIND', 'ONGC', 'POWERGRID', 'RELIANCE', 'SBILIFE', 'SBIN', 'SUNPHARMA', 'TCS', 'TATACONSUM', 'TATAMOTORS', 'TATASTEEL', 'TECHM', 'TITAN', 'UPL', 'ULTRACEMCO', 'WIPRO']

def top_five_gainers_losers():
    result = []
    for stock in stonks:
        q = n.stock_quote(stock)
        price_info = q.get('priceInfo', {})  # To handle the case when 'priceInfo' is not present
        pChange = price_info.get('pChange')
        result.append({'stock': stock, 'pChange': pChange})

    # Sort stocks based on pChange
    sorted_stocks = sorted(result, key=lambda x: x['pChange'] or 0, reverse=True)

    # Select top five winners and losers
    top_five_winners = sorted_stocks[:5]
    top_five_losers = sorted_stocks[-5:][::-1]  # Reversed to get top losers

    return top_five_winners, top_five_losers

winners, losers = top_five_gainers_losers()

print("Top Five Winners:")
for winner in winners:
    print(f"{winner['stock']}: {winner['pChange']}")

print("\nTop Five Losers:")
for loser in losers:
    print(f"{loser['stock']}: {loser['pChange']}")
