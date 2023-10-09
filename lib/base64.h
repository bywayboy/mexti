#ifndef _mexti_base64_h
#define _mexti_base64_h

#define MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL               -0x002A  /**< Output buffer too small. */
#define MBEDTLS_ERR_BASE64_INVALID_CHARACTER              -0x002C  /**< Invalid character in input. */


int base64_encode( unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen );
int base64_decode( unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen );

unsigned char crc8( unsigned char * data, int length );
#endif
