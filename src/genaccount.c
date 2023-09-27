#ifndef WIN32
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>


// SPDX-License-Identifier: GPL-2.0 OR MIT
/*
 * Copyright (C) 2015-2020 Jason A. Donenfeld <Jason@zx2c4.com>. All Rights Reserved.
 * Copyright (C) 2023 Vicente Aceituno Canal <vicente@cableguard.org>. All Rights Reserved.
 */

// longest path for the output file
#define PATH_MAX 4096
#ifdef __linux__
#include <sys/syscall.h>
#endif
#ifdef __APPLE__
#include <AvailabilityMacros.h>
#ifndef MAC_OS_X_VERSION_10_12
#define MAC_OS_X_VERSION_10_12 101200
#endif
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_12
#include <sys/random.h>
#endif
#endif

#include "curve25519.h"
#include "encoding.h"
#include "subcommands.h"
#include "ed25519.h"

#ifndef _WIN32
static inline bool __attribute__((__warn_unused_result__)) get_random_bytes(uint8_t *out, size_t len)
{
	ssize_t ret = 0;
	size_t i;
	int fd;

	if (len > 256) {
		errno = EOVERFLOW;
		return false;
	}

#if defined(__OpenBSD__) || (defined(__APPLE__) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_12) || (defined(__GLIBC__) && (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25)))
	if (!getentropy(out, len))
		return true;
#endif

#if defined(__NR_getrandom) && defined(__linux__)
	if (syscall(__NR_getrandom, out, len, 0) == (ssize_t)len)
		return true;
#endif

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return false;
	for (errno = 0, i = 0; i < len; i += ret, ret = 0) {
		ret = read(fd, out + i, len - i);
		if (ret <= 0) {
			ret = errno ? -errno : -EIO;
			break;
		}
	}
	close(fd);
	errno = -ret;
	return i == len;
}
#else
#include <ntsecapi.h>
static inline bool __attribute__((__warn_unused_result__)) get_random_bytes(uint8_t *out, size_t len)
{
        return RtlGenRandom(out, len);
}
#endif

/*
 * Copyright 2012-2014 Luke Dashjr
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the standard MIT license.  See COPYING for more details.
 */
#ifndef LIBBASE58_H
#define LIBBASE58_H
#endif

static const char b58digits_ordered[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

bool b58enc(char *b58, size_t *b58sz, const void *data, size_t binsz)
{
	const uint8_t *bin = data;
	int carry;
	size_t i, j, high, zcount = 0;
	size_t size;

	for (zcount = 0; zcount < binsz; ++zcount) {
		if (bin[zcount] == '\0') {
			break;
		}
	}
	/*	while (zcount < binsz && !bin[zcount]) ++zcount; */
	
	size = (binsz - zcount) * 138 / 100 + 1;
	uint8_t buf[size];
	memset(buf, 0, size);
	
	for (i = zcount, high = size - 1; i < binsz; ++i, high = j)
	{
		for (carry = bin[i], j = size - 1; (j > high) || carry; --j)
		{
			carry += 256 * buf[j];
			buf[j] = carry % 58;
			carry /= 58;
			if (!j) {
				// Otherwise j wraps to maxint which is > high
				break;
			}
		}
	}
	
	for (j = 0; j < size && !buf[j]; ++j);
	
	if (*b58sz <= zcount + size - j)
	{
		*b58sz = zcount + size - j + 1;
		return false;
	}
	
	if (zcount)
		memset(b58, '1', zcount);
	for (i = zcount; j < size; ++i, ++j)
		b58[i] = b58digits_ordered[buf[j]];
	b58[i] = '\0';
	*b58sz = i + 1;
	
	return true;
}

// SPDX-License-Identifier: GPL-2.0 OR MIT
/*
 * Copyright (C) 2023 Vicente Aceituno Canal <vicente@cableguard.org>. All Rights Reserved.
 */

int genaccount_main(int argc, const char *argv[])
{
	uint8_t  seed_hex[WG_KEY_LEN];
	uint8_t  extended_hex[WG_KEY_LEN*2];
	unsigned char private_hexkey[WG_KEY_LEN*2];
	unsigned char public_hexkey[WG_KEY_LEN];
	char public_base58key[WG_KEY_LEN*2];
	char extended_base58[WG_KEY_LEN*4];
	size_t publickeylenbase58ptr = WG_KEY_LEN*2;
	size_t privatekeylenbase58ptr = WG_KEY_LEN*4;
	char public_hexkey_asstring[WG_KEY_LEN * 2 + 1]; // +1 for null terminator
	char blockchain_env[PATH_MAX];
	char file_path[PATH_MAX];
	char home_dir[PATH_MAX];
	struct stat stat;

	if (argc != 1) {
		fprintf(stderr, "Usage: %s %s\n", PROG_NAME, argv[0]);
		return 1;
	}

	if (!fstat(STDOUT_FILENO, &stat) && S_ISREG(stat.st_mode) && stat.st_mode & S_IRWXO)
		fputs("Warning: writing to world accessible file.\nConsider setting the umask to 077 and trying again.\n", stderr);

	if (0!=ed25519_create_seed(seed_hex)) {
		perror("getrandom");
		return 1;
	}

	if (!strcmp(argv[0], "genaccount"))
		ed25519_create_keypair(public_hexkey,private_hexkey,(const unsigned char *)seed_hex);
	
	memcpy(extended_hex, seed_hex, WG_KEY_LEN);
    memcpy(extended_hex + WG_KEY_LEN , public_hexkey, WG_KEY_LEN);
	b58enc(extended_base58, &privatekeylenbase58ptr, extended_hex, WG_KEY_LEN*2);
	b58enc(public_base58key, &publickeylenbase58ptr, public_hexkey, WG_KEY_LEN);

	// This works for Linux only for the time being
	char *home_env = getenv("HOME");
	if (home_env == NULL) {
		fprintf(stderr, "Error: Unable to determine home directory.\n");
		return 1;
	} else {
		strncpy(home_dir, home_env, sizeof(home_dir) - 1);
		home_dir[sizeof(home_dir) - 1] = '\0'; // Ensure null-termination
	}

	// This works for Linux only for the time being
	char *bc_env = getenv("BLOCKCHAIN_ENV");
	if (bc_env == NULL) {
		fprintf(stderr, "Error: Unable to determine BLOCKCHAIN_ENV: mainnet/testnet?.\n");
		return 1;
	} else {
		strncpy(home_dir, bc_env, sizeof(home_dir) - 1);
		home_dir[sizeof(home_dir) - 1] = '\0'; // Ensure null-termination
	}

	for (int i = 0; i < WG_KEY_LEN; i++) {
    	sprintf(&public_hexkey_asstring[i * 2], "%02x", public_hexkey[i]);
	}
	public_hexkey_asstring[WG_KEY_LEN * 2] = '\0';
	snprintf(file_path, sizeof(file_path), "%s/.near-credentials/%s/%s.json", home_dir, blockchain_env, public_hexkey_asstring);

	FILE *file = fopen(file_path, "w");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    fprintf(file,"{\"account_id\":\"");
    for (int i = 0; i < WG_KEY_LEN; i++) {
        fprintf(file, "%02x", public_hexkey[i]);
        }
    fprintf(file,"\",\"public_key\":\"ed25519:%s\",\"private_key\":\"ed25519:%s\"}\n",public_base58key,extended_base58);

    fclose(file);

return 0;
}