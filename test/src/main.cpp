/**
    @file CRC.h
    @author Daniel Bahr
    @version 0.0.1.1
    @copyright
    @parblock
        CRC++
        Copyright (c) 2016, Daniel Bahr
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

#include <iomanip>
#include <iostream>
#include <string>

#ifdef CRCPP_USE_NAMESPACE
using CRCPP::CRC;
#endif

/**
    @brief Primary CRC test function. Calculates a CRC over some specified data and compares the calculated CRC to an expected CRC.
    @note On failure, a message is written to std::cerr.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data
    @param[in] parameters CRC parameters
    @param[in] expectedCRC Expected CRC of the data, used for comparison
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, uint16_t CRCWidth>
static void CRCTest(const void * data, size_t size, const CRC::Parameters<CRCType, CRCWidth> & parameters, CRCType expectedCRC)
{
    CRCType computedCRC = CRC::Calculate(data, size, parameters);

    if (expectedCRC != computedCRC)
    {
        std::string dataAsASCII(reinterpret_cast<const char *>(data), reinterpret_cast<const char *>(data) + size);

        std::cerr << "Invalid CRC: \"" << dataAsASCII << "\"" << std::endl;

        std::cerr << "CRC parameters: " << std::endl;
        std::cerr << "\tPolynomial: "     << std::setw(CRCWidth) << std::hex << uint64_t(parameters.polynomial)   << std::endl;
        std::cerr << "\tInitial value: "  << std::setw(CRCWidth) << std::hex << uint64_t(parameters.initialValue) << std::endl;
        std::cerr << "\tFinal XOR: "      << std::setw(CRCWidth) << std::hex << uint64_t(parameters.finalXOR)     << std::endl;
        std::cerr << "\tReflect input: "  << (parameters.reflectInput  ? "true" : "false") << std::endl;
        std::cerr << "\tReflect output: " << (parameters.reflectOutput ? "true" : "false") << std::endl;

        std::cerr << "Expected CRC: " << std::setw(CRCWidth) << std::hex << uint64_t(expectedCRC) << std::endl;
        std::cerr << "Computed CRC: " << std::setw(CRCWidth) << std::hex << uint64_t(computedCRC) << std::endl;
    }
}

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

    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_4_ITU,         uint8_t(0x7));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_5_EPC,         uint8_t(0x00));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_5_ITU,         uint8_t(0x07));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_5_USB,         uint8_t(0x19));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_6_CDMA2000A,   uint8_t(0x0D));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_6_CDMA2000B,   uint8_t(0x3B));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_6_ITU,         uint8_t(0x06));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_7,             uint8_t(0x75));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_8,             uint8_t(0xF4));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_8_EBU,         uint8_t(0x97));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_8_MAXIM,       uint8_t(0xA1));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_8_WCDMA,       uint8_t(0x25));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_10,            uint16_t(0x199));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_10_CDMA2000,   uint16_t(0x233));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_11,            uint16_t(0x5A3));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_12_3GPP,       uint16_t(0xDAF));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_12_CDMA2000,   uint16_t(0xD4D));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_12_DECT,       uint16_t(0xF5B));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_13_BBC,        uint16_t(0x04FA));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_15,            uint16_t(0x059E));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_15_MPT1327,    uint16_t(0x2566));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_BUYPASS,    uint16_t(0xFEE8));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_CCITTFALSE, uint16_t(0x29B1));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_CDMA2000,   uint16_t(0x4C06));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_DECTR,      uint16_t(0x007E));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_DECTX,      uint16_t(0x007F));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_DNP,        uint16_t(0xEA82));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_GENIBUS,    uint16_t(0xD64E));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_KERMIT,     uint16_t(0x2189));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_MAXIM,      uint16_t(0x44C2));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_MODBUS,     uint16_t(0x4B37));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_T10DIF,     uint16_t(0xD0DB));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_USB,        uint16_t(0xB4C8));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_X25,        uint16_t(0x906E));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_16_XMODEM,     uint16_t(0x31C3));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_17_CAN,        uint32_t(0x04F03));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_21_CAN,        uint32_t(0x0ED841));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_24,            uint32_t(0x21CF02));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_24_FLEXRAYA,   uint32_t(0x7979BD));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_24_FLEXRAYB,   uint32_t(0x1F23B8));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_30,            uint32_t(0x3B3CB540));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_32,            uint32_t(0xCBF43926));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_32_BZIP2,      uint32_t(0xFC891918));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_32_C,          uint32_t(0xE3069283));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_32_MPEG2,      uint32_t(0x0376E6E7));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_32_POSIX,      uint32_t(0x765E7680));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_32_Q,          uint32_t(0x3010BF7F));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_40_GSM,        uint64_t(0xD4164FC646));
    CRCTest(CRC_CHECK_DATA, CRC_CHECK_SIZE, CRC::CRC_64,            uint64_t(0x6C40DF5F0B497347));

    return 0;
}
