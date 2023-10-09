#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "base64.h"

static const unsigned char base64_enc_map[64] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/'
};

static const unsigned char base64_dec_map[128] =
{
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127,  62, 127, 127, 127,  63,  52,  53,
     54,  55,  56,  57,  58,  59,  60,  61, 127, 127,
    127,  64, 127, 127, 127,   0,   1,   2,   3,   4,
      5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
     25, 127, 127, 127, 127, 127, 127,  26,  27,  28,
     29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
     39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
     49,  50,  51, 127, 127, 127, 127, 127
};

#define BASE64_SIZE_T_MAX   ( (size_t) -1 ) /* SIZE_T_MAX is not standard */

/*
 * Encode a buffer into base64 format
 */
int base64_encode( unsigned char *dst, size_t dlen, size_t *olen,
                   const unsigned char *src, size_t slen )
{
    size_t i, n;
    int C1, C2, C3;
    unsigned char *p;

    if( slen == 0 )
    {
        *olen = 0;
        return( 0 );
    }

    n = slen / 3 + ( slen % 3 != 0 );

    if( n > ( BASE64_SIZE_T_MAX - 1 ) / 4 )
    {
        *olen = BASE64_SIZE_T_MAX;
        return( MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL );
    }

    n *= 4;

    if( dlen < n + 1 )
    {
        *olen = n + 1;
        return( MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL );
    }

    n = ( slen / 3 ) * 3;

    for( i = 0, p = dst; i < n; i += 3 )
    {
        C1 = *src++;
        C2 = *src++;
        C3 = *src++;

        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
        *p++ = base64_enc_map[(((C1 &  3) << 4) + (C2 >> 4)) & 0x3F];
        *p++ = base64_enc_map[(((C2 & 15) << 2) + (C3 >> 6)) & 0x3F];
        *p++ = base64_enc_map[C3 & 0x3F];
    }

    if( i < slen )
    {
        C1 = *src++;
        C2 = ( ( i + 1 ) < slen ) ? *src++ : 0;

        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
        *p++ = base64_enc_map[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];

        if( ( i + 1 ) < slen )
             *p++ = base64_enc_map[((C2 & 15) << 2) & 0x3F];
        else *p++ = '=';

        *p++ = '=';
    }

    *olen = p - dst;
    *p = 0;

    return( 0 );
}

/*
 * Decode a base64-formatted buffer
 */
int base64_decode( unsigned char *dst, size_t dlen, size_t *olen,
                   const unsigned char *src, size_t slen )
{
    size_t i, n;
    uint32_t j, x;
    unsigned char *p;

    /* First pass: check for validity and get output length */
    for( i = n = j = 0; i < slen; i++ )
    {
        /* Skip spaces before checking for EOL */
        x = 0;
        while( i < slen && src[i] == ' ' )
        {
            ++i;
            ++x;
        }

        /* Spaces at end of buffer are OK */
        if( i == slen )
            break;

        if( ( slen - i ) >= 2 &&
            src[i] == '\r' && src[i + 1] == '\n' )
            continue;

        if( src[i] == '\n' )
            continue;

        /* Space inside a line is an error */
        if( x != 0 )
            return( MBEDTLS_ERR_BASE64_INVALID_CHARACTER );

        if( src[i] == '=' && ++j > 2 )
            return( MBEDTLS_ERR_BASE64_INVALID_CHARACTER );

        if( src[i] > 127 || base64_dec_map[src[i]] == 127 )
            return( MBEDTLS_ERR_BASE64_INVALID_CHARACTER );

        if( base64_dec_map[src[i]] < 64 && j != 0 )
            return( MBEDTLS_ERR_BASE64_INVALID_CHARACTER );

        n++;
    }

    if( n == 0 )
    {
        *olen = 0;
        return( 0 );
    }

    /* The following expression is to calculate the following formula without
     * risk of integer overflow in n:
     *     n = ( ( n * 6 ) + 7 ) >> 3;
     */
    n = ( 6 * ( n >> 3 ) ) + ( ( 6 * ( n & 0x7 ) + 7 ) >> 3 );
    n -= j;

    if( dst == NULL || dlen < n )
    {
        *olen = n;
        return( MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL );
    }

   for( j = 3, n = x = 0, p = dst; i > 0; i--, src++ )
   {
        if( *src == '\r' || *src == '\n' || *src == ' ' )
            continue;

        j -= ( base64_dec_map[*src] == 64 );
        x  = ( x << 6 ) | ( base64_dec_map[*src] & 0x3F );

        if( ++n == 4 )
        {
            n = 0;
            if( j > 0 ) *p++ = (unsigned char)( x >> 16 );
            if( j > 1 ) *p++ = (unsigned char)( x >>  8 );
            if( j > 2 ) *p++ = (unsigned char)( x       );
        }
    }

    *olen = p - dst;

    return( 0 );
}

static unsigned char crctable[] = {
	 0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54,
	 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
	 0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06,
	 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
	 0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0,
	 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
	 0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2,
	 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
	 0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9,
	 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
	 0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B,
	 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
	 0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D,
	 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
	 0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F,
	 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
	 0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB,
	 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
	 0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9,
	 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
	  0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F,
	  0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
	  0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D,
	  0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
	  0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26,
	  0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
	  0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74,
	  0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
	  0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82,
	  0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
	  0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0,
	  0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9
};
#define CRC_TABLE_SIZE 	 (8 * 32)

static void initChecksum( unsigned char * crcvalue ){
	crcvalue[0] = 0;
}

static void updateChecksum(unsigned char * crcvalue, unsigned char * data, int length ){
	unsigned char crc;
	int m;

	crc = crcvalue[0];
	m = 0;
	while( (length--) != 0 )
	{
		crc = crctable[(crc&0xff) ^ ((data[m])&0xff)];
		m++;
	}
	crcvalue[0] = crc;
}	

static void finishChecksum( unsigned char * crcvalue ){
  unsigned char crcvalueT;
  crcvalueT = crcvalue[0];
  crcvalueT ^= 0;
  crcvalue[0] = crcvalueT;
}
	
unsigned char crc8( unsigned char * data, int length ){
  unsigned char crc[1] = {0};
  initChecksum( crc );
  updateChecksum( crc, data, length );
  finishChecksum( crc );
  return crc[0];
}