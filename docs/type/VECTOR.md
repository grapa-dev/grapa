# $VECTOR
- [GrapaVector.h header](../../source/grapa/GrapaVector.h)
- [GrapaVector.cpp source](../../source/grapa/GrapaVector.cpp)
- [$VECTOR object](../../lib/grapa/$VECTOR.grc)
- [$OBJ object](../../lib/grapa/$OBJ.grc)
 
A $VECTOR is a multi-dimentional set of values. Most often either 1 or 2 dimentions. 

The syntax for a $VECTOR is the same as an $ARRAY, but uses a '#' before and after the array, and requires all values for each dimention to be specified (for a 2 dimentional vector, all rows much have the same number of columns). Structually, both $ARRAY and $LIST use linked lists, and $VECTOR uses arrays - and so a $VECTOR is faster to manipulate large tabels of data. But they are all in memory - for a large table on disk use $file and $TABLE which is based on a BTree. 

```
> ["a","b","c"].vector();
#["a","b","c"]#

>#["a","b","c"]#.array();
["a","b","c"]
```

## t()

```
> #[["a","b","c"],["d","e","f"]]#.t();
#[["a","d"],["b","e"],["c","f"]]#
```

## rref()
Row-Echelon Form.
[Example reference](https://www.geeksforgeeks.org/row-echelon-form/)

```
> #[[4,0,1],[2,0,2],[3,0,3]]#.rref();
#[[1,0.0,0.0],[0,0,1]]#

> #[[1,2,-1,-4],[2,3,-1,-11],[-2,0,-3,22]]#.rref();
#[[1,0.0,0.0,-8],[0,1,0.0,1],[0,0,1,-2]]#
```


## inv()
[Example reference](https://www.tutorialspoint.com/numpy/numpy_inv.htm)
x
```
> #[[1,2],[3,4]]#.inv()
#[[-2,1],[1.5,-0.5]]#

> #[[1.0,1.0,1.0],[0.0,2.0,5.0],[2.0,5.0,-1.0]]#.inv();
#[[1.2857142857142857142857142857142,-0.28571428571428571428571428571428,-0.14285714285714285714285714285714],[-0.47619047619047619047619047619047,0.14285714285714285714285714285714,0.23809523809523809523809523809523],[0.19047619047619047619047619047619,0.14285714285714285714285714285714,-0.095238095238095238095238095238095]]#
```

## det()

## rank()

## solve()

## cov(axis)

## sum(axis)

## mean(axis)

## shape()

## reshape(dim) 

## norm()

## dot(b)

## triu(b) 

## tril(b)

## eigh()
