# $thread
Provides a thread library, cross functional with all platforms supported.

Take caution on accessing shared resources from within a map or reduce or $thread or $net operation...ensure thread safe by using lock/unlock (any variable can be used for lock/unlock).

## type()
$thread

## trylock()
Attempts to lock.

## lock()
Applies a lock.

## unlock()
Releases the lock.

## wait()
Current thread waits for signal.

## signal()
Signals the wait to continue.

## waiting()
Indicates in waiting state.

## start(runOp, input, doneOp)
Starts the runOp in the background, passing paramList. When the thread exists, doneOp is called. All 3 inputs are copied,  as the originals are likely to go away after running the start command. So if an instance of object is passed in, the thread will end up using a copied instance and not the original instance.

The thread is run from the same namespace as where it is called. To use a shared object instance, access the variable from within the thread rather than passing in the variable. Or pass in the $ID for the variable and difference the variable from the thread (which essentially does the same thing as a variable lookup but allows a different variable name to be used). 

If accessing shared resources from within a thread, take care and use thread save logic, such as lock/unlock. 

The following is an example of creating a thread.

```
myRun = op(input) {"myRun:".echo();$sys().echo(@$local); input.c = input.a+input.b; "\n".echo(); @$local;};
myDone = op(input,result) {"myDone:".echo();$sys().echo(@$local); "\n".echo();};
t = $thread();
t.start(myRun,{a:1,b:2},myDone);
```

Output for above
```
myRun:{"input":{"a":1,"b":2}}
myDone:{"input":{"a":1,"b":2,"c":3},"result":{"input":{"a":1,"b":2,"c":3}}}
```

The input parameter is passed to both the run op and done op. The done op also receives any output from the run op.


## stop()
Stops the thread.

## started()
Indicates the running state of the thread.

## suspend()
Suspends the thread. If the thread is processing a queue and the queue is empty, put the thread in suspend mode. Than after pushing data onto the queue, call resume to have the thread resume processing.

## resume()
See suspend.

## suspended()
Indicates whether the thread is in a suspended state.
