# High-Precision Math & Scientific Computing

Grapa's $INT, $FLOAT, and $TIME types support unlimited precision, making it valuable for scientific, cryptographic, and financial applications.

## Key Features for Math/Scientific Work:
- **Unlimited Precision**: Handle arbitrarily large numbers and precise calculations
- **Time Series**: Built-in time manipulation and analysis
- **Parallel Computation**: Distribute mathematical workloads across threads
- **Memory Efficiency**: Process large datasets without precision loss

## Example: Financial Calculations
```grapa
/* Calculate compound interest with unlimited precision */
compound_interest = op(principal, rate, time, periods) {
    rate_per_period = rate / periods;
    total_periods = time * periods;
    principal * (1 + rate_per_period).pow(total_periods);
};

/* Calculate mortgage payments */
mortgage_payment = op(principal, annual_rate, years) {
    monthly_rate = annual_rate / 12 / 100;
    total_payments = years * 12;
    principal * (monthly_rate * (1 + monthly_rate).pow(total_payments)) /
              ((1 + monthly_rate).pow(total_payments) - 1);
};

/* Example calculations */
loan_amount = 300000;
annual_rate = 3.5;
loan_years = 30;

monthly_payment = mortgage_payment(loan_amount, annual_rate, loan_years);
total_paid = monthly_payment * loan_years * 12;
total_interest = total_paid - loan_amount;

("Monthly payment: $" + monthly_payment.str()).echo();
```

## Example: Scientific Computing
```grapa
/* Calculate pi using infinite series with unlimited precision */
calculate_pi = op(iterations) {
    pi = 0;
    i = 0;
    while (i < iterations) {
        term = 4 / (2 * i + 1);
        if (i % 2 == 0) {
            pi += term;
        } else {
            pi -= term;
        };
        i += 1;
    };
    pi;
};

/* Calculate with high precision */
pi_approximation = calculate_pi(1000000);
("Pi approximation: " + pi_approximation.str()).echo();
```

## Example: Time Series Analysis
```grapa
/* Process time series data with unlimited precision */
process_timestamps = op(data) {
    data.map(op(row) {
        timestamp = $TIME().parse(row.get("timestamp"));
        value = row.get("value").float();
        {
            "timestamp": timestamp,
            "value": value,
            "hour": timestamp.hour(),
            "day": timestamp.day(),
            "month": timestamp.month()
        };
    });
};

/* Aggregate by time periods */
aggregate_by_hour = op(processed_data) {
    grouped = processed_data.group(op(record) { record.get("hour"); });
    grouped.map(op(hour, records) {
        {
            "hour": hour,
            "count": records.len(),
            "avg_value": records.reduce(op(sum, r) { sum + r.get("value"); }, 0) / records.len(),
            "min_value": records.map(op(r) { r.get("value"); }).min(),
            "max_value": records.map(op(r) { r.get("value"); }).max()
        };
    });
};
```

- **See also:** [Python Math Examples](../PYTHON_USE_CASES.md#3-high-precision-math-scientific-computing)
- **See also:** [Cryptography](cryptography.md) for cryptographic mathematical operations 