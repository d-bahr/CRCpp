# CRC++
Easy to use and fast C++ CRC library.

Tired of writing CRC code over and over again? Don't want to include a dozen boost header files just for a little bit of functionality? CRC++ is a portable and extremely lightweight alternative that is incredibly simple, fast, and clean.

### Features

CRC++ supports bit-by-bit and byte-by-byte calculation of full and multipart CRCs. The algorithms used are highly optimized and can even be configured to be branchless (as always, be sure to profile your code to choose the most efficient option). CRC++ is a great option for embedded C++ projects with a need for efficiency.

CRC++ consists of a single header file which can be included in any existing C++ application. No libraries, no boost, no mess, no fuss.

Any CRC width is supported, provided there is an integer type large enough to contain it. Trying to compute a 57-bit CRC? Got you covered.

Many common CRCs are provided out-of-the-box, such as CRC-32 (used in PKZip and Ethernet), CRC-XMODEM, and CRC-CCITT.

CRC++ will compile with any reasonably compliant C++11 compiler. CRC++ will not compile with C++03.

All of the CRC++ code is well-documented. Unit tests are included in the repository. HTML documentation can also be produced via Doxygen (also included in the repository).

### Usage

Computing a CRC is as simple as the following code:

```
#include "CRC.h" // Only need to include this header file!
                 // No libraries need to be included. No project settings need to be messed with.
				 
#include <iomanip>  // Includes ::std::hex
#include <iostream> // Includes ::std::cout
#include <cstdint>  // Includes ::std::uint32_t

int main(int argc, char ** argv)
{
	const char myString[] = { 'H', 'E', 'L', 'L', 'O', ' ', 'W', 'O', 'R', 'L', 'D' };
	
	std::uint32_t crc = CRC::Calculate(myString, sizeof(myString), CRC::CRC_32);
	
	std::cout << std::hex << crc;
	
	return 0;
}
```

Multi-part CRCs are also supported:

```
int main(int argc, char ** argv)
{
	const char myHelloString[] = { 'H', 'E', 'L', 'L', 'O', ' ' };
	const char myWorldString[] = { 'W', 'O', 'R', 'L', 'D' };
	
	std::uint32_t crc;
	
	crc = CRC::Calculate(myHelloString, sizeof(myHelloString), CRC::CRC_32);
	crc = CRC::Calculate(myWorldString, sizeof(myWorldString), CRC::CRC_32, crc);
	
	std::cout << std::hex << crc;
	
	return 0;
}
```

This will return the same CRC as the first example.

Both of the above examples compute a CRC bit-by-bit. However, CRC++ also supports lookup tables, as the following example demonstrates:

```
int main(int argc, char ** argv)
{
	const char myHelloString[] = { 'H', 'E', 'L', 'L', 'O', ' ' };
	const char myWorldString[] = { 'W', 'O', 'R', 'L', 'D' };
	
	CRC::Table<std::uint32_t, 32> table(CRC::CRC_32);
	
	std::uint32_t crc;
	
	crc = CRC::Calculate(myHelloString, sizeof(myHelloString), table);
	crc = CRC::Calculate(myWorldString, sizeof(myWorldString), table, crc);
	
	std::cout << std::hex << crc;
	
	return 0;
}
```

Or, if you prefer using the `auto` keyword:

```
int main(int argc, char ** argv)
{
	const char myHelloString[] = { 'H', 'E', 'L', 'L', 'O', ' ' };
	const char myWorldString[] = { 'W', 'O', 'R', 'L', 'D' };
	
	auto table = CRC::CRC_32.MakeTable();
	
	std::uint32_t crc;
	
	crc = CRC::Calculate(myHelloString, sizeof(myHelloString), table);
	crc = CRC::Calculate(myWorldString, sizeof(myWorldString), table, crc);
	
	std::cout << std::hex << crc;
	
	return 0;
}
```

Lookup tables are much faster than computing a CRC bit-by-bit, at the expense of extra memory usage. A lookup table can be reused for as many CRCs as desired until it goes out of scope.

### License

CRC++ is free to use and provided under a BSD license.
