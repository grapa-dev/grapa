# Loops

## while
Sytax:
* while(bool) statement;

Example:
```
> i = 0; while (i<5) {i+=1; (i.str()+":").echo();}; "\n".echo();
1:2:3:4:5:
```

## map
Sytax:
* list|array . map ( op );
* list|array . map ( op , param1, param2, etc);

Where op is:
* op (listvalue, param1, param2, etc) ( script )
* op (listvalue, param1, param2, et) { statement;}

Note: each item of the array will process in a separate thread. This is an easy way to add multi-threading to the processing, but beware and batch workoads in the array to keep the size of the array to the number of threads you want.

Take caution on accessing shared resources from within a map or reduce or $thread or $net operation...esure thread safe by using lock/unlock (any variable can be used for lock/unlock).

Example:
```
> [1,2,3].map(op(n){n*2});
[2,4,6]

> [1,2,3].map(op(n,p){n*2+p},5);
[7,9,11]
```

## reduce
Sytax:
* list|array . filter ( op );
* list|array . reduce ( op , startvalue);
* list|array . reduce ( op , startvalue, param1, param2, etc);

Where op is:
* op (startvalue, listvalue, param1, param2, etc) ( script )

Reduce process sequentially, as processing is intended to augment the startvalue. If the startvalue is not provided, the first item in the list is used as the startvalue.

Take caution on accessing shared resources from within a map or reduce or $thread or $net operation...esure thread safe by using lock/unlock (any variable can be used for lock/unlock).

Example:
```
> [1,2,3].reduce(op(s,n){s=s*n});
6

> [1,2,3].reduce(op(s,n){s+=n*2},[]);
[2,4,6]

> [1,2,3].reduce(op(s,n,p){s+=n*2+p},[],5);
[7,9,11]
```

## filter
Sytax:
* list|array . filter ( op );
* list|array . filter ( op , param1, param2, etc);

Where op is:
* op (listvalue, param1, param2, etc) ( script )
* op (listvalue, param1, param2, et) { statement;}

Note: each item of the array will process in a separate thread. This is an easy way to add multi-threading to the processing, but beware and batch workoads in the array to keep the size of the array to the number of threads you want.

Example:
```
> [1,2,3,4,5,6,7,8,9].filter(op(n){n%2!=0});
[1,3,5,7,9]

> [1,2,3,4,5,6,7,8,9].filter(op(n,p){n%p!=0},3);
[1,2,4,5,7,8]
```
