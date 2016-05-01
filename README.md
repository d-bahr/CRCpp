# CRC++
Easy to use and fast C++ CRC library.

Tired of writing CRC code over and over again? Don't want to include a dozen boost header files just for a little bit of functionality? CRC++ is an extremely lightweight alternative that is incredibly simple, fast, and clean.

Computing a CRC is as simple as the following code:

```
int main(int argc, char ** argv)
{
	char myString[] = "Hello World";
	
	uint32_t crc = CRC::Calculate(myString, sizeof(myString), CRC::CRC_32);
	
	std::cout << std::hex << crc;
	
	return 0;
}
```

CRC++ supports bit-by-bit calculation of full and multipart CRCs. The algorithm is branchless and highly optimized. Any CRC width is supported, provided there is an integer type large enough to contain it. Trying to compute a 57-bit CRC? Got you covered.

Many common CRCs are provided out-of-the-box, such as CRC-32 and CRC-CCITT.

CRC++ consists of a single header file which can be included in any existing C++ application. No libraries, no boost, no mess, no fuss.

CRC++ only includes one header file, cstdint, but it can be configured to compile with no include files at all.

CRC++ will compile with any reasonably compliant C++11 compiler.

CRC++ is free to use and provided under a BSD license.
