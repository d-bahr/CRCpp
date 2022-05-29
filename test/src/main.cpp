/**
    @file main.cpp
    @author Daniel Bahr
    @version 1.2.0.0
    @copyright
    @parblock
        CRC++
        Copyright (c) 2022, Daniel Bahr
        All rights reserved.

        Redistribution and use in source and binary forms, with or without
        modification, are permitted provided that the following conditions are met:

        * Redistributions of source code must retain the above copyright notice, this
          list of conditions and the following disclaimer.

        * Redistributions in binary form must reproduce the above copyright notice,
          this list of conditions and the following disclaimer in the documentation
          and/or other materials provided with the distribution.

        * Neither the name of CRC++ nor the names of its
          contributors may be used to endorse or promote products derived from
          this software without specific prior written permission.

        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
        AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
        IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
        DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
        FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
        DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
        SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
        CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
        OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
        OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    @endparblock
*/

#include "CRC.h"

#include <iomanip>  // Includes ::std::hex, ::std::setw
#include <iostream> // Includes ::std::cerr, ::std::endl
#include <string>   // Includes ::std::string

#ifdef CRCPP_USE_NAMESPACE
using ::CRCPP::CRC;
#endif

static int returnCode = 0;

/**
    @brief Checks the result of a CRC test and prints an error message if the test failed.
    @note Error message is printed to std::cerr with information about the CRC parameters, expected CRC, and computed CRC if a CRC test fails.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bytes
    @param[in] parameters CRC parameters
    @param[in] expectedCRC Expected CRC of the data, used for comparison
    @param[in] computedCRC Computed CRC of the data, used for comparison
    @param[in] crcName Human-readable name of the CRC algorithm used.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, uint16_t CRCWidth>
static void PrintResult(const void * data, size_t size, const CRC::Parameters<CRCType, CRCWidth> & parameters, CRCType expectedCRC, CRCType computedCRC, const std::string & crcName, bool printASCII)
{
    using ::std::cerr;
    using ::std::endl;
    using ::std::hex;
    using ::std::setw;
    using ::std::string;

    if (expectedCRC != computedCRC)
    {
        if (printASCII)
        {
            string dataAsASCII(reinterpret_cast<const char *>(data), reinterpret_cast<const char *>(data) + size);

            cerr << "Invalid CRC: \"" << dataAsASCII << "\"" << std::endl;
        }
        else
        {
            const unsigned char * bytes = reinterpret_cast<const unsigned char *>(data);
            cerr << "Invalid CRC: \"";
            for (size_t i = 0; i < size; ++i)
                cerr << setw(2) << hex << static_cast<uint16_t>(bytes[i]);
            cerr << "\"" << std::endl;
        }

        cerr << "CRC parameters: " << endl;
        cerr << "\tName: "           << crcName << endl;
        cerr << "\tPolynomial: "     << setw(CRCWidth) << hex << uint64_t(parameters.polynomial)   << endl;
        cerr << "\tInitial value: "  << setw(CRCWidth) << hex << uint64_t(parameters.initialValue) << endl;
        cerr << "\tFinal XOR: "      << setw(CRCWidth) << hex << uint64_t(parameters.finalXOR)     << endl;
        cerr << "\tReflect input: "  << (parameters.reflectInput  ? "true" : "false") << endl;
        cerr << "\tReflect output: " << (parameters.reflectOutput ? "true" : "false") << endl;

        cerr << "Expected CRC: " << setw(CRCWidth) << hex << uint64_t(expectedCRC) << endl;
        cerr << "Computed CRC: " << setw(CRCWidth) << hex << uint64_t(computedCRC) << endl;
        cerr << endl;

        returnCode = 1;
    }
}

/**
    @brief Full CRC bit-by-bit test function. Calculates a CRC over some specified data and compares the calculated CRC to an expected CRC.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bytes
    @param[in] parameters CRC parameters
    @param[in] expectedCRC Expected CRC of the data, used for comparison
    @param[in] crcName Human-readable name of the CRC algorithm used.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, uint16_t CRCWidth>
static void CRCBitByBitTest(const char * data, size_t size, const CRC::Parameters<CRCType, CRCWidth> & parameters, CRCType expectedCRC, const std::string & crcName)
{
    CRCType computedCRC = CRC::Calculate(data, size, parameters);

    PrintResult<CRCType, CRCWidth>(data, size, parameters, expectedCRC, computedCRC, crcName, true);
}

/**
    @brief Partial CRC bit-by-bit test function. Calculates a CRC in two parts over some specified data and compares the calculated CRC to an expected CRC.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bytes
    @param[in] parameters CRC parameters
    @param[in] expectedCRC Expected CRC of the data, used for comparison
    @param[in] crcName Human-readable name of the CRC algorithm used.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, uint16_t CRCWidth>
static void PartialCRCBitByBitTest(const char * data, size_t size, const CRC::Parameters<CRCType, CRCWidth> & parameters, CRCType expectedCRC, const std::string & crcName)
{
    CRCType computedCRC = CRC::Calculate(data, size / 2, parameters);

    // Don't forget (size + 1) to round odd numbers properly!
    computedCRC = CRC::Calculate(data + (size / 2), (size + 1) / 2, parameters, computedCRC);

    PrintResult<CRCType, CRCWidth>(data, size, parameters, expectedCRC, computedCRC, crcName, true);
}

/**
    @brief Full CRC table test function. Calculates a CRC over some specified data and compares the calculated CRC to an expected CRC.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bytes
    @param[in] parameters CRC parameters
    @param[in] expectedCRC Expected CRC of the data, used for comparison
    @param[in] crcName Human-readable name of the CRC algorithm used.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, uint16_t CRCWidth>
static void CRCTableTest(const char * data, size_t size, const CRC::Parameters<CRCType, CRCWidth> & parameters, CRCType expectedCRC, const std::string & crcName)
{
    CRC::Table<CRCType, CRCWidth> crcTable(parameters);

    CRCType computedCRC = CRC::Calculate(data, size, crcTable);

    PrintResult<CRCType, CRCWidth>(data, size, parameters, expectedCRC, computedCRC, crcName, true);
}

/**
    @brief Partial CRC table test function. Calculates a CRC in two parts over some specified data and compares the calculated CRC to an expected CRC.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bytes
    @param[in] parameters CRC parameters
    @param[in] expectedCRC Expected CRC of the data, used for comparison
    @param[in] crcName Human-readable name of the CRC algorithm used.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, uint16_t CRCWidth>
static void PartialCRCTableTest(const char * data, size_t size, const CRC::Parameters<CRCType, CRCWidth> & parameters, CRCType expectedCRC, const std::string & crcName)
{
    CRC::Table<CRCType, CRCWidth> crcTable(parameters);

    CRCType computedCRC = CRC::Calculate(data, size / 2, crcTable);

    // Don't forget (size + 1) to round odd numbers properly!
    computedCRC = CRC::Calculate(data + (size / 2), (size + 1) / 2, crcTable, computedCRC);

    PrintResult<CRCType, CRCWidth>(data, size, parameters, expectedCRC, computedCRC, crcName, true);
}

/**
    @brief Partial CRC table test function. Calculates a CRC in two parts over some specified data and compares the calculated CRC to an expected CRC.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bits
    @param[in] parameters CRC parameters
    @param[in] expectedCRC Expected CRC of the data, used for comparison
    @param[in] crcName Human-readable name of the CRC algorithm used.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, uint16_t CRCWidth>
static void CRCBitByBitNon8BitTest(const unsigned char * data, size_t size, const CRC::Parameters<CRCType, CRCWidth> & parameters, CRCType expectedCRC, const std::string & crcName)
{
    CRCType computedCRC = CRC::CalculateBits(data, size, parameters);

    PrintResult<CRCType, CRCWidth>(data, (size + CHAR_BIT - 1) / CHAR_BIT, parameters, expectedCRC, computedCRC, crcName, false);
}

/**
    @brief Partial CRC table test function. Calculates a CRC in two parts over some specified data and compares the calculated CRC to an expected CRC.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bits
    @param[in] parameters CRC parameters
    @param[in] expectedCRC Expected CRC of the data, used for comparison
    @param[in] crcName Human-readable name of the CRC algorithm used.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, uint16_t CRCWidth>
static void CRCTableNon8BitTest(const unsigned char * data, size_t size, const CRC::Parameters<CRCType, CRCWidth> & parameters, CRCType expectedCRC, const std::string & crcName)
{
    CRC::Table<CRCType, CRCWidth> crcTable(parameters);

    CRCType computedCRC = CRC::CalculateBits(data, size, crcTable);

    PrintResult<CRCType, CRCWidth>(data, (size + CHAR_BIT - 1) / CHAR_BIT, parameters, expectedCRC, computedCRC, crcName, false);
}

/**
    @brief Computes a bit-by-bit test and table CRC test.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bytes
    @param[in] parameters CRC parameters
    @param[in] expectedCRC Expected CRC of the data, used for comparison
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
#define CRC_TEST(data, size, parameters, expectedCRC) \
    CRCBitByBitTest(       data, size, parameters(), expectedCRC, #parameters); \
    PartialCRCBitByBitTest(data, size, parameters(), expectedCRC, #parameters); \
    CRCTableTest(          data, size, parameters(), expectedCRC, #parameters); \
    PartialCRCTableTest(   data, size, parameters(), expectedCRC, #parameters)

/**
    @brief Computes a bit-by-bit test and table CRC test with a non-8-bit multiple input size.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bits
    @param[in] parameters CRC parameters
    @param[in] expectedCRC Expected CRC of the data, used for comparison
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
#define CRC_PARTIAL_BYTE_TEST(data, size, parameters, expectedCRC) \
    CRCBitByBitNon8BitTest(data, size, parameters(), expectedCRC, #parameters); \
    CRCTableNon8BitTest(   data, size, parameters(), expectedCRC, #parameters);

/**
    @brief Unit test entry point.
    @param[in] argc Unused
    @param[in] argv Unused
    @return Always returns 0
*/
int main(int argc, char ** argv)
{
    static const char   CRC_CHECK_DATA[] = "123456789";
    static const size_t CRC_CHECK_SIZE   = sizeof(CRC_CHECK_DATA) - 1; // Note: Do not calculate CRC of null-terminator.

    // Suppress warning messages when compiling with -Wunused-parameter
    (void)argc;
    (void)argv;

#ifdef CRCPP_INCLUDE_ESOTERIC_CRC_DEFINITIONS
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_4_ITU,         uint8_t(0x7));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_5_EPC,         uint8_t(0x00));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_5_ITU,         uint8_t(0x07));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_5_USB,         uint8_t(0x19));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_6_CDMA2000A,   uint8_t(0x0D));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_6_CDMA2000B,   uint8_t(0x3B));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_6_ITU,         uint8_t(0x06));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_6_NR,          uint8_t(0x15));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_7,             uint8_t(0x75));
#endif
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_8,             uint8_t(0xF4));
#ifdef CRCPP_INCLUDE_ESOTERIC_CRC_DEFINITIONS
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_8_EBU,         uint8_t(0x97));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_8_HDLC,        uint8_t(0x2F));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_8_MAXIM,       uint8_t(0xA1));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_8_WCDMA,       uint8_t(0x25));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_8_LTE,         uint8_t(0xEA));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_10,            uint16_t(0x199));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_10_CDMA2000,   uint16_t(0x233));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_11,            uint16_t(0x5A3));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_11_NR,         uint16_t(0x5CA));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_12_UMTS,       uint16_t(0xDAF));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_12_CDMA2000,   uint16_t(0xD4D));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_12_DECT,       uint16_t(0xF5B));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_13_BBC,        uint16_t(0x04FA));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_15,            uint16_t(0x059E));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_15_MPT1327,    uint16_t(0x2566));
#endif
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_ARC,        uint16_t(0xBB3D));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_BUYPASS,    uint16_t(0xFEE8));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_CCITTFALSE, uint16_t(0x29B1));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_MCRF4XX,    uint16_t(0x6F91));
#ifdef CRCPP_INCLUDE_ESOTERIC_CRC_DEFINITIONS
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_CDMA2000,   uint16_t(0x4C06));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_CMS,        uint16_t(0xAEE7));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_DECTR,      uint16_t(0x007E));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_DECTX,      uint16_t(0x007F));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_DNP,        uint16_t(0xEA82));
#endif
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_GENIBUS,    uint16_t(0xD64E));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_KERMIT,     uint16_t(0x2189));
#ifdef CRCPP_INCLUDE_ESOTERIC_CRC_DEFINITIONS
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_MAXIM,      uint16_t(0x44C2));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_MODBUS,     uint16_t(0x4B37));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_T10DIF,     uint16_t(0xD0DB));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_USB,        uint16_t(0xB4C8));
#endif
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_X25,        uint16_t(0x906E));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_XMODEM,     uint16_t(0x31C3));
#ifdef CRCPP_INCLUDE_ESOTERIC_CRC_DEFINITIONS
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_17_CAN,        uint32_t(0x04F03));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_21_CAN,        uint32_t(0x0ED841));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_24,            uint32_t(0x21CF02));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_24_FLEXRAYA,   uint32_t(0x7979BD));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_24_FLEXRAYB,   uint32_t(0x1F23B8));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_24_LTEA,       uint32_t(0xCDE703));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_24_LTEB,       uint32_t(0x23EF52));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_24_NRC,        uint32_t(0xF48279));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_30,            uint32_t(0x3B3CB540));
#endif
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_32,            uint32_t(0xCBF43926));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_32_BZIP2,      uint32_t(0xFC891918));
#ifdef CRCPP_INCLUDE_ESOTERIC_CRC_DEFINITIONS
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_32_C,          uint32_t(0xE3069283));
#endif
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_32_MPEG2,      uint32_t(0x0376E6E7));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_32_POSIX,      uint32_t(0x765E7680));
#ifdef CRCPP_INCLUDE_ESOTERIC_CRC_DEFINITIONS
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_32_Q,          uint32_t(0x3010BF7F));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_40_GSM,        uint64_t(0xD4164FC646));
    CRC_TEST(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_64,            uint64_t(0x6C40DF5F0B497347));
#endif

#ifdef CRCPP_INCLUDE_ESOTERIC_CRC_DEFINITIONS
    // Tests for non-8-bit multiples of data.

    // USB 2.0 uses 5-bit CRCs on 11 bits of data. Check one such example
    // found in the USB specification.
    static const unsigned char CRC_5_USB_TEST_DATA[] = { 0x10, 0x07 };
    CRC_PARTIAL_BYTE_TEST(CRC_5_USB_TEST_DATA, 11, CRC::CRC_5_USB, uint8_t(0x05));

    // Test 13-bit inputs used in 5G-NR
    static const unsigned char CRC_NR_TEST_DATA[] = { 0xBD, 0x10 };
    CRC_PARTIAL_BYTE_TEST(CRC_NR_TEST_DATA, 13, CRC::CRC_6_NR,  uint8_t(0x2F));
    CRC_PARTIAL_BYTE_TEST(CRC_NR_TEST_DATA, 13, CRC::CRC_11_NR, uint16_t(0x06C8));
#endif

    return returnCode;
}
