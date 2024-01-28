from jugaad_data.nse import NSELive
n = NSELive()

#Data to be shown along with index(not stock) when its graph s being displayed.
def fetch_index_price_info(symbol):
    i = n.live_index(symbol)
    data = i['data'][0]
    keys_to_remove = list(data.keys())[:3]
    for key in keys_to_remove:
        del data[key]
    return data

print(fetch_index_price_info("NIFTY 50"))