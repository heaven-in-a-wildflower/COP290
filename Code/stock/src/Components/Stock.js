import React, { useState, useEffect } from 'react';
import { Line } from 'react-chartjs-2';
import { Select } from 'antd';
import 'chart.js/auto';
import { useParams } from 'react-router-dom';

const { Option } = Select;

const Stock = () => {
    
    const [showClosePrice, setShowClosePrice] = useState(true);
    const [showOpenPrice, setShowOpenPrice] = useState(true);
    const [timeRange, setTimeRange] = useState('1y');
    const [stockData, setStockData] = useState([]);

    const { stock } = useParams();

    useEffect(() => {
        const fetchData = async () => {
            try {
                // Replace with your Flask API endpoint
                const response = await fetch(`http://127.0.0.1:5000/api/stock-data/${stock}/${timeRange}`);
                const data = await response.json();
                // reverse the data to get the latest data first
                data.reverse();
                setStockData(data);
            } catch (error) {
                console.error("Error fetching data: ", error);
            }
        };

        fetchData();
    }, [stock, timeRange]);

    // Example data processing
    const dates = stockData.map(item => new Date(item.DATE).toLocaleDateString());
    const closePrices = stockData.map(item => item.CLOSE);
    const openPrices = stockData.map(item => item.OPEN);
    const volumes = stockData.map(item => item.VOLUME);

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
            }
        ],
    };

    const options = {
        scales: {
            y: {
                beginAtZero: true,
            },
        },
    };

    return (
        <>
            <Select defaultValue="1y" style={{ width: 120 }} onChange={setTimeRange}>
                <Option value="1w">1 Week</Option>
                <Option value="1m">1 Month</Option>
                <Option value="3m">3 Months</Option>
                <Option value="6m">6 Months</Option>
                <Option value="1y">1 Year</Option>
            </Select>
            <Line data={chartData} options={options} />
        </>
    );
};

export default Stock;
