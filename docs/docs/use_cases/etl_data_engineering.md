# ETL / Data Engineering

Grapa excels at Extract, Transform, Load (ETL) workflows, offering parallelism, unlimited precision, and unified file/database APIs. Ideal for data cleaning, transformation, and large-scale processing.

## Key Features for ETL:
- **Parallel Processing**: Use `$thread` and `map/reduce/filter` for high-performance data transformation
- **Unlimited Precision**: Handle large numbers and precise calculations with `$INT` and `$FLOAT`
- **Unified APIs**: Seamlessly work with files, databases, and network resources
- **Memory Efficiency**: Process large datasets without loading everything into memory

## Example: Parallel CSV Processing
```grapa
/* Extract: Read multiple CSV files in parallel */
files = ["data1.csv", "data2.csv", "data3.csv"];
data = files.map(op(file) { $file().read(file).split("\n"); });

/* Transform: Clean and validate data in parallel */
clean_data = data.map(op(rows) { 
    rows.filter(op(row) { 
        fields = row.split(",");
        fields.len() == 5 && fields.get(0) != "";  /* Validate row structure */
    });
});

/* Load: Aggregate results and save to database */
total_records = clean_data.reduce(op(sum, rows) { sum + rows.len(); }, 0);
("Processed " + total_records.str() + " records across " + files.len().str() + " files").echo();
```

## Example: Data Validation Pipeline
```grapa
/* Define validation rules */
rules = {
    "email": {"pattern": "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"},
    "age": {"min": 0, "max": 150},
    "salary": {"min": 0, "max": 1000000}
};

/* Validate data in parallel */
validate_record = op(record, rules) {
    errors = [];
    if (!record.get("email").match(rules.get("email").get("pattern"))) {
        errors += "Invalid email format";
    };
    if (record.get("age") < rules.get("age").get("min") || record.get("age") > rules.get("age").get("max")) {
        errors += "Age out of range";
    };
    if (record.get("salary") < rules.get("salary").get("min") || record.get("salary") > rules.get("salary").get("max")) {
        errors += "Salary out of range";
    };
    {"record": record, "errors": errors, "valid": errors.len() == 0};
};

/* Process validation results */
results = data.map(op(record) { validate_record(record, rules); });
valid_records = results.filter(op(result) { result.get("valid"); });
invalid_records = results.filter(op(result) { !result.get("valid"); });

("Valid records: " + valid_records.len().str()).echo();
("Invalid records: " + invalid_records.len().str()).echo();
```

## Example: Time Series Data Processing
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

- **See also:** [Python ETL Examples](../PYTHON_USE_CASES.md#1-etl--data-engineering)

> **Best Practice:**
> Grapa's parallelism is a core, production-ready feature, specifically designed and hardened for ETL and high-throughput data processing. Users can confidently leverage `.map()`, `.filter()`, and related methods for robust, parallel ETL workloads.
> **For Python Users:**
> See the [Python Integration Guide](../PYTHON_INTEGRATION.md) and [Migration Tips for Python Users](../PYTHON_USE_CASES.md#migration-tips-for-python-users) for guidance on using Grapa for ETL and data processing from Python. 