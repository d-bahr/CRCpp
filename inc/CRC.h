/**
    @file CRC.h
    @author Daniel Bahr
    @version 0.1.0.5
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

#include <climits>     // Includes CHAR_BIT
#include <cstddef>     // Includes ::std::size_t
#include <cstdint>     // Includes ::std::uint8_t, ::std::uint16_t, ::std::uint32_t, ::std::uint64_t
#include <type_traits> // Includes ::std::numeric_limits
#include <utility>     // Includes ::std::move

/**
    CRC++ allows client code to configure CRC++ by setting various #defines before #including this header file:

        #define crcpp_uint8             - Specifies the type used to store CRCs that have a width of 8 bits or less.
                                          This type is not used in CRC calculations. Defaults to ::std::uint8_t.
        #define crcpp_uint16            - Specifies the type used to store CRCs that have a width between 9 and 16 bits (inclusive).
                                          This type is not used in CRC calculations. Defaults to ::std::uint16_t.
        #define crcpp_uint32            - Specifies the type used to store CRCs that have a width between 17 and 32 bits (inclusive).
                                          This type is not used in CRC calculations. Defaults to ::std::uint32_t.
        #define crcpp_uint64            - Specifies the type used to store CRCs that have a width between 33 and 64 bits (inclusive).
                                          This type is not used in CRC calculations. Defaults to ::std::uint64_t.
        #define crcpp_size              - Specifies the type used to store CRCs that have a width between 33 and 64 bits (inclusive).
                                          This type is used for loop iteration and function signatures only. Defaults to ::std::size_t.
        #define CRCPP_USE_NAMESPACE     - Define to place all CRC++ code within the ::CRCPP namespace.
        #define CRCPP_BRANCHLESS        - Define to enable a branchless CRC implementation. The branchless implementation uses a single integer
                                          multiplication in the bit-by-bit calculation instead of a small conditional. The branchless implementation
                                          may be faster on processor architectures which support single-instruction integer multiplication.
*/

#ifndef crcpp_uint8
    /// @brief unsigned 8-bit integer definition, used primarily for parameter definitions
#   define crcpp_uint8 ::std::uint8_t
#endif

#ifndef crcpp_uint16
    /// @brief unsigned 16-bit integer definition, used primarily for parameter definitions
#   define crcpp_uint16 ::std::uint16_t
#endif

#ifndef crcpp_uint32
    /// @brief unsigned 32-bit integer definition, used primarily for parameter definitions
#   define crcpp_uint32 ::std::uint32_t
#endif

#ifndef crcpp_uint64
    /// @brief unsigned 64-bit integer definition, used primarily for parameter definitions
#   define crcpp_uint64 ::std::uint64_t
#endif

#ifndef crcpp_size
    /// @brief unsigned size definition, used for specifying data sizes
#   define crcpp_size ::std::size_t
#endif

#ifdef CRCPP_USE_NAMESPACE
namespace CRCPP
{
#endif

/**
    @brief Static class for computing CRCs.
    @note This class supports computation of full and multi-part CRCs, using a bit-by-bit algorithm or a
        byte-by-byte lookup table. The CRCs are calculated using as many optimizations as is reasonable.
        The constexpr keyword is used liberally so that many calculations are performed at compile-time
        instead of at runtime.
*/
class CRC
{
public:
    // Forward declaration
    template <typename CRCType, crcpp_uint16 CRCWidth>
    struct Table;

    /**
        @brief CRC parameters.
    */
    template <typename CRCType, crcpp_uint16 CRCWidth>
    struct Parameters
    {
        // Note: constructors are not allowed here; otherwise constexpr keyword won't work and
        // we would need a source file to define the fixed Parameters definitions.

        CRCType polynomial;   ///< CRC polynomial
        CRCType initialValue; ///< Initial CRC value
        CRCType finalXOR;     ///< Value to XOR with the final CRC
        bool reflectInput;    ///< true to reflect all input bytes
        bool reflectOutput;   ///< true to reflect the output CRC (reflection occurs before the final XOR)

        Table<CRCType, CRCWidth> MakeTable() const;
    };

    /**
        @brief CRC lookup table. After construction, the CRC parameters are fixed.
        @note A CRC table can be used for multiple CRC calculations.
    */
    template <typename CRCType, crcpp_uint16 CRCWidth>
    struct Table
    {
        // Constructors are intentionally NOT marked explicit.
        Table(const Parameters<CRCType, CRCWidth> & parameters);
        Table(Parameters<CRCType, CRCWidth> && parameters);
        ~Table() = default;
        Table(const Table<CRCType, CRCWidth> & other) noexcept = default;
        Table(Table<CRCType, CRCWidth> && other) noexcept = default;
        Table<CRCType, CRCWidth> & operator=(const Table<CRCType, CRCWidth> & rhs) noexcept = default;
        Table<CRCType, CRCWidth> & operator=(Table<CRCType, CRCWidth> && rhs) noexcept = default;

        const Parameters<CRCType, CRCWidth> & GetParameters() const;

        const CRCType * GetTable() const;

        CRCType operator[](unsigned char index) const;

    private:
        void InitTable();

        Parameters<CRCType, CRCWidth> parameters; ///< CRC parameters used to construct the table
        CRCType table[1 << CHAR_BIT];             ///< CRC lookup table
    };

    // The number of bits in CRCType must be at least as large as CRCWidth.
    // CRCType must be an unsigned integer type or a custom type with operator overloads.
    template <typename CRCType, crcpp_uint16 CRCWidth>
    static CRCType Calculate(const void * data, crcpp_size size, const Parameters<CRCType, CRCWidth> & parameters);

    template <typename CRCType, crcpp_uint16 CRCWidth>
    static CRCType Calculate(const void * data, crcpp_size size, const Parameters<CRCType, CRCWidth> & parameters, CRCType crc);

    template <typename CRCType, crcpp_uint16 CRCWidth>
    static CRCType Calculate(const void * data, crcpp_size size, const Table<CRCType, CRCWidth> & lookupTable);

    template <typename CRCType, crcpp_uint16 CRCWidth>
    static CRCType Calculate(const void * data, crcpp_size size, const Table<CRCType, CRCWidth> & lookupTable, CRCType crc);

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

    CRC() = delete;
    CRC(const CRC & other) = delete;
    CRC & operator=(const CRC & other) = delete;
    CRC(CRC && other) = delete;
    CRC & operator=(CRC && other) = delete;

private:
    template <typename IntegerType>
    static IntegerType Reflect(IntegerType value, crcpp_uint16 numBits);

    template <typename CRCType, crcpp_uint16 CRCWidth>
    static CRCType Finalize(CRCType remainder, CRCType finalXOR, bool reflectOutput);

    template <typename CRCType, crcpp_uint16 CRCWidth>
    static CRCType UndoFinalize(CRCType remainder, CRCType finalXOR, bool reflectOutput);

    template <typename CRCType, crcpp_uint16 CRCWidth>
    static CRCType CalculateRemainder(const void * data, crcpp_size size, const Parameters<CRCType, CRCWidth> & parameters, CRCType remainder);

    template <typename CRCType, crcpp_uint16 CRCWidth>
    static CRCType CalculateRemainder(const void * data, crcpp_size size, const Table<CRCType, CRCWidth> & lookupTable, CRCType remainder);

    template <typename IntegerType>
    static constexpr IntegerType BoundedConstexprValue(IntegerType x);
};

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
    CRCType remainder = CalculateRemainder(data, size, parameters, parameters.initialValue);

    // No need to mask the remainder here; the mask will be applied in the Finalize() function.

    return Finalize<CRCType, CRCWidth>(remainder, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);
}
/**
    @brief Appends additional data to a previous CRC calculation.
    @note This function can be used to compute multi-part CRCs.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data
    @param[in] parameters CRC parameters
    @param[in] crc CRC from a previous calculation
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRCType CRC::Calculate(const void * data, crcpp_size size, const Parameters<CRCType, CRCWidth> & parameters, CRCType crc)
{
    CRCType remainder = UndoFinalize<CRCType, CRCWidth>(crc, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);

    remainder = CalculateRemainder(data, size, parameters, remainder);

    // No need to mask the remainder here; the mask will be applied in the Finalize() function.

    return Finalize<CRCType, CRCWidth>(remainder, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);
}

/**
    @brief Computes a CRC via a lookup table.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data
    @param[in] lookupTable CRC lookup table
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRCType CRC::Calculate(const void * data, crcpp_size size, const Table<CRCType, CRCWidth> & lookupTable)
{
    const Parameters<CRCType, CRCWidth> & parameters = lookupTable.GetParameters();

    CRCType remainder = CalculateRemainder(data, size, lookupTable, parameters.initialValue);

    // No need to mask the remainder here; the mask will be applied in the Finalize() function.

    return Finalize<CRCType, CRCWidth>(remainder, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);
}

/**
    @brief Appends additional data to a previous CRC calculation using a lookup table.
    @note This function can be used to compute multi-part CRCs.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data
    @param[in] lookupTable CRC lookup table
    @param[in] crc CRC from a previous calculation
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRCType CRC::Calculate(const void * data, crcpp_size size, const Table<CRCType, CRCWidth> & lookupTable, CRCType crc)
{
    const Parameters<CRCType, CRCWidth> & parameters = lookupTable.GetParameters();

    CRCType remainder = UndoFinalize<CRCType, CRCWidth>(crc, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);

    remainder = CalculateRemainder(data, size, lookupTable, remainder);

    // No need to mask the remainder here; the mask will be applied in the Finalize() function.

    return Finalize<CRCType, CRCWidth>(remainder, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);
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
    IntegerType reversedValue(0);

    for (crcpp_uint16 i = 0; i < numBits; ++i)
    {
        reversedValue = (reversedValue << 1) | (value & 1);
        value >>= 1;
    }

    return reversedValue;
}

/**
    @brief Computes the final reflection and XOR of a CRC remainder.
    @param[in] remainder CRC remainder to reflect and XOR
    @param[in] finalXOR Final value to XOR with the remainder
    @param[in] reflectInput true to reflect each byte of the remainder before the XOR
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return Final CRC
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRCType CRC::Finalize(CRCType remainder, CRCType finalXOR, bool reflectOutput)
{
    // For masking off the bits for the CRC (in the event that the number of bits in CRCType is larger than CRCWidth)
    static constexpr CRCType BIT_MASK = (CRCType(1) << (CRCWidth - CRCType(1))) |
                                       ((CRCType(1) << (CRCWidth - CRCType(1))) - CRCType(1));

    if (reflectOutput)
    {
        remainder = Reflect(remainder, CRCWidth);
    }

    return (remainder ^ finalXOR) & BIT_MASK;
}

/**
    @brief Undoes the process of computing the final reflection and XOR of a CRC remainder.
    @note This function allows for computation of multi-part CRCs
    @note Calling UndoFinalize() followed by Finalize() (or vice versa) will always return the original remainder value:

        CRCType x = ...;
        CRCType y = Finalize(x, finalXOR, reflectOutput);
        CRCType z = UndoFinalize(y, finalXOR, reflectOutput);
        // x will always equal z here.

    @param[in] crc Reflected and XORed CRC
    @param[in] finalXOR Final value XORed with the remainder
    @param[in] reflectOutput true if the remainder is to be reflected
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return Un-finalized CRC remainder
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRCType CRC::UndoFinalize(CRCType crc, CRCType finalXOR, bool reflectOutput)
{
    // For masking off the bits for the CRC (in the event that the number of bits in CRCType is larger than CRCWidth)
    static constexpr CRCType BIT_MASK = (CRCType(1) << (CRCWidth - CRCType(1))) |
                                       ((CRCType(1) << (CRCWidth - CRCType(1))) - CRCType(1));

    crc = (crc & BIT_MASK) ^ finalXOR;

    if (reflectOutput)
    {
        crc = Reflect(crc, CRCWidth);
    }

    return crc;
}

/**
    @brief Computes a CRC remainder.
    @param[in] data Data over which the remainder will be computed
    @param[in] size Size of the data
    @param[in] parameters CRC parameters
    @param[in] remainder Running CRC remainder. Can be an initial value or the result of a previous CRC remainder calculation.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC remainder
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRCType CRC::CalculateRemainder(const void * data, crcpp_size size, const Parameters<CRCType, CRCWidth> & parameters, CRCType remainder)
{
    // This static_assert is put here because this function will always be compiled in no matter what
    // the template parameters are and whether or not a table lookup or bit-by-bit algorithm is used.
    static_assert(::std::numeric_limits<CRCType>::digits >= CRCWidth, "CRCType is too small to contain a CRC of width CRCWidth.");

    const unsigned char * current = reinterpret_cast<const unsigned char *>(data);

    // Slightly different implementations based on the parameters. The current implementations try to eliminate as much
    // computation from the inner loop (looping over each bit) as possible.
    if (parameters.reflectInput)
    {
        CRCType polynomial = CRC::Reflect(parameters.polynomial, CRCWidth);
        while (size--)
        {
            remainder ^= *current++;

            // An optimizing compiler might choose to unroll this loop.
            for (crcpp_size i = 0; i < CHAR_BIT; ++i)
            {
#ifdef CRCPP_BRANCHLESS
                // Clever way to avoid a branch at the expense of a multiplication. This code is equivalent to the following:
                // if (remainder & 1)
                //     remainder = (remainder >> 1) ^ polynomial;
                // else
                //     remainder >>= 1;
                remainder = (remainder >> 1) ^ ((remainder & 1) * polynomial);
#else
                remainder = (remainder & 1) ? ((remainder >> 1) ^ polynomial) : (remainder >> 1);
#endif
            }
        }
    }
    else if (CRCWidth >= CHAR_BIT)
    {
        static constexpr CRCType CRC_HIGHEST_BIT_MASK(CRCType(1) << (CRCWidth - CRCType(1)));
        static constexpr CRCType SHIFT(BoundedConstexprValue(CRCWidth - CHAR_BIT));

        while (size--)
        {
            remainder ^= (static_cast<CRCType>(*current++) << SHIFT);

            // An optimizing compiler might choose to unroll this loop.
            for (crcpp_size i = 0; i < CHAR_BIT; ++i)
            {
#ifdef CRCPP_BRANCHLESS
                // Clever way to avoid a branch at the expense of a multiplication. This code is equivalent to the following:
                // if (remainder & CRC_HIGHEST_BIT_MASK)
                //     remainder = (remainder << 1) ^ parameters.polynomial;
                // else
                //     remainder <<= 1;
                remainder = (remainder << 1) ^ (((remainder >> WidthMinusOne) & 1) * parameters.polynomial);
#else
                remainder = (remainder & CRC_HIGHEST_BIT_MASK) ? ((remainder << 1) ^ parameters.polynomial) : (remainder << 1);
#endif
            }
        }
    }
    else
    {
        static constexpr CRCType CHAR_BIT_HIGHEST_BIT_MASK(CRCType(1) << (CHAR_BIT - 1));
        static constexpr CRCType SHIFT(BoundedConstexprValue(CHAR_BIT - CRCWidth));

        CRCType polynomial = parameters.polynomial << SHIFT;
        remainder <<= SHIFT;

        while (size--)
        {
            remainder ^= *current++;

            // An optimizing compiler might choose to unroll this loop.
            for (crcpp_size i = 0; i < CHAR_BIT; ++i)
            {
#ifdef CRCPP_BRANCHLESS
                // Clever way to avoid a branch at the expense of a multiplication. This code is equivalent to the following:
                // if (remainder & CHAR_BIT_HIGHEST_BIT_MASK)
                //     remainder = (remainder << 1) ^ polynomial;
                // else
                //     remainder <<= 1;
                remainder = (remainder << 1) ^ (((remainder >> CharBitMinusOne) & 1) * polynomial);
#else
                remainder = (remainder & CHAR_BIT_HIGHEST_BIT_MASK) ? ((remainder << 1) ^ polynomial) : (remainder << 1);
#endif
            }
        }

        remainder >>= SHIFT;
    }

    return remainder;
}

/**
    @brief Computes a CRC remainder using lookup table.
    @param[in] data Data over which the remainder will be computed
    @param[in] size Size of the data
    @param[in] lookupTable CRC lookup table
    @param[in] remainder Running CRC remainder. Can be an initial value or the result of a previous CRC remainder calculation.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC remainder
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRCType CRC::CalculateRemainder(const void * data, crcpp_size size, const Table<CRCType, CRCWidth> & lookupTable, CRCType remainder)
{
    const unsigned char * current = reinterpret_cast<const unsigned char *>(data);

    if (lookupTable.GetParameters().reflectInput)
    {
        while (size--)
        {
#if defined(WIN32) || defined(_WIN32) || defined(WINCE)
    // Disable warning about data loss when doing (remainder >> CHAR_BIT) when
    // remainder is one byte long. The algorithm is still correct in this case,
    // though it's possible that one additional machine instruction will be executed.
#   pragma warning (push)
#   pragma warning (disable : 4333)
#endif
            remainder = (remainder >> CHAR_BIT) ^ lookupTable[static_cast<unsigned char>(remainder ^ *current++)];
#if defined(WIN32) || defined(_WIN32) || defined(WINCE)
#   pragma warning (pop)
#endif
        }
    }
    else if (CRCWidth >= CHAR_BIT)
    {
        static constexpr CRCType SHIFT(BoundedConstexprValue(CRCWidth - CHAR_BIT));

        while (size--)
        {
            remainder = (remainder << CHAR_BIT) ^ lookupTable[static_cast<unsigned char>((remainder >> SHIFT) ^ *current++)];
        }
    }
    else
    {
        static constexpr CRCType SHIFT(BoundedConstexprValue(CHAR_BIT - CRCWidth));

        remainder <<= SHIFT;

        while (size--)
        {
            // Note: no need to mask here since remainder is guaranteed to fit in a single byte.
            remainder = lookupTable[static_cast<unsigned char>(remainder ^ *current++)];
        }

        remainder >>= SHIFT;
    }

    return remainder;
}

/**
    @brief Function to force a constexpr value to be >= 0.
    @note This function is used to avoid compiler warnings because all constexpr values are evaluated
        in a function even in a branch will never be executed. This also means we don't need #pragmas
        to get rid of warnings, but it still can be computed at compile-time. Win-win!
    @param[in] parameters CRC parameters
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename IntegerType>
inline constexpr IntegerType CRC::BoundedConstexprValue(IntegerType x)
{
    return (x < IntegerType(0)) ? IntegerType(0) : x;
}

/**
    @brief Returns a CRC lookup table construct using these CRC parameters.
    @note This function primarily exists to allow use of the auto keyword instead of instantiating
        a table directly, since template parameters are not inferred in constructors.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC lookup table
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRC::Table<CRCType, CRCWidth> CRC::Parameters<CRCType, CRCWidth>::MakeTable() const
{
    // This should take advantage of RVO and optimize out the copy.
    return CRC::Table<CRCType, CRCWidth>(*this);
}

/**
    @brief Constructs a CRC table from a set of CRC parameters
    @param[in] parameters CRC parameters
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRC::Table<CRCType, CRCWidth>::Table(const Parameters<CRCType, CRCWidth> & parameters) :
    parameters(parameters)
{
    InitTable();
}

/**
    @brief Constructs a CRC table from a set of CRC parameters
    @param[in] parameters CRC parameters
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRC::Table<CRCType, CRCWidth>::Table(Parameters<CRCType, CRCWidth> && parameters) :
    parameters(::std::move(parameters))
{
    InitTable();
}

/**
    @brief Gets the CRC parameters used to construct the CRC table
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC parameters
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline const CRC::Parameters<CRCType, CRCWidth> & CRC::Table<CRCType, CRCWidth>::GetParameters() const
{
    return parameters;
}

/**
    @brief Gets the CRC table
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC table
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline const CRCType * CRC::Table<CRCType, CRCWidth>::GetTable() const
{
    return table;
}

/**
    @brief Gets an entry in the CRC table
    @param[in] index Index into the CRC table
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC table entry
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline CRCType CRC::Table<CRCType, CRCWidth>::operator[](unsigned char index) const
{
    return table[index];
}

/**
    @brief Initializes a CRC table.
    @param[in] polynomial CRC polynomial
    @param[in] reflectInput true to reflect each byte of input data
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, crcpp_uint16 CRCWidth>
inline void CRC::Table<CRCType, CRCWidth>::InitTable()
{
    // For masking off the bits for the CRC (in the event that the number of bits in CRCType is larger than CRCWidth)
    static constexpr CRCType BIT_MASK((CRCType(1) << (CRCWidth - CRCType(1))) |
                                     ((CRCType(1) << (CRCWidth - CRCType(1))) - CRCType(1)));

    CRCType crc;
    unsigned char byte = 0;

    // Loop over each dividend (each possible number storable in an unsigned char)
    do
    {
        crc = CRC::CalculateRemainder<CRCType, CRCWidth>(&byte, sizeof(byte), parameters, CRCType(0));

        // This mask might not be necessary; all unit tests pass with this line commented out,
        // but that might just be a coincidence based on the CRC parameters used for testing.
        // In any case, this is harmless to leave in and only adds a single machine instruction per loop iteration.
        crc &= BIT_MASK;

        if (!parameters.reflectInput && CRCWidth < CHAR_BIT)
        {
            // Undo the special operation at the end of the CalculateRemainder()
            // function for non-reflected CRCs < CHAR_BIT.
            crc <<= (CHAR_BIT - CRCWidth);
        }

        table[byte] = crc;
    }
    while (++byte);
}

#ifdef CRCPP_USE_NAMESPACE
}
#endif

#endif // CRCPP_CRC_H_
