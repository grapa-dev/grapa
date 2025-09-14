# $TIME
References:



Uses UTC timezone. $TIME is intended to be a timestamp starting at Jan 1 1970, and a timer for tracking the passage of time. The year is not bounded, in that it can be large negative numbers and large positive numbers. 

## time()

```
> (0).time();
1970-01-01T00:00:00.000000
```

### String Parsing

The `.time()` method can parse various string formats to create a `$TIME` object:

**Supported Formats:**

1. **ISO 8601 with microseconds and timezone**:
   ```grapa
   "2025-09-14T20:15:17.8017135+05:00".time()
   "2025-09-14T20:15:17.8017135-08:00".time()
   "2025-09-14T20:15:17.8017135+0500".time()    // 4-digit timezone format
   ```

2. **ISO 8601 with microseconds** (default):
   ```grapa
   "2025-09-14T20:15:17.8017135".time()
   ```

3. **ISO 8601 without microseconds, with timezone**:
   ```grapa
   "2025-09-14T20:15:17+05:00".time()
   "2025-09-14T20:15:17-08:00".time()
   "2025-09-14T20:15:17+0500".time()            // 4-digit timezone format
   "2025-09-14T20:15:17Z".time()                // UTC indicator
   ```

4. **ISO 8601 without microseconds**:
   ```grapa
   "2025-09-14T20:15:17".time()
   ```

5. **Space-separated date and time**:
   ```grapa
   "2025-09-14 20:15:17".time()
   ```

6. **US format (MM/DD/YYYY with time)**:
   ```grapa
   "09/14/2025 20:15:17".time()
   ```

7. **US format (MM/DD/YYYY, time defaults to 00:00:00)**:
   ```grapa
   "09/14/2025".time()
   ```

8. **European format (DD/MM/YYYY with time)**:
   ```grapa
   "14/09/2025 20:15:17".time()
   ```

9. **European format (DD/MM/YYYY, time defaults to 00:00:00)**:
   ```grapa
   "14/09/2025".time()
   ```

10. **Date only** (time defaults to 00:00:00):
    ```grapa
    "2025-09-14".time()
    ```

11. **Time only** (uses current date):
    ```grapa
    "20:15:17".time()
    ```

**Timezone Handling:**
- All timezone offsets are automatically converted to UTC for internal storage
- Supported timezone formats: `+05:00`, `-08:00`, `+0500`, `-0800`, `Z`
- Use `.tz()` to convert from UTC to local timezones as needed

**Note:** All parsed times are stored internally in UTC. Use `.tz()` to convert to local timezones as needed.

## tz()
```
> $TIME().tz();
-28800000000000

> $TIME().tz()/1000000000
-28800
```

## utc()
```
> $TIME().utc();
2020-07-10T19:05:58.6135794

> $TIME().utc() + $TIME().tz();
2020-07-10T12:06:15.9529207
```

## ms()
```
> t1 = $TIME().utc();

> t1.ms();
3.8281652
```

The `.ms()` method calculates the time difference in milliseconds. It can be used in two ways:

1. **Time since creation**: `t1.ms()` - returns milliseconds since the time object was created
2. **Time difference**: `t1.ms(t2)` - returns milliseconds between `t1` and `t2`

### Performance Monitoring Example
The `.ms()` method is particularly useful for performance monitoring:

```grapa
t1 = $TIME().utc();
1000.sleep();
t2 = t1.utc();
'Time1=${from_t1}; Time2=${from_t2}; Delta=${t2_t1}\n'.interpolate(from_t1:t1.ms().float(30),from_t2:t1.ms(t2).float(30),t2_t1:(t2-t1)/1000000).echo();
1000.sleep();
'Time1=${from_t1}; Time2=${from_t2}; Delta=${t2_t1}\n'.interpolate(from_t1:t1.ms().float(30),from_t2:t1.ms(t2).float(30),t2_t1:(t2-t1)/1000000).echo();
```

**Output:**
```
Time1=1249.2387; Time2=80.3584; Delta=1168.8803
Time1=2509.5192; Time2=1340.6389; Delta=1168.8803
```

This example demonstrates:
- **Time1**: Milliseconds since `t1` was created
- **Time2**: Milliseconds between `t1` and `t2` 
- **Delta**: Time difference in seconds (converted from nanoseconds)

## Examples
The following illustrate using various date ranges and string formats.

```
> "1020-04-23T12:33:33.921638".time();
1020-04-23T12:33:33.921638

> "2025-09-14 20:15:17".time();
2025-09-14T20:15:17.0

> "2025-09-14".time();
2025-09-14T00:00:00.0

> "09/14/2025".time();
2025-09-14T00:00:00.0

> "2025-09-14T20:15:17+05:00".time();
2025-09-14T15:15:17.0

> "2025-09-14T20:15:17-08:00".time();
2025-09-15T04:15:17.0

> "2025-09-14T20:15:17.123456+05:00".time();
2025-09-14T15:15:17.000123456

> "2025-09-14T20:15:17Z".time();
2025-09-14T20:15:17.0

> "1020-04-23T12:33:33.921638".time() + 24*60*60*1000000000;
1020-04-24T12:33:33.921638

> "-1020-04-23T12:33:33.921638".time() + 1;
-1020-04-23T12:33:33.921638001

> "-1020-04-23T12:33:33.921638".time().int();
-94345471586078362000

> (-94345471586078362000).time();
-1020-04-23T12:33:33.921638
```

## Side effect of 1970
And a side effect of using 1970 as the center. 

```
> a = $TIME().utc();

> a;
2020-07-10T19:51:26.3316532

> (0-a).time();
1919-06-24T04:08:33.6683468
```
