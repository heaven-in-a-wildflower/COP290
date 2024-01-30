import React, { useState, useEffect } from 'react';
import { Line } from 'react-chartjs-2';
import { Select } from 'antd';
import 'chart.js/auto';

const { Option } = Select;

const CompareStocks = () => {
    const [selectedStocks, setSelectedStocks] = useState([]);
    const [chartDataSets, setChartDataSets] = useState([]);
    const [timeRange, setTimeRange] = useState('1y');

    useEffect(() => {
        const fetchData = async (stock) => {
            try {
                // Replace with your Flask API endpoint
                const response = await fetch(`http://127.0.0.1:5000/api/stock-data/${stock}/${timeRange}`);
                const data = await response.json();
                // reverse the data to get the latest data first
                data.reverse();
                return data;
            } catch (error) {
                console.error("Error fetching data for stock: ", stock, error);
                return [];
            }
        };

        const loadChartData = async () => {
            const allDataSets = await Promise.all(selectedStocks.map(async (stock) => {
                const data = await fetchData(stock);
                // Process the data to fit the chart data structure
                return {
                    label: stock,
                    data: data.map(item => item.CLOSE), // example for close price
                    fill: false,
                    borderColor: `#${Math.floor(Math.random()*16777215).toString(16)}`, // random color
                };
            }));

            setChartDataSets(allDataSets);
        };

        if (selectedStocks.length > 0) {
            loadChartData();
        } else {
            setChartDataSets([]);
        }
    }, [selectedStocks, timeRange]);

    const chartData = {
        labels: chartDataSets[0]?.data.map((_, index) => `Day ${index + 1}`) || [],
        datasets: chartDataSets
    };

    return (
        <>
            <Select
                mode="multiple"
                style={{ width: '100%' }}
                placeholder="Select stocks"
                onChange={setSelectedStocks}
            >
                <Option value="ADANIENT">Adani Enterprises Ltd.</Option>
                <Option value="ADANIPORTS">Adani Ports and Special Economic Zone Ltd.</Option>
                <Option value="APOLLOHOSP">Apollo Hospitals Enterprise Ltd.</Option>
                <Option value="ASIANPAINT">Asian Paints Ltd.</Option>
                <Option value="AXISBANK">Axis Bank Ltd.</Option>
                <Option value="BAJAJ-AUTO">Bajaj Auto Ltd.</Option>
                <Option value="BAJFINANCE">Bajaj Finance Ltd.</Option>
                <Option value="BAJAJFINSV">Bajaj Finserv Ltd.</Option>
                <Option value="BPCL">Bharat Petroleum Corporation Ltd.</Option>
                <Option value="BHARTIARTL">Bharti Airtel Ltd.</Option>
                <Option value="BRITANNIA">Britannia Industries Ltd.</Option>
                <Option value="CIPLA">Cipla Ltd.</Option>
                <Option value="COALINDIA">Coal India Ltd.</Option>
                <Option value="DIVISLAB">Divi's Laboratories Ltd.</Option>
                <Option value="DRREDDY">Dr. Reddy's Laboratories Ltd.</Option>
                <Option value="EICHERMOT">Eicher Motors Ltd.</Option>
                <Option value="GRASIM">Grasim Industries Ltd.</Option>
                <Option value="HCLTECH">HCL Technologies Ltd.</Option>
                <Option value="HDFCBANK">HDFC Bank Ltd.</Option>
                <Option value="HDFCLIFE">HDFC Life Insurance Company Ltd.</Option>
                <Option value="HEROMOTOCO">Hero MotoCorp Ltd.</Option>
                <Option value="HINDALCO">Hindalco Industries Ltd.</Option>
                <Option value="HINDUNILVR">Hindustan Unilever Ltd.</Option>
                <Option value="ICICIBANK">ICICI Bank Ltd.</Option>
                <Option value="ITC">ITC Ltd.</Option>
                <Option value="INDUSINDBK">IndusInd Bank Ltd.</Option>
                <Option value="INFY">Infosys Ltd.</Option>
                <Option value="JSWSTEEL">JSW Steel Ltd.</Option>
                <Option value="KOTAKBANK">Kotak Mahindra Bank Ltd.</Option>
                <Option value="LTIM">LTIMindtree Ltd.</Option>
                <Option value="LT">Larsen & Toubro Ltd.</Option>
                <Option value="MARUTI">Maruti Suzuki India Ltd.</Option>
                <Option value="NTPC">NTPC Ltd.</Option>
                <Option value="NESTLEIND">Nestle India Ltd.</Option>
                <Option value="ONGC">Oil & Natural Gas Corporation Ltd.</Option>
                <Option value="POWERGRID">Power Grid Corporation of India Ltd.</Option>
                <Option value="RELIANCE">Reliance Industries Ltd.</Option>
                <Option value="SBILIFE">SBI Life Insurance Company Ltd.</Option>
                <Option value="SBIN">State Bank of India</Option>
                <Option value="SUNPHARMA">Sun Pharmaceutical Industries Ltd.</Option>
                <Option value="TCS">Tata Consultancy Services Ltd.</Option>
                <Option value="TATACONSUM">Tata Consumer Products Ltd.</Option>
                <Option value="TATAMOTORS">Tata Motors Ltd.</Option>
                <Option value="TATASTEEL">Tata Steel Ltd.</Option>
                <Option value="TECHM">Tech Mahindra Ltd.</Option>
                <Option value="TITAN">Titan Company Ltd.</Option>
                <Option value="UPL">UPL Ltd.</Option>
                <Option value="ULTRACEMCO">UltraTech Cement Ltd.</Option>
                <Option value="WIPRO">Wipro Ltd.</Option>

            </Select>
            <Select defaultValue="1y" style={{ width: 120, marginTop: 10 }} onChange={setTimeRange}>
                <Option value="1w">1 Week</Option>
                <Option value="1m">1 Month</Option>
                <Option value="3m">3 Months</Option>
                <Option value="6m">6 Months</Option>
                <Option value="1y">1 Year</Option>
            </Select>
            <Line data={chartData} />
        </>
    );
};

export default CompareStocks;
