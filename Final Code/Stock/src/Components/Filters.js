import React, { useState, useEffect } from 'react';
import { Select, Spin, List, Typography } from 'antd';

const { Option } = Select;

function Filters() {
    const [filterKey, setFilterKey] = useState('marketCap');
    const [rankedStocks, setRankedStocks] = useState([]);
    const [loading, setLoading] = useState(false);

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
        <div className="filters">
            <Typography.Title level={2}>Top 10 Stocks by {filterKey}</Typography.Title>
            <Select
                defaultValue="marketCap"
                style={{ width: 200 }}
                onChange={value => setFilterKey(value)}
            >
                {/* Dynamically generate filter options */}
                {['marketCap', 'debtToEquity', 'revenuePerShare', 'returnOnAssets', 'returnOnEquity', 'earningsGrowth', 'revenueGrowth', 'grossMargins', 'pegRatio'].map((filter) => (
                    <Option key={filter} value={filter}>{filter}</Option>
                ))}
            </Select>

            {loading ? (
                <Spin tip="Loading..." />
            ) : (
                <List
                    itemLayout="horizontal"
                    dataSource={rankedStocks}
                    renderItem={item => (
                        <List.Item>
                            <List.Item.Meta
                                title={<Typography.Text>{item.title}</Typography.Text>}
                            />
                        </List.Item>
                    )}
                />
            )}
        </div>
    );
}

export default Filters;
