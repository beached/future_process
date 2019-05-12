## Future Process

Create processes and return data like a ```std::future```.  With the only restriction being that the return type is trivial and that exceptions do not traverse the processes.  If an exception is thrown, a runtime_error will be thrown.

```cpp
auto func = []( int b ) {
	std::cout << "process\n";
	return b * b;
} );

auto f1 = daw::process::async( func, 5 );
auto f2 = daw::process::async( func, 10 );

return f1.get( ) + f2.get( );
```

The above example will create two child processes.  async( Function, Args... ) returns a std::future.
