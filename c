import yfinance as yf
import pandas as pd
import matplotlib.pyplot as plt

# Define cryptocurrencies
crypto_list = ["BTC-USD", "ETH-USD", "ADA-USD", "XRP-USD", "BNB-USD", "SOL-USD", "DOGE-USD"]

# Download price data
data = yf.download(crypto_list, start="2009-01-01", end="2023-12-31")

# Resample to monthly data (adjust as needed)
data = data.resample("M").mean()

# Handle potential missing 'Adj Close' column
yearly_data_list = []
for currency in crypto_list:
    closing_column = ("Adj Close", currency) if ("Adj Close", currency) in data.columns else ("Close", currency)
    # Use the available closing price column (either 'Adj Close' or 'Close')
    yearly_data_currency = data.resample("Y").agg({closing_column: ["first", "last"]})
    yearly_data_currency.columns = [f"{currency} Jan 1st", f"{currency} Dec 31st"]
    yearly_data_list.append(yearly_data_currency)
    if closing_column[0] == "Close":
        print(f"WARNING: 'Adj Close' not found for {currency}. Using 'Close' instead.")

# Combine the yearly data for all currencies
yearly_data = pd.concat(yearly_data_list, axis=1)

# Calculate annual change and percentage change for each currency
for currency in crypto_list:
    yearly_data[f"{currency} Change"] = yearly_data[f"{currency} Dec 31st"] - yearly_data[f"{currency} Jan 1st"]
    yearly_data[f"{currency} Change %"] = (yearly_data[f"{currency} Change"] / yearly_data[f"{currency} Jan 1st"]) * 100

# Create subplots (3x2 grid for this example)
fig, axes = plt.subplots(3, 2, figsize=(15, 12))
count = 0

# Plot price data and calculate price at the beginning of each year (assuming $1 investment)
for i in range(3):
    for j in range(2):
        if count < len(crypto_list):
            currency = crypto_list[count]
            # Price data subplot
            axes[i, j].plot(data.index, data[("Close", currency)]) # Use "Close" here as it's always available
            axes[i, j].set_title(currency + " Price")
            axes[i, j].grid(True)

            # Calculate price at the beginning of each year (assuming $1 investment)
            yearly_data[f"Price at Jan 1st ({currency})"] = data[("Close", currency)].iloc[0] / yearly_data[f"{currency} Jan 1st"]

            # Plot price at the beginning of each year (optional)
            # axes[i, j].twinx()  # Uncomment to add a secondary y-axis for price
            # axes[i, j].plot(yearly_data.index, yearly_data[f"Price at Jan 1st ({currency})"], color='red')

        count += 1

# Adjust layout and suptitle
fig.suptitle('Cryptocurrency Price Comparison (2009-2023) - Subplots & Annual Change')
plt.tight_layout()
plt.show()

# Print annual change table (consider exporting to CSV for detailed data)
print("Annual Change Summary (2009-2023):")
columns_to_print = [f"{currency} Change %" for currency in crypto_list]
print(yearly_data[columns_to_print].to_string())
