---
tags:
  - maintainer
  - implementation
  - threading
  - coroutines
  - system-components
---

# Grapa $thread System Deep Dive

## Overview

Grapa's `$thread` system provides a complete coroutine and threading implementation that is **already fully implemented** and powers the entire language execution pipeline. This document provides a comprehensive deep dive into both the Grapa interface (`lib/grapa/$thread.grc`) and the underlying C++ implementation.

## Table of Contents

1. [Grapa Interface (`$thread.grc`)](#grapa-interface-threadgrc)
2. [C++ Implementation Architecture](#c-implementation-architecture)
3. [Thread Lifecycle Management](#thread-lifecycle-management)
4. [Synchronization and Locking](#synchronization-and-locking)
5. [Coroutine Control Methods](#coroutine-control-methods)
6. [Real-World Usage Patterns](#real-world-usage-patterns)
7. [Performance Characteristics](#performance-characteristics)
8. [Cross-Platform Implementation](#cross-platform-implementation)

---

## Grapa Interface (`$thread.grc`)

### Complete Method Reference

The `$thread` class provides 13 methods for complete thread and coroutine control:

```grapa
@global["$thread"] = class {
    /* Core Thread Management */
    start = @<[op,@<thread_start,{null,@<var,{runCode}>,@<var,{param}>,@<var,{doneCode}>}>],{runCode,param,doneCode}>;
    stop = @<[op,@<thread_stop>]>;
    started = @<[op,@<thread_started>]>;
    
    /* Coroutine Control */
    suspend = @<[op,@<thread_suspend>]>;
    resume = @<[op,@<thread_resume>]>;
    suspended = @<[op,@<thread_suspended>]>;
    
    /* Synchronization Primitives */
    trylock = @<[op,@<thread_trylock>]>;
    lock = @<[op,@<thread_lock>]>;
    unlock = @<[op,@<thread_unlock>]>;
    
    /* Condition Variables */
    wait = @<[op,@<thread_wait>]>;
    signal = @<[op,@<thread_signal>]>;
    waiting = @<[op,@<thread_waiting>]>;
    
    /* Utility Methods */
    type = @<"type",{@<this>}>;
    describe = @<"describe",{@<this>}>;
};
```

### Method Details

#### **Core Thread Management**

##### `start(runCode, param, doneCode)`
- **Purpose**: Start a thread with execution code and callbacks
- **Parameters**:
  - `runCode`: Function to execute in the thread
  - `param`: Parameter to pass to the run function
  - `doneCode`: Completion callback function
- **Returns**: Error code (0 = success, -1 = failure)
- **C++ Implementation**: `GrapaLibraryRuleStartEvent::Run()`

##### `stop()`
- **Purpose**: Stop the thread gracefully
- **Parameters**: None
- **Returns**: Error code
- **C++ Implementation**: `GrapaLibraryRuleStopEvent::Run()`

##### `started()`
- **Purpose**: Check if thread is currently running
- **Parameters**: None
- **Returns**: Boolean (true = running, false = stopped)
- **C++ Implementation**: `GrapaLibraryRuleStartedEvent::Run()`

#### **Coroutine Control**

##### `suspend()`
- **Purpose**: Pause thread execution (coroutine suspend)
- **Parameters**: None
- **Returns**: Error code
- **C++ Implementation**: `GrapaLibraryRuleSuspendEvent::Run()`

##### `resume()`
- **Purpose**: Resume suspended thread (coroutine resume)
- **Parameters**: None
- **Returns**: Error code
- **C++ Implementation**: `GrapaLibraryRuleResumeEvent::Run()`

##### `suspended()`
- **Purpose**: Check if thread is currently suspended
- **Parameters**: None
- **Returns**: Boolean (true = suspended, false = running)
- **C++ Implementation**: `GrapaLibraryRuleSuspendedEvent::Run()`

#### **Synchronization Primitives**

##### `trylock()`
- **Purpose**: Try to acquire lock (non-blocking)
- **Parameters**: None
- **Returns**: Boolean (true = acquired, false = failed)
- **C++ Implementation**: `GrapaLibraryRuleTryLockEvent::Run()`

##### `lock()`
- **Purpose**: Acquire lock (blocking)
- **Parameters**: None
- **Returns**: Error code
- **C++ Implementation**: `GrapaLibraryRuleLockEvent::Run()`

##### `unlock()`
- **Purpose**: Release lock
- **Parameters**: None
- **Returns**: Error code
- **C++ Implementation**: `GrapaLibraryRuleUnLockEvent::Run()`

#### **Condition Variables**

##### `wait()`
- **Purpose**: Wait for signal (condition variable)
- **Parameters**: None
- **Returns**: Error code
- **C++ Implementation**: `GrapaLibraryRuleWaitEvent::Run()`

##### `signal()`
- **Purpose**: Signal waiting threads
- **Parameters**: None
- **Returns**: Error code
- **C++ Implementation**: `GrapaLibraryRuleSignalEvent::Run()`

##### `waiting()`
- **Purpose**: Check if thread is waiting for signal
- **Parameters**: None
- **Returns**: Boolean (true = waiting, false = not waiting)
- **C++ Implementation**: `GrapaLibraryRuleWaitingEvent::Run()`
