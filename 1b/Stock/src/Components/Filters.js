import React, { useState, useEffect } from 'react';
import { Select, Spin, List, Typography } from 'antd';

const { Option } = Select;

function Filters() {
    const [filterKey, setFilterKey] = useState('marketCap');
    const [rankedStocks, setRankedStocks] = useState([]);
    const [loading, setLoading] = useState(false);

    // Mapping of filter keys to user-friendly names
    const filterKeyMapping = {
        marketCap: 'Market Cap',
        debtToEquity: 'Debt To Equity',
        revenuePerShare: 'Revenue Per Share',
        returnOnAssets: 'Return On Assets',
        returnOnEquity: 'Return On Equity',
        earningsGrowth: 'Earnings Growth',
        revenueGrowth: 'Revenue Growth',
        grossMargins: 'Gross Margins',
        pegRatio: 'PEG Ratio',
        trailingPE: 'P/E Ratio'
    };

    useEffect(() => {
        const fetchRankedStocks = async () => {
            setLoading(true);
            try {
                const response = await fetch(`http://127.0.0.1:5000/api/rank_stocks?filter_key=${filterKey}`);
                const data = await response.json();
                // Assuming the response is an object where keys are stock symbols and values are the respective filter values
                const formattedData = Object.entries(data).map(([key, value]) => ({
                    title: `${key}: ${value}`
                }));
                setRankedStocks(formattedData);
            } catch (error) {
                console.error("Error fetching ranked stocks:", error);
            } finally {
                setLoading(false);
            }
        };

        fetchRankedStocks();
    }, [filterKey]);

    return (
        <div className="filters" style={{ padding: '0 20px' }}>
            <Typography.Title level={2} className="filter-title">
                Top 10 Stocks by {filterKeyMapping[filterKey]}
            </Typography.Title>
            <Select
                defaultValue="marketCap"
                style={{ width: 200, marginBottom: '16px' }}
                onChange={value => setFilterKey(value)}
            >
                {/* Dynamically generate filter options */}
                {Object.keys(filterKeyMapping).map((key) => (
                    <Option key={key} value={key}>{filterKeyMapping[key]}</Option>
                ))}
            </Select>
    
            {loading ? (
                <Spin tip="Loading..." />
            ) : (
                <List
                    itemLayout="horizontal"
                    dataSource={rankedStocks}
                    renderItem={(item, index) => (
                        <List.Item className="stock-item">
                            <List.Item.Meta
                                title={<Typography.Text className="stock-name" style={{ color: 'black', fontSize: '1.2em', marginLeft: '8px' }}>{`${index + 1}. ${item.title.split(': ')[0]}`}</Typography.Text>}
                                description={<Typography.Text className="stock-number" style={{ color: 'blue', fontSize: '1.2em' }}>{item.title.split(': ')[1]}</Typography.Text>}
                            />
                        </List.Item>
                    )}
                />
            )}
        </div>
    );
    
    
}

export default Filters;
