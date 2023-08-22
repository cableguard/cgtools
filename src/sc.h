// SPDX-License-Identifier: zlib license /* https://github.com/orlp/ed25519/tree/master  
// Copyright (C) 2022-2023 Orson Peters <orsonpeters@gmail.com>. All Rights Reserved

#ifndef SC_H
#define SC_H

/*
The set of scalars is \Z/l
where l = 2^252 + 27742317777372353535851937790883648493.
*/

void sc_reduce(unsigned char *s);
void sc_muladd(unsigned char *s, const unsigned char *a, const unsigned char *b, const unsigned char *c);

#endif
