/* doin extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>
#include "php.h"
#include "ext/standard/info.h"


#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <time.h>
//#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include "xxtea.h"


//#define XXTEA_MX (z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (k[p & 3 ^ e] ^ z)
#define XXTEA_MX ((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((sum ^ y) + (k[(p & 3) ^ e] ^ z))
//#define XXTEA_MX (((z >> 5) ^ (y << 2)) + ((y >> 3) ^ (z << 4))) ^ ((sum ^ y) + (k[(p & 3) ^ e] ^ z))
#define XXTEA_DELTA 0x9e3779b9

void xxtea_long_encrypt(xxtea_long *v, xxtea_long len, xxtea_long *k) {
	xxtea_long n = len - 1;
	xxtea_long z = v[n], y = v[0], p, q = 6 + 52 / (n + 1), sum = 0, e;
	if (n < 1) {
		return;
	}
	while (0 < q--) {
		sum += XXTEA_DELTA;
		e = sum >> 2 & 3;
		for (p = 0; p < n; p++) {
			y = v[p + 1];
			z = v[p] += XXTEA_MX;
		}
		y = v[0];
		z = v[n] += XXTEA_MX;
	}
}

void xxtea_long_decrypt(xxtea_long *v, xxtea_long len, xxtea_long *k) {
	xxtea_long n = len - 1;
	xxtea_long z = v[n], y = v[0], p, q = 6 + 52 / (n + 1), sum = q * XXTEA_DELTA, e;
	if (n < 1) {
		return;
	}
	while (sum != 0) {
		e = sum >> 2 & 3;
		for (p = n; p > 0; p--) {
			z = v[p - 1];
			y = v[p] -= XXTEA_MX;
		}
		z = v[n];
		y = v[0] -= XXTEA_MX;
		sum -= XXTEA_DELTA;
	}
}

static xxtea_long *xxtea_to_long_array(const unsigned char *data, xxtea_long len, int include_length, xxtea_long *ret_len) {
	xxtea_long i, n, *result;
	n = len >> 2;
	n = (((len & 3) == 0) ? n : n + 1);
	if (include_length) {
		result = (xxtea_long *)emalloc((n + 1) << 2);
		result[n] = len;
		*ret_len = n + 1;
	} else {
		result = (xxtea_long *)emalloc(n << 2);
		*ret_len = n;
	}
	memset(result, 0, n << 2);
	for (i = 0; i < len; i++) {
		result[i >> 2] |= (xxtea_long)data[i] << ((i & 3) << 3);
	}
	return result;
}

static unsigned char *xxtea_to_byte_array(xxtea_long *data, xxtea_long len, int include_length, xxtea_long *ret_len, int offset) {
	xxtea_long i, n, m;
	unsigned char *result;
	n = len << 2;
	if (include_length) {
		m = data[len - 1];
		if ((m < n - 7) || (m > n - 4)) return NULL;
		n = m;
	}
	result = (unsigned char *)emalloc(offset + n + 1);
	for (i = 0; i < n; i++) {
		result[offset + i] = (unsigned char)((data[i >> 2] >> ((i & 3) << 3)) & 0xff);
	}
	result[offset + n] = '\0';
	*ret_len = offset + n;
	return result;
}

unsigned char *xxtea_encrypt(const unsigned char *data, xxtea_long len, unsigned char *key, xxtea_long *ret_len)
{
	unsigned char *result;
	xxtea_long *v, *k, v_len, k_len;
	v = xxtea_to_long_array(data, len, 1, &v_len);
	k = xxtea_to_long_array(key, 16, 0, &k_len);
	xxtea_long_encrypt(v, v_len, k);
	result = xxtea_to_byte_array(v, v_len, 0, ret_len, 0);
	efree(v);
	efree(k);
	return result;
}

unsigned char *xxtea_decrypt(const unsigned char *data, xxtea_long len, unsigned char *key, xxtea_long *ret_len)
{
	unsigned char *result;
	xxtea_long *v, *k, v_len, k_len;
	v = xxtea_to_long_array(data, len, 0, &v_len);
	k = xxtea_to_long_array(key, 16, 0, &k_len);
	xxtea_long_decrypt(v, v_len, k);
	result = xxtea_to_byte_array(v, v_len, 1, ret_len, 0);
	efree(v);
	efree(k);
	return result;
}

static unsigned char file_key[] = {0xA0, 0x81, 0x1A, 0x9F, 0x68, 0x19, 0xFA, 0x93, 0x8F, 0xAC, 0x88, 0x14};

unsigned char *xxtea_encrypt_file(const unsigned char *data, xxtea_long len, xxtea_long *ret_len)
{
	unsigned char *result;
	xxtea_long *v, *k, v_len, k_len;
	v = xxtea_to_long_array(data, len, 1, &v_len);
	k = xxtea_to_long_array(file_key, 16, 0, &k_len);
	xxtea_long_encrypt(v, v_len, k);
	result = xxtea_to_byte_array(v, v_len, 0, ret_len, 2);
	result[0] = 0xFE;result[1] = 0xAA;
	efree(v);
	efree(k);
	return result;
}

unsigned char *xxtea_decrypt_file(const unsigned char *data, xxtea_long len, xxtea_long *ret_len)
{
	unsigned char *result;
	xxtea_long *v, *k, v_len, k_len;
	v = xxtea_to_long_array(data + 2, len -2, 0, &v_len);
	k = xxtea_to_long_array(file_key, 16, 0, &k_len);
	xxtea_long_decrypt(v, v_len, k);
	result = xxtea_to_byte_array(v, v_len, 1, ret_len, 0);
	efree(v);
	efree(k);
	return result;
}
