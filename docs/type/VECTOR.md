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
```

## inv()
[Example reference](https://www.tutorialspoint.com/numpy/numpy_inv.htm)

* something not working quite right...might be the rref. 

```
> #[[1,2],[3,4]]#.inv()
#[[-2,1],[1.5,-0.5]]#
```

```
Tried: > #[[1,1,1],[0,2,5],[2,5,-1]]#.inv()
#[[0.0,0.0,0.5]]#

This should be:
 [[ 1.28571429 -0.28571429 -0.14285714]
  [-0.47619048 0.14285714 0.23809524]
  [ 0.19047619 0.14285714 -0.0952381 ]]

Can see it here with rref.
#[[1,1,1,1,0,0],[0,2,5,0,1,0],[2,5,-1,0,0,1]]#.rref();
#[[1,2.5,-0.5,0.0,0.0,0.5]]#
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
