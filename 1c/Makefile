.PHONY: all run file1 file2 file3 file4 file5 best_of_all

# VENV_DIR := $(CURDIR)/venv
# PYTHON := $(VENV_DIR)/bin/python
CXX := g++
THREADS := 4

run: #download_requirements

export BEST_OF_ALL_CHECK = false

# download_requirements:
# 	@echo "Installing dependencies"
# 	@$(VENV_DIR)/bin/pip install -q pandas
# 	@$(VENV_DIR)/bin/pip install -q jugaad-data
# 	@$(VENV_DIR)/bin/pip install -q matplotlib

# BASIC strategy
ifeq ($(strip $(strategy)),BASIC)
run: file1

file1: #download_requirements
	@echo "Compiling and executing BASIC strategy"
	@$(CXX) -o basic basic.cpp
	@./basic $(symbol) $(n) $(x) "$(start_date)" "$(end_date)"

# DMA strategy
else ifeq ($(strip $(strategy)),DMA)
run: file2

file2: #download_requirements
	@echo "Compiling and executing DMA strategy"
	@$(CXX) -o dma dma.cpp
	@./dma $(symbol) $(n) $(x) $(p) "$(start_date)" "$(end_date)"

# DMA++ strategy
else ifeq ($(strip $(strategy)),DMA++)
run: file3

file3: #download_requirements
	@echo "Compiling and executing DMA++ strategy"
	@$(CXX) -o dma++ dma++.cpp
	@./dma++ $(symbol) $(x) $(p) $(n) $(max_hold_days) $(c1) $(c2) "$(start_date)" "$(end_date)"

# MACD strategy
else ifeq ($(strip $(strategy)),MACD)
run: file4

file4: #download_requirements
	@echo "Compiling and executing MACD strategy"
	@$(CXX) -o macd macd.cpp
	@./macd $(symbol) $(x) "$(start_date)" "$(end_date)"

# RSI strategy
else ifeq ($(strip $(strategy)),RSI)
run: file5

file5: #download_requirements
	@echo "Compiling and executing RSI strategy"
	@$(CXX) -o rsi rsi.cpp
	@./rsi $(symbol) $(x) $(n) $(oversold_threshold) $(overbought_threshold) "$(start_date)" "$(end_date)"

# ADX strategy
else ifeq ($(strip $(strategy)),ADX)
run: file6

file6: #download_requirements
	@echo "Compiling and executing ADX strategy"
	@$(CXX) -o adx adx.cpp
	@./adx $(symbol) $(x) $(n) $(adx_threshold) "$(start_date)" "$(end_date)"

else ifeq ($(strip $(strategy)),LINEAR_REGRESSION)
run: file7

file7: #download_requirements
	@echo "Compiling and executing LINEAR strategy"
	@$(CXX) -o lr lr.cpp
	@./lr $(symbol) $(x) $(p) "$(train_start_date)" "$(train_end_date)" "$(start_date)" "$(end_date)"

# PAIRS strategy
else ifeq ($(strip $(strategy)),PAIRS)
    ifeq ($(strip $(stop_loss_threshold)),)
run: run_pairs
run_pairs:
	@echo "Compiling and executing PAIRS strategy"
	@$(CXX) -o pairs pairs.cpp
	@./pairs $(symbol1) $(symbol2) $(x) $(n) $(threshold) "$(start_date)" "$(end_date)"

    else
run: run_pairs_stoploss
run_pairs_stoploss:
	@echo "Compiling and executing PAIRS strategy with stop loss threshold"
	@$(CXX) -o stoploss stoploss.cpp
	@./stoploss $(symbol1) $(symbol2) $(x) $(n) $(threshold) $(stop_loss_threshold) "$(start_date)" "$(end_date)"
    endif


# BEST_OF_ALL strategy
else ifeq ($(strip $(strategy)),BEST_OF_ALL)
run: best_of_all

# Run all strategies in parallel
best_of_all:
	@echo "Running best-of-all strategy"
	$(MAKE) -j$(THREADS) file1_bol file2_bol file3_bol file4_bol file5_bol file6_bol file7_bol symbol=$(symbol) start_date=$(start_date) end_date=$(end_date) BEST_OF_ALL_CHECK=true
	@python3 best_strategy_chooser.py

# basic.cpp for BEST_OF_ALL
file1_bol: #download_requirements
	@echo "Compiling and executing BASIC strategy"
	@$(CXX) -o basic basic.cpp
	@./basic $(symbol) 7 5 "$(start_date)" "$(end_date)"

# dma.cpp for BEST_OF_ALL
file2_bol: #download_requirements
	@echo "Compiling and executing DMA strategy"
	@$(CXX) -o dma dma.cpp
	@./dma $(symbol) 50 5 2 "$(start_date)" "$(end_date)"

# dma++.cpp for BEST_OF_ALL
file3_bol: #download_requirements
	@echo "Compiling and executing DMA++ strategy"
	@$(CXX) -o dma++ dma++.cpp
	@./dma++ $(symbol) 5 5 14 28 2 0.2 "$(start_date)" "$(end_date)"

# macd.cpp for BEST_OF_ALL
file4_bol: #download_requirements
	@echo "Compiling and executing MACD strategy"
	@$(CXX) -o macd macd.cpp
	@./macd $(symbol) 5 "$(start_date)" "$(end_date)"

# rsi.cpp for BEST_OF_ALL
file5_bol: #download_requirements
	@echo "Compiling and executing RSI strategy"
	@$(CXX) -o rsi rsi.cpp
	@./rsi $(symbol) 5 14 30 70 "$(start_date)" "$(end_date)"

# adx.cpp for BEST_OF_ALL
file6_bol: #download_requirements
	@echo "Compiling and executing ADX strategy"
	@$(CXX) -o adx adx.cpp
	@./adx $(symbol) 5 14 25 "$(start_date)" "$(end_date)"

file7_bol: #download_requirements
	@echo "Compiling and executing LINEAR_REGRESSION strategy"
	@$(CXX) -o lr lr.cpp
	@./lr $(symbol) 5 2 "$(start_date)" "$(end_date)"

endif
