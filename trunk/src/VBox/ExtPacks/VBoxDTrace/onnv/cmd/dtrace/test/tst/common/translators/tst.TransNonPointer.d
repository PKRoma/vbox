/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#pragma ident	"%Z%%M%	%I%	%E% SMI"

/*
 * ASSERTION:
 * Translate the input expression to output struct type
 *
 * SECTION: Translators/ Translator Declarations
 * SECTION: Translators/ Translate Operator
 *
 */

#pragma D option quiet

struct myinput_struct {
	int i;
	char c;
};

struct myoutput_struct {
	int myi;
	char myc;
};

translator struct myoutput_struct < struct myinput_struct ivar >
{
	myi = ((struct myinput_struct ) ivar).i;
	myc = ((struct myinput_struct ) ivar).c;

};

struct myinput_struct f;
BEGIN
{
	f.i = 10;
	f.c = 'c';

	realmyi = xlate < struct myoutput_struct > (f).myi;
	realmyc = xlate < struct myoutput_struct > (f).myc;
}

BEGIN
/(10 != f.i) || ('c' != f.c)/
{
	exit(1);
}

BEGIN
/(10 == f.i) && ('c' == f.c)/
{
	printf("realmyi: %d\n", realmyi);
	printf("realmyc: %c\n", realmyc);
	exit(0);
}

ERROR
{
	exit(1);
}
