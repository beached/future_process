## Future Process

Create processes and return data like a ```std::future```.  With the only restriction being that the return type is trivial and that exceptions do not traverse the processes.  If an exception is thrown, a runtime_error will be thrown.

```cpp
	auto a = daw::process::future_process<int( int )>( []( int b ) {
		std::cout << "process\n";
		return b * b;
	} );

	auto f1 = a( 5 );
	auto f2 = a( 10 );

	return f1.get( ) + f2.get( );
}
```
