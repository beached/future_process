A set of process/ipc classes and functions to allow for a C++ like experience.  So far tested on Linux(gcc 8.3.0/clang 8.0) and Mac(Apple Clang 10.0.1/gcc 9.1)
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
static constexpr std::string_view message = "This is a long string test, how about that eh! Hello World.";

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
    std::string val = chan.read( );
    std::cout << "parent: message from child '" << val << "'\n";
    assert( val == message );
}
```

## Collection Channel
Send multiple trivial types over a channel.  When reading a ```std::vector<T>``` is returned.

```cpp
#include <daw/daw_process.h>
#include <daw/daw_collection_channel.h>

static std::vector<int> mul( std::vector<int> vec, int multiplier ) {
	for( int & item: vec ) {
		item *= multiplier;
	}
	return vec;
}

int main( ) {
	auto chan = daw::process::collection_channel<int>( );
	static std::vector<int> const message = { 2, 5, 6, 7 };

	auto proc = daw::process::fork_process(
	  [&chan]( unsigned int t ) {
	  	int multiplier = 0;
		  while( true ) {
			  puts( "child: sleeping\n" );
			  sleep( t );
			  puts( "child: awake\n" );
			  chan.write( mul( message, multiplier++ ) );
		  }
	  },
	  2 );

	int multiplier = 0;
	while( true ) {
		puts( "parent: awaiting child\n" );
		auto val = chan.read( );
		puts( "parent: message from child " );
		assert( val == mul( message, multiplier++ ) );
	}
}
```

## Shared Mutex
An interprocess mutex that acts like ```std::mutex```.  It can be used with items like ```std::lock_guard```

```cpp
#include <daw/daw_process.h>
#include <daw/daw_shared_mutex.h>

auto mut = daw::process::shared_mutex( );
auto lck = std::unique_lock( mut );

auto proc = daw::process::fork_process(
	[]( daw::process::shared_mutex mtx ) {
		puts( "child: about to wait\n" );
		auto lck2 = std::lock_guard( mtx );
		puts( "child: awake\n" );
	},
	mut );

sleep( 2 );
puts( "parent: about to wake child\n" );
lck.unlock( );
```
