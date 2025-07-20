# $FLOAT
References:



Supports both fix and float format. Fix will apply the precision to just the decimal. Float will not restrict to the decimal, supporting large exponents with a specified precision. Each number maintains it's own precision, which is used in math operations with numbers of other precision. Some math operations may convert an input from one format to another. For example, passing in a float to a trig function will produce a fix format result. Floats also support specifying "extra" bits to apply to the calculations to reduce error propagation (the default is 10 bits). 

Example:
```
> 5.13
5.13
```

**Note**: Scientific notation (e.g., `1e-10`, `1.5e+3`) is not currently supported. Use decimal notation instead (e.g., `0.0000000001` instead of `1e-10`).

You can also create using hex format:
```
> -0x4.5
-4.3125
```

Or binaary format:
```
> 0b101.11
5.75
```

By default, all floats are "float" format. To change formats use the fix() and float() routines, where both the precision and the "extra" bits can also be specified. The parsing engine will set the precision to the system default, which is 128 bits. There are two ways to change this. First, change the system default using the setfloat and setfix routines. Second, pass in a $STR to the fix or float routines. For example:

```
>"3.5".float()/0.511
6.849315068493150684931506849315

>"3.5".float(300,6)/0.511
6.849315068493150684931506849315068493150684931

>("3.5".float(300,6)/(0.511).float(300,6))
6.84931506849315068493150684931506849315068493150684931560202071220533200564
```

Convert to other formats:
```
> (30.75).hex()
1E.C

> (30.75).bin()
11110.11

> (30.75).int()
30
```

Bit shifts:
```
> (30.75) >> 4
1.921875

> (30.75) << 4
492.0
```

The binary representation for a grapa $FLOAT includes the items returned from the decode method - sign, trunc, fix, exp, max, extra, data.

## Float Encoding and Decoding

Grapa provides both `.encode("FLOAT")` and `.decode("FLOAT")` methods to work with the internal float structure.

### Decoding Floats

Use `.decode("FLOAT")` to extract the internal components of a float:

```grapa
> (30.75).raw()
0x00048100077B

> "30.75".float(300,6).decode("FLOAT")
{"sign":false,"trunc":false,"fix":false,"exp":4,"max":300,"extra":6,"data":123}

>"30.75".float(300,6).decode("FLOAT").data.raw()
0x7B

>"3.5".float(300,6)/0.511
6.849315068493150684931506849315068493150684931

>("3.5".float(300,6)/0.511).raw()
0x200281370A06D96CB65B2D96CB65B2D96CB65B2D96CB65B2D96CB65B3AAB

>("3.5".float(300,6)/0.511).decode("FLOAT")
{"sign":false,"trunc":true,"fix":false,"exp":2,"max":183,"extra":10,"data":42993847502648498382436913857586756274673667427835852831403}

>("3.5".float(300,6)/0.511).decode("FLOAT").data.raw()
0x6D96CB65B2D96CB65B2D96CB65B2D96CB65B2D96CB65B3AAB
```

### Encoding Floats

Use `.encode("FLOAT")` to create a float from its internal components:

```grapa
/* Create a float from its components */
components = {
    "sign": false,
    "trunc": false, 
    "fix": false,
    "exp": 4,
    "max": 300,
    "extra": 6,
    "data": 123
};

/* Encode the components back to a float */
float_value = components.encode("FLOAT");

/* The result is the same as the original */
float_value == 30.75;  /* true */

### High-Precision Math Example

```grapa
/* Create a high-precision float for mathematical calculations */
pi = "3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006606315588174881520920962829254091715364367892590360011330530548820466521384146951941511609433057270365759591953092186117381932611793105118548074462379962749567351885752724891227938183011949129833673362440656643086021394946395224737190702179860943702770539217176293176752384674818467669405132000568127145263560827785771342757789609173637178721468440901224953430146549585371050792279689258923542019956112129021960864034418159813629774771309960518707211349999998372978049951059731732816096318595024459455346908302642522308253344685035261931188171010003137838752886587533208381420617177669147303598253490428755468731159562863882353787593751957781857780532171226806613001927876611195909216420198938095257201065485863278865936153381827968230301952035301852968995773622599413891249721775283479131515574857242454150695950829533116861727855889075098381754637464939319255060400927701671139009848824012858361603563707660104710181942955596198946767837449448255379774726847104047534646208046684259069491293313677028989152104752162056966024058038150193511253382430035587640247496473263914199272604269922796782354781636009341721641219924586315030286182974555706749838505494588586926995690927210797509302955321165344987202755960236480665499119881834797753566369807426542527862551818417574672890977772793800081647060016145249192173217214772350141441973568548161361157352552133475741849468438523323907394143334547762416862518983569485562099219222184272550254256887671790494601674".float(1000,20);

/* Check the precision settings */
pi_components = pi.decode("FLOAT");
("Precision: " + pi_components.max.str() + " bits, Extra: " + pi_components.extra.str()).echo();

/* Verify no truncation occurred */
if (pi_components.trunc) {
    "Warning: Precision was truncated during calculation".echo();
} else {
    "Full precision maintained".echo();
}

/* Access the raw binary data for analysis */
pi_raw = pi_components.data.raw();
("Raw data length: " + pi_raw.len().str() + " bytes").echo();
```

### Component Details

The decode method returns an object with these components:

- **`sign`**: Boolean indicating if the number is negative
- **`trunc`**: Boolean indicating if the number was truncated during calculation
- **`fix`**: Boolean indicating if it's in fixed-point mode
- **`exp`**: The exponent value
- **`max`**: The maximum bit precision
- **`extra`**: The extra bits for calculation
- **`data`**: The actual numeric data (can be accessed with `.raw()` for hex representation)

### Use Cases

- **Debugging**: Inspect the internal state of floats during calculations
- **Serialization**: Convert floats to structured data for storage or transmission
- **Precision Analysis**: Examine precision settings and truncation status
- **Binary Analysis**: Access the raw binary representation of float data
- **High-Precision Math**: Work with arbitrary-precision calculations and verify precision settings

## Float Comparisons

Grapa handles float comparisons intelligently by normalizing precision settings during comparison operations. This ensures consistent behavior when comparing float literals with string-converted floats.

### Comparison Behavior

```grapa
/* Direct float comparisons work correctly */
55.3 == 55.3;                    /* true */
123.456 == 123.456;              /* true */

/* String-to-float comparisons work correctly */
"55.3".float() == 55.3;          /* true */
"123.456".float() == 123.456;    /* true */

/* Mixed type comparisons work correctly */
55.3 == "55.3".float();          /* true */
"123.456".float() == 123.456;    /* true */

/* Non-numeric strings don't match */
"hello".float() == 5.0;          /* false */

/* Adaptive tolerance handles edge cases */
0.0000000001 == "0.0000000001";  /* true */
1000000.0 == "1000000.0000000001"; /* true (adaptive tolerance) */
```

### Precision Normalization and Adaptive Tolerance

When comparing floats, Grapa automatically normalizes both operands to use the same precision settings as the `.float()` method. For string-to-float comparisons, Grapa uses adaptive tolerance that scales with the magnitude of the numbers being compared:

- **Float literals and string-converted floats compare correctly**
- **Precision differences don't cause unexpected comparison failures**
- **The same precision settings are applied to both operands**
- **Adaptive tolerance provides better handling for very small and very large numbers**
- **Base tolerance of 10^-10 scales proportionally with number magnitude**

### Best Practices

- Use the `.float()` method when converting strings to floats for consistent precision
- Float comparisons work correctly regardless of how the values were created
- Non-numeric strings converted to floats will not match numeric values
- **Scientific notation**: Use decimal notation instead of scientific notation (e.g., `0.0000000001` not `1e-10`)


