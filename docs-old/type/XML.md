# $XML
$XML is an array of $TAG or $STR values. The $XML class also inherits the $ARRAY class, and any operation that works on an $ARRAY will work on an $XML. 

For consistency, entering a single $TAG will always result in the creation of $XML, where the $XML includes at least 1 $TAG. 

To reference items in $XML, use an index.
```
> x = <test one=1>data</test><tt hi="dd" />;

> x.len();
2

> x[0];
<test one=1>data</test>

> x[1];
<tt hi="dd"/>
```

Or specify a tag name.
```
> x = <test one=1>data</test><tt hi="dd" />;
> x.tt;
<tt hi="dd"/>
```

Use $LIST to access the attributes.
```
> x = <test one=1>data</test><tt hi="dd" />;

> x.test.$LIST.len();
1

> x.test.$LIST.one;
1
```

Index into the $TAG to access the contents.
```
> x = <test one=1>data</test><tt hi="dd" />;

> x.test.len();
1

> x.test[0];
data
```

To convert $XML or $TAG to JSON format, use the list method. This produces the underlying representation of how the $XML and $TAG values are stored.
```
> x = <test one=1>data</test><tt hi="dd" />;

> x.list();
[{"test":[{"one":1},["data"]]},{"tt":[{"hi":dd}]}]

> x.test.list();
{"test":[{"one":1},["data"]]}
```
