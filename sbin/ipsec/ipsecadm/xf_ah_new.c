/* $OpenBSD: xf_ah_new.c,v 1.2 1997/09/23 21:41:00 angelos Exp $ */
/*
 * The author of this code is John Ioannidis, ji@tla.org,
 * 	(except when noted otherwise).
 *
 * This code was written for BSD/OS in Athens, Greece, in November 1995.
 *
 * Ported to OpenBSD and NetBSD, with additional transforms, in December 1996,
 * by Angelos D. Keromytis, kermit@forthnet.gr. Additional code written by
 * Niels Provos in Germany.
 * 
 * Copyright (C) 1995, 1996, 1997 by John Ioannidis, Angelos D. Keromytis and
 * Niels Provos.
 *	
 * Permission to use, copy, and modify this software without fee
 * is hereby granted, provided that this entire notice is included in
 * all copies of any software which is or includes a copy or
 * modification of this software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTY. IN PARTICULAR, NEITHER AUTHOR MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE
 * MERCHANTABILITY OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR
 * PURPOSE.
 */

#include <sys/param.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/mbuf.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/route.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <netns/ns.h>
#include <netiso/iso.h>
#include <netccitt/x25.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <paths.h>
#include "net/encap.h"
#include "netinet/ip_ipsp.h"
#include "netinet/ip_ah.h"

extern char buf[]; 

int xf_set __P(( struct encap_msghdr *));
int x2i __P((char *));

int
xf_ah_new(src, dst, spi, auth, keyp, osrc, odst)
struct in_addr src, dst;
u_int32_t spi;
int auth;
u_char *keyp;
struct in_addr osrc, odst;
{
	int klen, i;

	struct encap_msghdr *em;
	struct ah_new_xencap *xd;

	klen = strlen(keyp)/2;

	em = (struct encap_msghdr *)&buf[0];
	
	em->em_msglen = EMT_SETSPI_FLEN + AH_NEW_XENCAP_LEN + klen;
	em->em_version = PFENCAP_VERSION_1;
	em->em_type = EMT_SETSPI;
	em->em_spi = spi;
	em->em_src = src;
	em->em_dst = dst;
	em->em_osrc = osrc;
	em->em_odst = odst;
	em->em_alg = XF_NEW_AH;
	em->em_sproto = IPPROTO_AH;

	xd = (struct ah_new_xencap *)(em->em_dat);

	xd->amx_hash_algorithm = auth;
	xd->amx_wnd = -1;	/* Manual setup -- no sequence number */
	xd->amx_keylen = klen;

	bzero(xd->amx_key, klen);
	for (i = 0; i < klen; i++ )
	  xd->amx_key[i] = x2i(keyp+2*i);

	return xf_set(em);
}
