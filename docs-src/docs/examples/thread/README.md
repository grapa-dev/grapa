# Thread Examples

This directory contains working examples of Grapa's threading system that demonstrate proper patterns for multi-threaded programming.

## ⚠️ **Important: Sleep Limitation**

**Critical**: These examples work because they either avoid using `.sleep()` or use it in a way that doesn't trigger the global sleep queue limitation. See [Thread System](../../sys/thread.md) for details about the sleep behavior.

## Available Examples

### 1. `thread_example1.grc` - Basic Thread Lifecycle
- **Demonstrates**: Basic thread creation, execution, and completion
- **Key Pattern**: Simple thread management without complex coordination
- **Sleep Usage**: Single sleep call (avoids global queue issues)
- **Use Case**: Basic threading when you need simple background execution

### 2. `thread_example2.grc` - Coroutine Suspend/Resume
- **Demonstrates**: Coroutine pattern using suspend/resume
- **Key Pattern**: Thread coordination through suspend/resume instead of sleep
- **Sleep Usage**: None (uses suspend/resume for coordination)
- **Use Case**: When you need precise control over thread execution flow

### 3. `thread_example3.grc` - Thread-Safe Counter with Lock
- **Demonstrates**: Shared resource protection using locks
- **Key Pattern**: Critical section protection with proper lock/unlock
- **Sleep Usage**: None (uses suspend/resume for coordination)
- **Use Case**: When multiple threads need to access shared data safely

### 4. `thread_example4.grc` - Producer-Consumer Pattern
- **Demonstrates**: Classic producer-consumer pattern with proper coordination
- **Key Pattern**: Thread coordination through suspend/resume and locks
- **Sleep Usage**: Coordinated sleep calls (designed around the limitation)
- **Use Case**: When you need producer-consumer workflows with multiple workers

## Best Practices Demonstrated

1. **Use suspend/resume for coordination** instead of sleep
2. **Protect shared resources** with proper locking
3. **Coordinate thread execution** through state management
4. **Avoid global sleep coordination** in multi-threaded code
5. **Design around the sleep limitation** when sleep is necessary

## Running the Examples

```bash
# Run any example
./grapa docs-src/docs/examples/thread/thread_example1.grc
./grapa docs-src/docs/examples/thread/thread_example2.grc
./grapa docs-src/docs/examples/thread/thread_example3.grc
./grapa docs-src/docs/examples/thread/thread_example4.grc
```

## Related Documentation

- [Thread System](../../sys/thread.md) - Complete threading system documentation
- [Thread-Safe Variables](../../syntax/thread_safe_variables.md) - Variable safety in concurrent code
- [Sleep Limitation Details](../../sys/thread.md#critical-sleep-behavior-limitation) - In-depth explanation of sleep behavior

## Why These Examples Work

These examples work because they:

1. **Avoid the global sleep queue** by using suspend/resume for coordination
2. **Use proper locking patterns** to protect shared resources
3. **Coordinate threads through state management** rather than timing
4. **Design around the sleep limitation** when sleep is necessary
5. **Follow proven concurrency patterns** that work in any threading system

Use these examples as templates for your own multi-threaded code, and always consider the sleep limitation when designing thread coordination.

