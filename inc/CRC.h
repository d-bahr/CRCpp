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

#ifndef CRCPP_CRC_H_
#define CRCPP_CRC_H_

#ifndef CRCPP_NO_CSTDINT

#   include <cstdint>

#   ifndef crcpp_uint8
        /// @brief unsigned 8-bit integer definition, used primarily for parameter definitions
#       define crcpp_uint8 uint8_t
#   endif

#   ifndef crcpp_uint16
        /// @brief unsigned 16-bit integer definition, used primarily for parameter definitions
#       define crcpp_uint16 uint16_t
#   endif

#   ifndef crcpp_uint32
        /// @brief unsigned 32-bit integer definition, used primarily for parameter definitions
#       define crcpp_uint32 uint32_t
#   endif

#   ifndef crcpp_uint64
        /// @brief unsigned 64-bit integer definition, used primarily for parameter definitions
#       define crcpp_uint64 uint64_t
#   endif

#   ifndef crcpp_size
        /// @brief unsigned size definition, used for specifying data sizes
#       define crcpp_size size_t
#   endif

#else // CRCPP_NO_CSTDINT

#   ifndef crcpp_uint8
        /// @brief unsigned 8-bit integer definition, used primarily for parameter definitions
#       define crcpp_uint8 unsigned char
#   endif

#   ifndef crcpp_uint16
        /// @brief unsigned 16-bit integer definition, used primarily for parameter definitions
#       define crcpp_uint16 unsigned short
#   endif

#   ifndef crcpp_uint32
        /// @brief unsigned 32-bit integer definition, used primarily for parameter definitions
#       define crcpp_uint32 unsigned long
#   endif

#   ifndef crcpp_uint64
        /// @brief unsigned 64-bit integer definition, used primarily for parameter definitions
#       define crcpp_uint64 unsigned long long
#   endif

#   ifndef crcpp_size
        /// @brief unsigned size definition, used for specifying data sizes
#       define crcpp_size crcpp_uint32
#   endif

#endif // CRCPP_NO_CSTDINT

#ifdef CRCPP_USE_NAMESPACE
namespace CRCPP
{
#endif

/**
    @brief Static class for computing CRCs.
*/
class CRC
{
public:
    /**
        @brief CRC parameters.
    */
    template <typename CRCType, crcpp_uint16 CRCWidth>
    struct Parameters
    {
        CRCType polynomial;   ///< CRC polynomial
        CRCType initialValue; ///< Initial CRC value
        CRCType finalXOR;     ///< Value to XOR with the final CRC
        bool reflectInput;    ///< true to reflect all input bytes
        bool reflectOutput;   ///< treu to reflect the output CRC (reflection occurs before the final XOR)
    };

    CRC() = delete;
    CRC(const CRC & other) = delete;
    CRC & operator=(const CRC & other) = delete;
    CRC(CRC && other) = delete;
    CRC & operator=(CRC && other) = delete;

    // The number of bits in CRCType must be at least as large as CRCWidth.
    // CRCType must be an unsigned integer type or a custom type with operator overloads.
    template <typename CRCType, crcpp_uint16 CRCWidth>
    static CRCType Calculate(const void * data, crcpp_size size, CRCType polynomial, CRCType initialValue, CRCType finalXOR, bool reflectInput, bool reflectOutput);

    template <typename CRCType, crcpp_uint16 CRCWidth>
    static CRCType Calculate(const void * data, crcpp_size size, const Parameters<CRCType, CRCWidth> & parameters);

    // Common CRCs up to 64 bits.
    // Note: Check values are the computed CRCs when given an ASCII input of "123456789"
    static constexpr Parameters< crcpp_uint8,  4> CRC_4_ITU         = Parameters< crcpp_uint8,  4>{ 0x3, 0x0, 0x0, true, true }; // check = 0x7
    static constexpr Parameters< crcpp_uint8,  5> CRC_5_EPC         = Parameters< crcpp_uint8,  5>{ 0x09, 0x09, 0x0, false, false }; // check = 0x00
    static constexpr Parameters< crcpp_uint8,  5> CRC_5_ITU         = Parameters< crcpp_uint8,  5>{ 0x15, 0x00, 0x00, true, true }; // check = 0x07
    static constexpr Parameters< crcpp_uint8,  5> CRC_5_USB         = Parameters< crcpp_uint8,  5>{ 0x05, 0x1F, 0x1F, true, true }; // check = 0x19
    static constexpr Parameters< crcpp_uint8,  6> CRC_6_CDMA2000A   = Parameters< crcpp_uint8,  6>{ 0x27, 0x3F, 0x00, false, false }; // check = 0x0D
    static constexpr Parameters< crcpp_uint8,  6> CRC_6_CDMA2000B   = Parameters< crcpp_uint8,  6>{ 0x07, 0x3F, 0x00, false, false }; // check = 0x3B
    static constexpr Parameters< crcpp_uint8,  6> CRC_6_ITU         = Parameters< crcpp_uint8,  6>{ 0x03, 0x00, 0x00, true, true }; // check = 0x06
    static constexpr Parameters< crcpp_uint8,  7> CRC_7             = Parameters< crcpp_uint8,  7>{ 0x09, 0x00, 0x00, false, false }; // check = 0x75
    static constexpr Parameters< crcpp_uint8,  8> CRC_8             = Parameters< crcpp_uint8,  8>{ 0x07, 0x00, 0x00, false, false }; // check = 0xF4
    static constexpr Parameters< crcpp_uint8,  8> CRC_8_EBU         = Parameters< crcpp_uint8,  8>{ 0x1D, 0xFF, 0x00, true, true }; // check = 0x97
    static constexpr Parameters< crcpp_uint8,  8> CRC_8_MAXIM       = Parameters< crcpp_uint8,  8>{ 0x31, 0x00, 0x00, true, true }; // check = 0xA1
    static constexpr Parameters< crcpp_uint8,  8> CRC_8_WCDMA       = Parameters< crcpp_uint8,  8>{ 0x9B, 0x00, 0x00, true, true }; // check = 0x25
    static constexpr Parameters<crcpp_uint16, 10> CRC_10            = Parameters<crcpp_uint16, 10>{ 0x233, 0x000, 0x000, false, false }; // check = 0x199
    static constexpr Parameters<crcpp_uint16, 10> CRC_10_CDMA2000   = Parameters<crcpp_uint16, 10>{ 0x3D9, 0x3FF, 0x000, false, false }; // check = 0x233
    static constexpr Parameters<crcpp_uint16, 11> CRC_11            = Parameters<crcpp_uint16, 11>{ 0x385, 0x01A, 0x000, false, false }; // check = 0x5A3
    static constexpr Parameters<crcpp_uint16, 12> CRC_12_3GPP       = Parameters<crcpp_uint16, 12>{ 0x80F, 0x000, 0x000, false, true }; // check = 0xDAF
    static constexpr Parameters<crcpp_uint16, 12> CRC_12_CDMA2000   = Parameters<crcpp_uint16, 12>{ 0xF13, 0xFFF, 0x000, false, false }; // check = 0xD4D
    static constexpr Parameters<crcpp_uint16, 12> CRC_12_DECT       = Parameters<crcpp_uint16, 12>{ 0x80F, 0x000, 0x000, false, false }; // check = 0xF5B
    static constexpr Parameters<crcpp_uint16, 13> CRC_13_BBC        = Parameters<crcpp_uint16, 13>{ 0x1CF5, 0x0000, 0x0000, false, false }; // check = 0x04FA
    static constexpr Parameters<crcpp_uint16, 15> CRC_15            = Parameters<crcpp_uint16, 15>{ 0x4599, 0x0000, 0x0000, false, false }; // check = 0x059E
    static constexpr Parameters<crcpp_uint16, 15> CRC_15_MPT1327    = Parameters<crcpp_uint16, 15>{ 0x6815, 0x0000, 0x0001, false, false }; // check = 0x2566
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_BUYPASS    = Parameters<crcpp_uint16, 16>{ 0x8005, 0x0000, 0x0000, false, false }; // check = 0xFEE8
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_CCITTFALSE = Parameters<crcpp_uint16, 16>{ 0x1021, 0xFFFF, 0x0000, false, false }; // check = 0x29B1
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_CDMA2000   = Parameters<crcpp_uint16, 16>{ 0xC867, 0xFFFF, 0x0000, false, false }; // check = 0x4C06
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_DECTR      = Parameters<crcpp_uint16, 16>{ 0x0589, 0x0000, 0x0001, false, false }; // check = 0x007E
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_DECTX      = Parameters<crcpp_uint16, 16>{ 0x0589, 0x0000, 0x0000, false, false }; // check = 0x007F
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_DNP        = Parameters<crcpp_uint16, 16>{ 0x3D65, 0x0000, 0xFFFF, true, true }; // check = 0xEA82
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_GENIBUS    = Parameters<crcpp_uint16, 16>{ 0x1021, 0xFFFF, 0xFFFF, false, false }; // check = 0xD64E
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_KERMIT     = Parameters<crcpp_uint16, 16>{ 0x1021, 0x0000, 0x0000, true, true }; // check = 0x2189
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_MAXIM      = Parameters<crcpp_uint16, 16>{ 0x8005, 0x0000, 0xFFFF, true, true }; // check = 0x44C2
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_MODBUS     = Parameters<crcpp_uint16, 16>{ 0x8005, 0xFFFF, 0x0000, true, true }; // check = 0x4B37
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_T10DIF     = Parameters<crcpp_uint16, 16>{ 0x8BB7, 0x0000, 0x0000, false, false }; // check = 0xD0DB
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_USB        = Parameters<crcpp_uint16, 16>{ 0x8005, 0xFFFF, 0xFFFF, true, true }; // check = 0xB4C8
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_X25        = Parameters<crcpp_uint16, 16>{ 0x1021, 0xFFFF, 0xFFFF, true, true }; // check = 0x906E
    static constexpr Parameters<crcpp_uint16, 16> CRC_16_XMODEM     = Parameters<crcpp_uint16, 16>{ 0x1021, 0x0000, 0x0000, false, false }; // check = 0x31C3
    static constexpr Parameters<crcpp_uint32, 17> CRC_17_CAN        = Parameters<crcpp_uint32, 17>{ 0x1685B, 0x00000, 0x00000, false, false }; // check = 0x04F03
    static constexpr Parameters<crcpp_uint32, 21> CRC_21_CAN        = Parameters<crcpp_uint32, 21>{ 0x102899, 0x000000, 0x000000, false, false }; // check = 0x0ED841
    static constexpr Parameters<crcpp_uint32, 24> CRC_24            = Parameters<crcpp_uint32, 24>{ 0x864CFB, 0xB704CE, 0x0000, false, false }; // check = 0x21CF02
    static constexpr Parameters<crcpp_uint32, 24> CRC_24_FLEXRAYA   = Parameters<crcpp_uint32, 24>{ 0x5D6DCB, 0xFEDCBA, 0x0000, false, false }; // check = 0x7979BD
    static constexpr Parameters<crcpp_uint32, 24> CRC_24_FLEXRAYB   = Parameters<crcpp_uint32, 24>{ 0x5D6DCB, 0xABCDEF, 0x0000, false, false }; // check = 0x1F23B8
    static constexpr Parameters<crcpp_uint32, 30> CRC_30            = Parameters<crcpp_uint32, 30>{ 0x2030B9C7, 0x3FFFFFFF, 0x00000000, false, false }; // check = 0x3B3CB540
    static constexpr Parameters<crcpp_uint32, 32> CRC_32            = Parameters<crcpp_uint32, 32>{ 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true }; // check = 0xCBF43926
    static constexpr Parameters<crcpp_uint32, 32> CRC_32_BZIP2      = Parameters<crcpp_uint32, 32>{ 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, false, false }; // check = 0xFC891918
    static constexpr Parameters<crcpp_uint32, 32> CRC_32_C          = Parameters<crcpp_uint32, 32>{ 0x1EDC6F41, 0xFFFFFFFF, 0xFFFFFFFF, true, true }; // check = 0xE3069283
    static constexpr Parameters<crcpp_uint32, 32> CRC_32_MPEG2      = Parameters<crcpp_uint32, 32>{ 0x04C11DB7, 0xFFFFFFFF, 0x00000000, false, false }; // check = 0x0376E6E7
    static constexpr Parameters<crcpp_uint32, 32> CRC_32_POSIX      = Parameters<crcpp_uint32, 32>{ 0x04C11DB7, 0x00000000, 0xFFFFFFFF, false, false }; // check = 0x765E7680
    static constexpr Parameters<crcpp_uint32, 32> CRC_32_Q          = Parameters<crcpp_uint32, 32>{ 0x814141AB, 0x00000000, 0x00000000, false, false }; // check = 0x3010BF7F
    static constexpr Parameters<crcpp_uint64, 40> CRC_40_GSM        = Parameters<crcpp_uint64, 40>{ 0x0004820009, 0x0000000000, 0xFFFFFFFFFF, false, false }; // check = 0xD4164FC646
    static constexpr Parameters<crcpp_uint64, 64> CRC_64            = Parameters<crcpp_uint64, 64>{ 0x42F0E1EBA9EA3693, 0x0000000000000000, 0x0000000000000000, false, false }; // check = 0x6C40DF5F0B497347

private:
    // Function to swap the bits in an integer.
    template <typename IntegerType>
    static IntegerType Reflect(IntegerType value, crcpp_uint16 numBits);

    // Two separate implementations to avoid repeatedly evaluating branches.
    // The algorithm used is branchless, with the except of a single branch to do an optional reflection of the CRC before the final XOR.
    template <typename CRCType, crcpp_uint16 CRCWidth>
    static CRCType InternalCalculate(const void * data, crcpp_size size, CRCType polynomial, CRCType initialValue, CRCType finalXOR, bool finalReflect);

    template <typename CRCType, crcpp_uint16 CRCWidth>
    static CRCType InternalCalculateReflected(const void * data, crcpp_size size, CRCType polynomial, CRCType initialValue, CRCType finalXOR, bool finalReflect);
};

/**
    @brief Computes a CRC.
    @note This function is a dispatch function. See InternalCalculate() and InternalCalculateReflected() for the actual implementation.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data
    @param[in] polynomial CRC polynomial
    @param[in] initialValue Initial CRC value
    @param[in] finalXOR Final value to XOR with the CRC
    @param[in] reflectInput true to reflect each byte of input data
    @param[in] reflectOutput true to reflect the final CRC (reflection occurs before the final XOR)
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRCType CRC::Calculate(const void * data, crcpp_size size, CRCType polynomial, CRCType initialValue, CRCType finalXOR, bool reflectInput, bool reflectOutput)
{
    if (reflectInput)
    {
        return InternalCalculateReflected<CRCType, CRCWidth>(data, size, polynomial, initialValue, finalXOR, reflectOutput);
    }
    else
    {
        return InternalCalculate<CRCType, CRCWidth>(data, size, polynomial, initialValue, finalXOR, reflectOutput);
    }
}

/**
    @brief Computes a CRC.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data
    @param[in] parameters CRC parameters
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRCType CRC::Calculate(const void * data, crcpp_size size, const Parameters<CRCType, CRCWidth> & parameters)
{
    return Calculate<CRCType, CRCWidth>(data, size, parameters.polynomial, parameters.initialValue, parameters.finalXOR, parameters.reflectInput, parameters.reflectOutput);
}

/**
    @brief Reflects (i.e. reverses the bits within) an integer value.
    @param[in] value Value to reflect
    @param[in] numBits Number of bits in the integer which will be reflected
    @tparam IntegerType Integer type of the value being reflected
    @return Reflected value
*/
template <typename IntegerType>
inline IntegerType CRC::Reflect(IntegerType value, crcpp_uint16 numBits)
{
    static constexpr IntegerType One(1);

    IntegerType reversedValue(0);

    // Smart compilers will probably unroll this loop for small numeric types
    for (crcpp_uint16 i = 0; i < numBits; ++i)
    {
        reversedValue = (reversedValue << One) | (value & One);
        value = value >> One;
    }

    return reversedValue;
}

/**
    @brief Computes a CRC.
    @note This function is designed and optimized for non-reflected input.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data
    @param[in] polynomial CRC polynomial
    @param[in] initialValue Initial CRC value
    @param[in] finalXOR Final value to XOR with the CRC
    @param[in] finalReflect true to reflect the final CRC (reflection occurs before the final XOR)
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRCType CRC::InternalCalculate(const void * data, crcpp_size size, CRCType polynomial, CRCType initialValue, CRCType finalXOR, bool finalReflect)
{
    static_assert(std::numeric_limits<CRCType>::digits >= CRCWidth, "CRCWidth must be no larger than the number of bits in CRCType.");
    
    // These constants will be compiled out and replaced with hard numbers
    static constexpr CRCType One(1);
    static constexpr CRCType WidthMinusOne(CRCWidth - One);

    // For masking off the highest order bit.
    static constexpr CRCType HighestOrderBit(One << WidthMinusOne);

#if defined(WIN32) || defined(_WIN32)
    // Stop Visual Studio from giving a warning; the conditional expression should prevent the warning from occuring, because
    // it is evaluating a constexpr... but it doesn't.
#   pragma warning (push)
#   pragma warning (disable : 4293)
#endif

    // For masking off the bits for the CRC (in the event that the number of bits in CRCType is larger than CRCWidth)
    static constexpr CRCType BitMask = (std::numeric_limits<CRCType>::digits == CRCWidth) ? (CRCType(-1)) : ((One << CRCWidth) - One);

#if defined(WIN32) || defined(_WIN32)
#   pragma warning (pop)
#endif

    const unsigned char * current = reinterpret_cast<const unsigned char *>(data);
    CRCType doXOR;
    unsigned char nextValue;

    while (size--)
    {
        nextValue = *current++;
        
        // A good compiler should be able to unroll this loop.
        for (crcpp_uint8 i = 0x00; i < 0x08; ++i)
        {
            // Do the XOR only if the next bit from the input data is opposite of the highest order bit in the running CRC.
            // doXOR can only be 0 or 1.
            doXOR = ((initialValue & HighestOrderBit) >> WidthMinusOne) ^ CRCType((nextValue & 0x80) >> 0x07);
            
            // Clever way to avoid a branch at the expense of a multiplication. This code is equivalent to the following:
            // if (doXOR)
            //     initialValue = (initialValue << One) ^ polynomial;
            // else
            //     initialValue <<= One;
            initialValue = (initialValue << One) ^ (doXOR * polynomial);

            // Shift so we evaluate the next bit in the input data.
            nextValue = (nextValue << 0x01);
        }
    }

    if (finalReflect)
    {
        initialValue = Reflect(initialValue, CRCWidth);
    }
    
    return (initialValue ^ finalXOR) & BitMask;
}

/**
    @brief Computes a CRC.
    @note This function is designed and optimized for reflected input.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data
    @param[in] polynomial CRC polynomial
    @param[in] initialValue Initial CRC value
    @param[in] finalXOR Final value to XOR with the CRC
    @param[in] finalReflect true to reflect the final CRC (reflection occurs before the final XOR)
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRCType CRC::InternalCalculateReflected(const void * data, crcpp_size size, CRCType polynomial, CRCType initialValue, CRCType finalXOR, bool finalReflect)
{
    static_assert(std::numeric_limits<CRCType>::digits >= CRCWidth, "CRCWidth must be no larger than the number of bits in CRCType.");

    // These constants will be compiled out and replaced with hard numbers
    static constexpr CRCType One(1);
    static constexpr CRCType WidthMinusOne(CRCWidth - One);

    // For masking off the highest order bit.
    static constexpr CRCType HighestOrderBit(One << WidthMinusOne);

#if defined(WIN32) || defined(_WIN32)
    // Stop Visual Studio from giving a warning; the conditional expression should prevent the warning from occuring, because
    // it is evaluating a constexpr... but it doesn't.
#   pragma warning (push)
#   pragma warning (disable : 4293)
#endif

    // For masking off the bits for the CRC (in the event that the number of bits in CRCType is larger than CRCWidth)
    static constexpr CRCType BitMask = (std::numeric_limits<CRCType>::digits == CRCWidth) ? (CRCType(-1)) : ((One << CRCType(CRCWidth)) - One);

#if defined(WIN32) || defined(_WIN32)
#   pragma warning (pop)
#endif

    const unsigned char * current = reinterpret_cast<const unsigned char *>(data);
    CRCType doXOR;
    unsigned char nextValue;

    while (size--)
    {
        // Rather than do the reflection here, which would require more instructions to flip the bits around,
        // the loop below is slightly different to 'pull' the bits out of nextValue starting from the LSB instead of the MSB.
        nextValue = *current++;

        // A good compiler should be able to unroll this loop.
        for (crcpp_uint8 i = 0x00; i < 0x08; ++i)
        {
            // Do the XOR only if the next bit from the input data is opposite of the highest order bit in the running CRC.
            // doXOR can only be 0 or 1.
            doXOR = ((initialValue & HighestOrderBit) >> WidthMinusOne) ^ CRCType(nextValue & 0x01);

            // Clever way to avoid a branch at the expense of a multiplication. This code is equivalent to the following:
            // if (doXOR)
            //     initialValue = (initialValue << One) ^ polynomial;
            // else
            //     initialValue <<= One;
            initialValue = (initialValue << One) ^ (doXOR * polynomial);

            // Shift so we evaluate the next bit in the input data.
            nextValue = (nextValue >> 0x01);
        }
    }

    if (finalReflect)
    {
        initialValue = Reflect(initialValue, CRCWidth);
    }

    return (initialValue ^ finalXOR) & BitMask;
}

#ifdef CRCPP_USE_NAMESPACE
}
#endif

#endif // CRCPP_CRC_H_
