import React, { useState } from 'react';
import { BrowserRouter as Router, Route, Routes } from 'react-router-dom';
import Home from './Components/Home';
import Stock from './Components/Stock';
import { Layout, Menu, AutoComplete, Input } from 'antd';
import { SearchOutlined } from '@ant-design/icons';
import { Link } from 'react-router-dom';
import CompareStocks from './Components/CompareStocks';
import Filters from './Components/Filters';

const { Header, Content } = Layout;

function App() {

    const [data, setData] = useState({
        stocknames: [
            {
                "fullName": "Adani Enterprises Ltd.",
                "name": "ADANIENT"
            },
            {
                "fullName": "Adani Ports and Special Economic Zone Ltd.",
                "name": "ADANIPORTS"
            },
            {
                "fullName": "Apollo Hospitals Enterprise Ltd.",
                "name": "APOLLOHOSP"
            },
            {
                "fullName": "Asian Paints Ltd.",
                "name": "ASIANPAINT"
            },
            {
                "fullName": "Axis Bank Ltd.",
                "name": "AXISBANK"
            },
            {
                "fullName": "Bajaj Auto Ltd.",
                "name": "BAJAJ-AUTO"
            },
            {
                "fullName": "Bajaj Finance Ltd.",
                "name": "BAJFINANCE"
            },
            {
                "fullName": "Bajaj Finserv Ltd.",
                "name": "BAJAJFINSV"
            },
            {
                "fullName": "Bharat Petroleum Corporation Ltd.",
                "name": "BPCL"
            },
            {
                "fullName": "Bharti Airtel Ltd.",
                "name": "BHARTIARTL"
            },
            {
                "fullName": "Britannia Industries Ltd.",
                "name": "BRITANNIA"
            },
            {
                "fullName": "Cipla Ltd.",
                "name": "CIPLA"
            },
            {
                "fullName": "Coal India Ltd.",
                "name": "COALINDIA"
            },
            {
                "fullName": "Divi's Laboratories Ltd.",
                "name": "DIVISLAB"
            },
            {
                "fullName": "Dr. Reddy's Laboratories Ltd.",
                "name": "DRREDDY"
            },
            {
                "fullName": "Eicher Motors Ltd.",
                "name": "EICHERMOT"
            },
            {
                "fullName": "Grasim Industries Ltd.",
                "name": "GRASIM"
            },
            {
                "fullName": "HCL Technologies Ltd.",
                "name": "HCLTECH"
            },
            {
                "fullName": "HDFC Bank Ltd.",
                "name": "HDFCBANK"
            },
            {
                "fullName": "HDFC Life Insurance Company Ltd.",
                "name": "HDFCLIFE"
            },
            {
                "fullName": "Hero MotoCorp Ltd.",
                "name": "HEROMOTOCO"
            },
            {
                "fullName": "Hindalco Industries Ltd.",
                "name": "HINDALCO"
            },
            {
                "fullName": "Hindustan Unilever Ltd.",
                "name": "HINDUNILVR"
            },
            {
                "fullName": "ICICI Bank Ltd.",
                "name": "ICICIBANK"
            },
            {
                "fullName": "ITC Ltd.",
                "name": "ITC"
            },
            {
                "fullName": "IndusInd Bank Ltd.",
                "name": "INDUSINDBK"
            },
            {
                "fullName": "Infosys Ltd.",
                "name": "INFY"
            },
            {
                "fullName": "JSW Steel Ltd.",
                "name": "JSWSTEEL"
            },
            {
                "fullName": "Kotak Mahindra Bank Ltd.",
                "name": "KOTAKBANK"
            },
            {
                "fullName": "LTIMindtree Ltd.",
                "name": "LTIM"
            },
            {
                "fullName": "Larsen & Toubro Ltd.",
                "name": "LT"
            },
            {
                "fullName": "Maruti Suzuki India Ltd.",
                "name": "MARUTI"
            },
            {
                "fullName": "NTPC Ltd.",
                "name": "NTPC"
            },
            {
                "fullName": "Nestle India Ltd.",
                "name": "NESTLEIND"
            },
            {
                "fullName": "Oil & Natural Gas Corporation Ltd.",
                "name": "ONGC"
            },
            {
                "fullName": "Power Grid Corporation of India Ltd.",
                "name": "POWERGRID"
            },
            {
                "fullName": "Reliance Industries Ltd.",
                "name": "RELIANCE"
            },
            {
                "fullName": "SBI Life Insurance Company Ltd.",
                "name": "SBILIFE"
            },
            {
                "fullName": "State Bank of India",
                "name": "SBIN"
            },
            {
                "fullName": "Sun Pharmaceutical Industries Ltd.",
                "name": "SUNPHARMA"
            },
            {
                "fullName": "Tata Consultancy Services Ltd.",
                "name": "TCS"
            },
            {
                "fullName": "Tata Consumer Products Ltd.",
                "name": "TATACONSUM"
            },
            {
                "fullName": "Tata Motors Ltd.",
                "name": "TATAMOTORS"
            },
            {
                "fullName": "Tata Steel Ltd.",
                "name": "TATASTEEL"
            },
            {
                "fullName": "Tech Mahindra Ltd.",
                "name": "TECHM"
            },
            {
                "fullName": "Titan Company Ltd.",
                "name": "TITAN"
            },
            {
                "fullName": "UPL Ltd.",
                "name": "UPL"
            },
            {
                "fullName": "UltraTech Cement Ltd.",
                "name": "ULTRACEMCO"
            },
            {
                "fullName": "Wipro Ltd.",
                "name": "WIPRO"
            }
        ]
    });

    const [searchValue, setSearchValue] = useState('');
    const [options, setOptions] = useState([]);

    const handleSearch = value => {
        setSearchValue(value);
        if (value.length >= 2) {
            const filteredOptions = data.stocknames
                .filter(stock => stock.fullName.toLowerCase().includes(value.toLowerCase()))
                .slice(0, 5) 
                .map(stock => ({ value: stock.name, label: stock.fullName }));
            setOptions(filteredOptions);
        } else {
            setOptions([]);
        }
    };

    const handleSelect = value => {
        // Redirect to the selected stock page
        console.log(`Selected: ${value}`);
        window.location.href = `/${value}`;
    };

    return (
        <Router>
            <Layout className="layout">
                <Header>
                    <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                    <Menu theme="dark" mode="horizontal" style={{ lineHeight: '64px' , width : "100%"}}>
                        <Menu.Item key="1">
                            <Link to="/">Home</Link>
                        </Menu.Item>
                        <Menu.Item key="2">
                            <Link to="/compare">Compare Stocks</Link>
                        </Menu.Item>
                        <Menu.Item key="3">
                            <Link to="/filters">Apply Filter</Link>
                        </Menu.Item>
                    </Menu>
                        <AutoComplete
                            style={{ width: 200, marginTop: '16px' }}
                            options={options}
                            value={searchValue}
                            onSelect={handleSelect}
                            onSearch={handleSearch}
                            placeholder="Search stocks"
                        >
                            <Input suffix={<SearchOutlined />} />
                        </AutoComplete>
                    </div>
                </Header>
                <Routes>
                    <Route path="/" element={<Home />} />
                    <Route path="/compare" element={<CompareStocks />} />
                    <Route path="/:stock" element={<Stock />} />
                    <Route path="/filters" element={<Filters />} />
                </Routes>
        </Layout>
        </Router>
    );
}

export default App;