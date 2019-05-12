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

## Semaphore

A semaphore that allows post, wait, and try_wait operations.

```cpp
#include <daw/daw_process.h>
#include <daw/daw_semaphore.h>

auto sem_a = daw::process::semaphore( );
auto sem_b = daw::process::semaphore( );

auto proc = daw::process::fork_process( [&]( unsigned int t ) {
	while( true ) {
		puts( "child: sleeping\n" );
		sleep( t );
		puts( "child: awake\n" );
		sem_b.post( );
		puts( "child: awaiting parent acknowledgement\n" );
		sem_a.wait( );
		puts( "child: got parent's acknowledgement\n" );
	}
}, 2 );

while( true ) {
	puts( "parent: awaiting child\n" );
	sem_b.wait( );
	puts( "parent: got child's post\n" );
	sem_a.post( );
	puts( "parent: sent child's post\n" );
}
```

## Channel

A way to send and ackknowledge a message has been both sent and recieved.

```cpp
#include <daw/daw_channel.h>
#include <daw/daw_process.h>

auto chan = daw::process::channel<unsigned int>( );

auto proc = daw::process::fork_process( [&chan]( unsigned int t ) {
	while( true ) {
		puts( "child: sleeping\n" );
		sleep( t );
		puts( "child: awake\n" );
		chan.write( t );
	}
}, 2 );

while( true ) {
	puts( "parent: awaiting child\n" );
	auto val = chan.read( );
	assert( val == 2 );
	puts( "parent: got child's post\n" );
}
```

## String Channel

Similar to channel but for transferring string like things

```cpp
auto chan = daw::process::string_channel( );
static std::string const message = "This is a long string test, how about that eh! Hello World.";

auto proc = daw::process::fork_process( [&chan]( unsigned int t ) {
    while( true ) {
        puts( "child: sleeping\n" );
        sleep( t );
        puts( "child: awake\n" );
        chan.write( message );
    }
}, 2 );

while( true ) {
    puts( "parent: awaiting child\n" );
    auto val = chan.read( );
    std::cout << "parent: message from child '" << val << "'\n";
    assert( val == message );
}
```
