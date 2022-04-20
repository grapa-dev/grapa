# $VECTOR
- [GrapaVector.h header](../../source/grapa/GrapaVector.h)
- [GrapaVector.cpp source](../../source/grapa/GrapaVector.cpp)
- [$VECTOR object](../../lib/grapa/$VECTOR.grc)
- [$OBJ object](../../lib/grapa/$OBJ.grc)

A $VECTOR is a multi-dimentional set of values. Most often either 1 or 2 dimentions. 

The syntax for a $VECTOR is the same as an $ARRAY, but uses a '#' before and after the array, and requires all values for each dimention to be specified (for a 2 dimentional vector, all rows much have the same number of columns). Structually, both $ARRAY and $LIST use linked lists, and $VECTOR uses arrays - and so a $VECTOR is faster to manipulate large tabels of data. But they are all in memory - for a large table on disk use $file and $TABLE which is based on a BTree. 

## t()

## rref()

## inv()

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
