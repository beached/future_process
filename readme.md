## Future Process

Create processes and return data via a ```std::future```.  With the only restriction being that the return type is trivial and that exceptions do not traverse the processes.  If an exception is thrown, a runtime_error will be thrown.

```cpp
#include <daw/daw_future_process.h>

auto func = []( int b ) {
	std::cout << "process\n";
	return b * b;
};

std::future<int> f1 = daw::process::async( func, 5 );
std::future<int> f2 = daw::process::async( func, 10 );

return f1.get( ) + f2.get( );
```

The above example will create two child processes.  async( Function, Args... ) returns a std::future.

## Process

Fork a child process and run a function.  This is analagous to a ```std::thread``` in interface and functionality.  

```cpp
#include <daw/daw_process.h>

auto proc = daw::process::fork_process( []( int t ) {
	sleep( t )
}, 5 );

puts( "Waiting for child to complete\n" );
proc.join( );
puts( "Child completed\n" );
```




	
