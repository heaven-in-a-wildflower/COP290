import React, { useState, useEffect } from 'react';
import { Line } from 'react-chartjs-2';
import { Select } from 'antd';
import 'chart.js/auto';
import { useParams } from 'react-router-dom';
import styles from './Stock.module.css';

const { Option } = Select;

const stockNameMap = {
    ADANIENT: 'Adani Enterprises Ltd.',
    ADANIPORTS: 'Adani Ports and Special Economic Zone Ltd.',
    APOLLOHOSP: 'Apollo Hospitals Enterprise Ltd.',
    ASIANPAINT: 'Asian Paints Ltd.',
    AXISBANK: 'Axis Bank Ltd.',
    BAJAJAUTO: 'Bajaj Auto Ltd.',
    BAJFINANCE: 'Bajaj Finance Ltd.',
    BAJAJFINSV: 'Bajaj Finserv Ltd.',
    BPCL: 'Bharat Petroleum Corporation Ltd.',
    BHARTIARTL: 'Bharti Airtel Ltd.',
    BRITANNIA: 'Britannia Industries Ltd.',
    CIPLA: 'Cipla Ltd.',
    COALINDIA: 'Coal India Ltd.',
    DIVISLAB: "Divi's Laboratories Ltd.",
    DRREDDY: "Dr. Reddy's Laboratories Ltd.",
    EICHERMOT: 'Eicher Motors Ltd.',
    GRASIM: 'Grasim Industries Ltd.',
    HCLTECH: 'HCL Technologies Ltd.',
    HDFCBANK: 'HDFC Bank Ltd.',
    HDFCLIFE: 'HDFC Life Insurance Company Ltd.',
    HEROMOTOCO: 'Hero MotoCorp Ltd.',
    HINDALCO: 'Hindalco Industries Ltd.',
    HINDUNILVR: 'Hindustan Unilever Ltd.',
    ICICIBANK: 'ICICI Bank Ltd.',
    ITC: 'ITC Ltd.',
    INDUSINDBK: 'IndusInd Bank Ltd.',
    INFY: 'Infosys Ltd.',
    JSWSTEEL: 'JSW Steel Ltd.',
    KOTAKBANK: 'Kotak Mahindra Bank Ltd.',
    LTIM: 'LTIMindtree Ltd.',
    LT: 'Larsen & Toubro Ltd.',
    MARUTI: 'Maruti Suzuki India Ltd.',
    NTPC: 'NTPC Ltd.',
    NESTLEIND: 'Nestle India Ltd.',
    ONGC: 'Oil & Natural Gas Corporation Ltd.',
    POWERGRID: 'Power Grid Corporation of India Ltd.',
    RELIANCE: 'Reliance Industries Ltd.',
    SBILIFE: 'SBI Life Insurance Company Ltd.',
    SBIN: 'State Bank of India',
    SUNPHARMA: 'Sun Pharmaceutical Industries Ltd.',
    TCS: 'Tata Consultancy Services Ltd.',
    TATACONSUM: 'Tata Consumer Products Ltd.',
    TATAMOTORS: 'Tata Motors Ltd.',
    TATASTEEL: 'Tata Steel Ltd.',
    TECHM: 'Tech Mahindra Ltd.',
    TITAN: 'Titan Company Ltd.',
    UPL: 'UPL Ltd.',
    ULTRACEMCO: 'UltraTech Cement Ltd.',
    WIPRO: 'Wipro Ltd.',
};

const Stock = () => {
    const [showClosePrice, setShowClosePrice] = useState(true);
    const [showOpenPrice, setShowOpenPrice] = useState(true);
    const [timeRange, setTimeRange] = useState('1w');
    const [stockData, setStockData] = useState([]);
    const [stockInfo, setStockInfo] = useState({});
    const [stockName, setStockName] = useState('');

    const { stock } = useParams();

    const fetchStockName = async () => {
       // loop through the stockNameMap object and check if the key matches the stock symbol
         // if it does, set the stock name to the value of that key
        for (const key in stockNameMap) {
            if (key === stock) {
                setStockName(stockNameMap[key]);
            }
        }
    };

    useEffect(() => {
        const fetchData = async () => {
            try {
                const response = await fetch(`http://127.0.0.1:5000/api/stock_data?symbol=${stock}&time_period=${timeRange}`);
                const data = await response.json();

                let dataJSON = JSON.parse(data);

                let newData = [];

                for (let key in dataJSON) {
                    if (dataJSON.hasOwnProperty(key)) {
                        newData.push({
                            date: key,
                            open: dataJSON[key]["Open"],
                            low: dataJSON[key]["Low"],
                            close: dataJSON[key]["Close"],
                            high: dataJSON[key]["High"]
                        })
                    }
                }

                setStockData(newData);
            } catch (error) {
                console.error("Error fetching data: ", error);
            }
        };

        const fetchStockInfo = async () => {
            try {
                const response = await fetch(`http://127.0.0.1:5000/api/stock_info?symbol=${stock}`);
                const info = await response.json();
                console.log(info);
                setStockInfo(info);
            } catch (error) {
                console.error("Error fetching stock info: ", error);
            }
        };

        fetchStockName();
        fetchData();
        fetchStockInfo();
    }, [stock, timeRange]);

    // Update data processing based on the new array structure
    const dates = stockData.map(item => new Date(item.date).toLocaleDateString());
    const closePrices = stockData.map(item => item.close);
    const openPrices = stockData.map(item => item.open);
    const lowPrices = stockData.map(item => item.low);
    const highPrices = stockData.map(item => item.high);

    const chartData = {
        labels: dates,
        datasets: [
            {
                label: 'Close Price',
                data: closePrices,
                fill: false,
                borderColor: 'rgb(75, 192, 192)',
                hidden: !showClosePrice,
            },
            {
                label: 'Open Price',
                data: openPrices,
                fill: false,
                borderColor: 'rgb(255, 99, 132)',
                hidden: !showOpenPrice,
            },
            {
                label: 'Low Price',
                data: lowPrices,
                fill: false,
                borderColor: 'rgb(0, 99, 132)',
                hidden: !showOpenPrice,
            },
            {
                label: 'High Price',
                data: highPrices,
                fill: false,
                borderColor: 'rgb(55, 0, 132)',
                hidden: !showOpenPrice,
            }

        ],
    };

    const options = {
        scales: {
            y: {
                beginAtZero: false,
            },
        },
    };

    return (
        <>
            <h1>{stockName}</h1>
            <Select defaultValue="1w" style={{ width: 120 }} onChange={setTimeRange}>
            <Option value="1d">1 Day</Option>
            <Option value="1w">1 Week</Option>
            <Option value="1m">1 Month</Option>
            <Option value="3m">3 Months</Option>
            <Option value="6m">6 Months</Option>
            <Option value="1y">1 Year</Option>
            <Option value="5y">5 Year</Option>
            <Option value="10y">10 Year</Option>
            </Select>
            <div className={styles.container}>
            <div className={styles.chartContainer}>
                <Line data={chartData} options={options} />
            </div>
            {Object.keys(stockInfo).length > 0 && (
                <div className={styles.infoContainer}>
                    <div className={styles.card}>
                    <div className={styles.cardTitle}>{`${stockName} Information`}</div>
                    {Object.entries(stockInfo).map(([key, value]) => (
                        <div key={key} className={styles.keyValueRow}>
                        <span className={styles.key}>{key}:</span>
                        <span className={styles.value}>{value}</span>
                        </div>
                    ))}
                    </div>
                </div>
            )}
            </div>
        </>
    );
};

export default Stock;