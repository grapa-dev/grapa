# High-Precision Math & Scientific Computing

Grapa's $INT, $FLOAT, and $TIME types support unlimited precision, making it valuable for scientific, cryptographic, and financial applications. Grapa includes sophisticated mathematical operators and functions that go far beyond basic arithmetic.

## Key Features for Math/Scientific Work:
- **Unlimited Precision**: Handle arbitrarily large numbers and precise calculations
- **Advanced Mathematical Operators**: Root calculations, matrix operations, modular arithmetic
- **Number Theory Functions**: Prime generation, primality testing, GCD calculations
- **Time Series**: Built-in time manipulation and analysis
- **Parallel Computation**: Distribute mathematical workloads across threads
- **Memory Efficiency**: Process large datasets without precision loss

## Advanced Mathematical Operators

### Root Calculations (`*/`)
```grapa
/* Calculate nth roots with unlimited precision */
square_root = 16 */ 2;        /* 4 (square root) */
cube_root = 27 */ 3;          /* 3 (cube root) */
fourth_root = 256 */ 4;       /* 4 (fourth root) */

/* Large number roots */
large_number = 123456789012345678901234567890;
large_root = large_number */ 5;  /* Fifth root of large number */
("Fifth root: " + large_root.str()).echo();
```

### Matrix Operations
```grapa
/* Matrix multiplication */
matrix1 = [[1,2],[3,4]];
matrix2 = [[5,6],[7,8]];
product = matrix1 * matrix2;  /* [[19,22],[43,50]] */

/* Matrix inversion using bitwise NOT */
inverse = ~matrix1;           /* Matrix inverse */

/* Vector operations */
vector = [1,2,3,4,5];
scaled = vector * 2;          /* [2,4,6,8,10] */
```

### Modular Arithmetic (`%`)
```grapa
/* Basic modulo operations */
result = 17 % 5;              /* 2 */

/* Modular exponentiation */
base = 7;
exponent = 13;
modulus = 11;
result = base.modpow(exponent, modulus);  /* 2 */

/* Modular multiplicative inverse */
value = 3;
modulus = 11;
inverse = value.modinv(modulus);  /* 4 */
("Modular inverse: " + inverse.str()).echo();
```

## Number Theory and Prime Numbers

### Prime Number Generation
```grapa
/* Generate large prime numbers for mathematical research */
prime_256 = 256.genprime();   /* 256-bit prime */
prime_512 = 512.genprime();   /* 512-bit prime */
prime_1024 = 1024.genprime(); /* 1024-bit prime */

("Generated 256-bit prime: " + prime_256.str()).echo();

/* Generate safe primes (p-1)/2 is also prime */
safe_prime = 256.genprime(1);
("Safe prime: " + safe_prime.str()).echo();
```

### Primality Testing
```grapa
/* Test numbers for primality with high confidence */
is_prime = 17.isprime();      /* true */
is_prime = 100.isprime();     /* false */

/* Test large numbers */
large_number = 123456789012345678901234567890123456789;
is_large_prime = large_number.isprime();
("Is large number prime? " + is_large_prime.str()).echo();

/* Test with custom confidence level */
is_prime = 17.isprime(100);   /* Test with confidence 100 */
```

### Greatest Common Divisor
```grapa
/* Find GCD of two numbers */
a = 48;
b = 18;
gcd_result = a.gcd(b);        /* 6 */

/* Large number GCD */
large_a = 123456789012345678901234567890;
large_b = 987654321098765432109876543210;
large_gcd = large_a.gcd(large_b);
("Large GCD: " + large_gcd.str()).echo();
```

## Data Format Conversion

### Mathematical Format Conversions
```grapa
/* Convert between mathematical formats */
number = 123456789;

/* Raw byte representation */
raw_bytes = number.raw();
("Raw bytes: " + raw_bytes.str()).echo();

/* Hexadecimal representation */
hex_string = number.hex();
("Hex: " + hex_string).echo();

/* Binary representation */
binary_string = number.bin();
("Binary: " + binary_string).echo();

/* Unsigned integer (important for mathematical operations) */
unsigned_int = number.uint();
("Unsigned: " + unsigned_int.str()).echo();
```

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

## Example: Number Theory Research
```grapa
/* Research prime number patterns */
find_prime_patterns = op(range_start, range_end) {
    primes = [];
    i = range_start;
    while (i <= range_end) {
        if (i.isprime()) {
            primes += i;
        };
        i += 1;
    };
    primes;
};

/* Analyze prime gaps */
analyze_prime_gaps = op(primes) {
    gaps = [];
    i = 1;
    while (i < primes.len()) {
        gap = primes.get(i) - primes.get(i - 1);
        gaps += gap;
        i += 1;
    };
    {
        "gaps": gaps,
        "avg_gap": gaps.reduce(op(sum, g) { sum + g; }, 0) / gaps.len(),
        "max_gap": gaps.max(),
        "min_gap": gaps.min()
    };
};

/* Example usage */
primes = find_prime_patterns(1000, 2000);
analysis = analyze_prime_gaps(primes);
("Average prime gap: " + analysis.get("avg_gap").str()).echo();
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

## Example: Linear Algebra
```grapa
/* Solve linear systems using matrix operations */
solve_linear_system = op(coefficients, constants) {
    /* Convert to matrix form: Ax = b */
    A = coefficients;
    b = constants;
    
    /* Calculate inverse of A */
    A_inv = ~A;
    
    /* Solution: x = A^(-1) * b */
    solution = A_inv * b;
    solution;
};

/* Example: Solve 2x + 3y = 8, 4x + y = 7 */
coefficients = [[2,3],[4,1]];
constants = [8,7];
solution = solve_linear_system(coefficients, constants);
("Solution: x = " + solution.get(0).str() + ", y = " + solution.get(1).str()).echo();
```

- **See also:** [Python Math Examples](../PYTHON_USE_CASES.md#3-high-precision-math-scientific-computing)
- **See also:** [Cryptography](cryptography.md) for cryptographic mathematical operations
- **See also:** [Operators](../syntax/operator.md) for advanced mathematical operators 