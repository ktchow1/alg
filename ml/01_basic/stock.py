
import yfinance as yf
import matplotlib.pyplot as plt
import argparse
from datetime import datetime, timedelta
import os

def parse_date_range(date_input_str):
    """
    Parses a date string (e.g., '20231027' or '20231023-27' or '20231228-0105')
    into a list of datetime objects.
    """
    dates = []
    try:
        if '-' in date_input_str:
            start_str, end_str = date_input_str.split('-')
            start_date = datetime.strptime(start_str, '%Y%m%d')

            # Handle YYYYMMDD-DD format
            if len(end_str) == 2:
                end_date = start_date.replace(day=int(end_str))
            # Handle YYYYMMDD-MMDD format
            elif len(end_str) == 4:
                end_date = datetime.strptime(f"{start_date.year}{end_str}", '%Y%m%d')
                # If the resulting date is earlier, assume it's for the next year
                if end_date < start_date:
                    end_date = end_date.replace(year=start_date.year + 1)
            # Handle YYYYMMDD-YYYYMMDD format
            elif len(end_str) == 8:
                end_date = datetime.strptime(end_str, '%Y%m%d')
            else:
                raise ValueError("End of date range has an unsupported format.")

            if end_date < start_date:
                raise ValueError("End date cannot be before start date.")

            current_date = start_date
            while current_date <= end_date:
                dates.append(current_date)
                current_date += timedelta(days=1)
        else:
            dates.append(datetime.strptime(date_input_str, '%Y%m%d'))
    except ValueError as e:
        print(f"Error parsing date '{date_input_str}': {e}")
        print("Please use formats: YYYYMMDD, YYYYMMDD-DD, YYYYMMDD-MMDD, or YYYYMMDD-YYYYMMDD")
        exit(1)
    return dates


def fetch_and_plot_for_day(ticker, date_to_plot):
    """
    Fetches, plots, and saves intraday stock data for a given ticker and a single date.
    """
    date_str = date_to_plot.strftime('%Y%m%d')
    display_date_str = date_to_plot.strftime('%Y-%m-%d')
    start_dt = date_to_plot
    end_dt = start_dt + timedelta(days=1)

    print(f"--- Processing {ticker} for {display_date_str} ---")

    # Download data for the specified date
    data = yf.download(
        tickers=ticker,
        start=start_dt.strftime('%Y-%m-%d'),
        end=end_dt.strftime('%Y-%m-%d'),
        interval="1m",
        progress=False  # Suppress progress bar for cleaner output in a loop
    )

    if data.empty:
        print(f"No data found. It might be a weekend, a holiday, or an invalid ticker/date.\n")
        return

    # Plotting
    plt.figure(figsize=(15, 7))
    plt.plot(data['Close'], label=f'{ticker} Intraday Price')
    plt.title(f'{ticker} Stock Price per Minute ({display_date_str})')
    plt.xlabel('Time of Day (Exchange Time - America/New_York)')
    plt.ylabel('Price (USD)')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()

    # Saving the plot
    plot_dir = "plots"
    if not os.path.exists(plot_dir):
        os.makedirs(plot_dir)

    filename = f"{ticker.lower()}_{date_str}.png"
    filepath = os.path.join(plot_dir, filename)
    plt.savefig(filepath)
    print(f"Plot saved to '{filepath}'\n")
    plt.close()  # Important to free memory when in a loop


def main():
    """
    Main function to parse arguments and orchestrate the plotting process.
    """
    parser = argparse.ArgumentParser(description="Plot intraday stock data for given tickers and a date/date-range.")
    parser.add_argument('date', type=str, help="Date (YYYYMMDD) or range (e.g., 20231023-27 or 20231228-0105).")
    parser.add_argument(
        '--tickers',
        nargs='+',
        type=str,
        default=['AAPL', 'MSFT', 'GOOG', 'AMZN', 'NVDA', 'TSLA'],
        help="One or more stock ticker symbols. Defaults to a list of common tech stocks."
    )
    args = parser.parse_args()

    dates_to_process = parse_date_range(args.date)
    tickers_to_process = args.tickers

    print(f"Tickers to process: {', '.join(tickers_to_process)}")
    print(f"Date(s) to process: {args.date}\n")

    for ticker in tickers_to_process:
        for plot_date in dates_to_process:
            fetch_and_plot_for_day(ticker, plot_date)

if __name__ == "__main__":
    main()