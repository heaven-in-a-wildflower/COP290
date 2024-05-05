import React, { useState, useEffect } from 'react';
import { Layout, Card, Row, Col, Avatar } from 'antd';
import { ArrowDownOutlined, ArrowUpOutlined, StockOutlined } from '@ant-design/icons';

const { Content } = Layout;

const Home = () => {
    const [losers, setLosers] = useState([]);
    const [gainers, setGainers] = useState([]);
    const [indices, setIndices] = useState([]);  // Assuming you have an API endpoint for indices

    useEffect(() => {
        // Fetch top losers and gainers
        const fetchData = async () => {
            try {
                const resGainers = await fetch('http://127.0.0.1:5000/api/gainers-losers');
                const dataGainers = await resGainers.json();
                setGainers(dataGainers.winners);
                setLosers(dataGainers.losers);
    
                // Fetch indices from the new API endpoint
                const resIndices = await fetch('http://127.0.0.1:5000/api/nse_data');
                const dataIndices = await resIndices.json();
                console.log(dataIndices);
                setIndices(dataIndices);
                console.log(dataIndices);
            } catch (error) {
                console.error('Error fetching data:', error);
            }
        };
        fetchData();
    }, []);

    return (
        <Layout className="layout">
            <Content style={{ padding: '50px' }}>
                <Row gutter={16}>
                    {/* Losers */}
                    <Col span={8}>
                        <h2 style={{ color: 'red' }}>Top Losers <ArrowDownOutlined /></h2>
                        {losers.map(loser => (
                            <Card size="small" key={loser.stock} style={{ marginBottom: '10px' }}>
                                <Avatar style={{ backgroundColor: 'red' }} icon={<ArrowDownOutlined />} />
                                <span style={{ marginLeft: '10px'}}>{loser.stock} : <span style={{color : 'red'}}>{loser.pChange.toFixed(2)} %</span></span>
                            </Card>
                        ))}
                    </Col>
                    {/* Gainers */}
                    <Col span={8}>
                        <h2 style={{ color: 'green' }}>Top Gainers <ArrowUpOutlined /></h2>
                        {gainers.map(gainer => (
                            <Card size="small" key={gainer.stock} style={{ marginBottom: '10px' }}>
                                <Avatar style={{ backgroundColor: 'green' }} icon={<ArrowUpOutlined />} />
                                <span style={{ marginLeft: '10px'}}>{gainer.stock} : <span style={{color : 'green'}}>+{gainer.pChange.toFixed(2)} % </span></span>
                            </Card>
                        ))}
                    </Col>
                    {/* Indices */}
                    {/* Update accordingly if you have indices data */}
                    <Col span={8}>
                        <h2>Indices <StockOutlined /></h2>
                        {indices.map(index => (
                            <Card size="small" key={index.indexName} style={{ marginBottom: '10px' }}>
                                <Avatar icon={<StockOutlined />} />
                                <span style={{ marginLeft: '10px' }}>
                                    {index.indexName} - 
                                    <span style={{ color: index.percChange > 0 ? 'green' : 'red' }}>
                                        {index.last} ({index.percChange}%)
                                    </span>
                                </span>
                            </Card>
                        ))}
                    </Col>
                </Row>
            </Content>
        </Layout>
    );
};

export default Home;
