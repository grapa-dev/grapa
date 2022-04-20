# $TIME
References:
- [$TIME object](../../lib/grapa/$TIME.grc)
- [$OBJ object](../../lib/grapa/$OBJ.grc)

Uses UTC timezone. $TIME is intended to be a timestamp starting at Jan 1 1970, and a timer for tracking the passage of time. 

## time()

```
> (0).time();
1970-01-01T00:00:00.000000
```

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

## delta()
```
> t1 = $TIME().utc();

> t1.delta();
3.8281652
```

## Examples
The following illustrate using various date ranges.
'''
> "1020-04-23T12:33:33.921638".time();
1020-04-23T12:33:33.921638

> "1020-04-23T12:33:33.921638".time() + 24*60*60*1000000000;
1020-04-24T12:33:33.921638

> "-1020-04-23T12:33:33.921638".time() + 1;
-1020-04-23T12:33:33.921638001

> "-1020-04-23T12:33:33.921638".time().int();
-94345471586078362000

> (-94345471586078362000).time();
-1020-04-23T12:33:33.921638
''''

## Side effect of 1970
And a side effect of using 1970 as the center. 
```
> a = $TIME().utc();

> a;
2020-07-10T19:51:26.3316532

> (0-a).time();
1919-06-24T04:08:33.6683468
```
