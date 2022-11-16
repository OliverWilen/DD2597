#	$NetBSD: parselist.awk,v 1.16 2009/04/10 16:16:12 apb Exp $
#
# Copyright (c) 2002 The NetBSD Foundation, Inc.
# All rights reserved.
#
# This code is derived from software contributed to The NetBSD Foundation
# by Luke Mewburn of Wasabi Systems.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

#
# awk -f parselist.awk -v mode=MODE [var=val ...] file1 [...]
#
#	Parse list files file1 [...], generating different output,
#	depending upon the value of MODE:
#
#	    crunch	crunchgen(1) config
#
#	    install	make(1) Makefile to install commands into ${TARGETDIR},
#			with an `install' target.
#			The following environment variables need to be set:
#			    TARGETDIR	Directory to populate
#
#	    mtree	mtree(8) specfile
#
#	    populate	sh(1) commands to populate ${TARGETDIR} from ${CURDIR}
#			The following environment variables need to be set:
#			    CURDIR	Source directory; make(1)'s ${.CURDIR}
#			    TARGETDIR	Directory to populate
#
#	The following environment variables need to be set for all modes:
#	    CRUNCHBIN	Name of crunchgen(1) target binary
#	    OBJDIR	Object directory; make(1)'s ${.OBJDIR}
#
# 	Each line of the input is either a comment (starts with `#'),
#	or contains one of the following keywords and arguments.
#
#	Before each line is parsed for a keyword, words surrounded by
#	"${" and "}", and containing only letters, numbers, and `_'
#	will be replaced by the value of the environment variable of
#	the same name.  I.e., "${MACHINE_ARCH}" will be replaced with the
#	value of ENVIRON["MACHINE_ARCH"].
#
#	mode key	operation
#	--------	---------
#	C		crunch
#	I		install
#	M		mtree
#	P		populate
#
#	mode	keyword arg1 [...]	description
#	----	------------------	-----------
#
#	C	ARGVLN	prog link	as per crunchgen(1) `ln'
#
#	P	CMD	arg1 [...]	run CMD as a shell command
#
#	IMP	COPY	src dest [perm]	copy src to dest. perm defaults to 0444
#
#	IMP	COPYDIR	src dest	recursively copy files under src to
#					dest.  for M, dest is listed first,
#					followed by the subdirectories in src.
#					copied directories have mode 0755.
#					copied files have mode 0444.
#
#	C	LIBS	libspec ...	as per crunchgen(1) `libs'
#
#	IMP	LINK	src d1 [d2 ...]	hard link src to d1, d2, ...
#
#	M	MTREE	arg1 [...]	output arguments `as-is' to specfile
#
#	CIMP	PROG	prog [links...]	program(s) to crunch/mtree/populate.
#					for I, M & P, the first prog listed
#					is copied from ${OBJDIR}/${CRUNCHBIN}
#					and then used as the name to link
#					all other PROG entries to.
#
#	C	SPECIAL	prog cmd ...	as per crunchgen(1) `special'
#
#	C	SRCDIRS	dirname ...	as per crunchgen(1) `srcdirs'
#
#	IMP	SYMLINK src dest [...]	symlink src to dest, [...]
#

BEGIN \
{
	crunchprog = "";

	if (mode != "crunch" && mode != "install" &&
	    mode != "mtree" && mode != "populate")
		errx("Unknown parselist mode '" mode "'");

	needvars["CRUNCHBIN"]++
	needvars["OBJDIR"]++
	if (mode == "install") {
		needvars["TARGETDIR"]++
	}
	else if (mode == "populate") {
		needvars["CURDIR"]++
	}
	for (nv in needvars) {
		if (! (nv in ENVIRON))
			errx("Environment variable " nv " not defined");
	}

	print "#";
	print "# This file is automatically generated by";
	print "#\tparselist mode=" mode;
	print "#";
	print "";
	if (mode == "install") {
		print ".include <bsd.own.mk>"
		print "install:"
	} else if (mode == "mtree") {
		print "/unset\tall";
		print "/set\ttype=file uname=root gname=wheel";
		print;
	} else if (mode == "populate") {
		print "cd " ENVIRON["CURDIR"];
		print;
	}
}

/^$/ || /^#/ \
{
	print;
	next;
}

#	replace ${FOO} with ENVIRON["FOO"]
#
/\$\{[A-Za-z0-9_]+\}/ \
{
	while (match($0, /\$\{[A-Za-z0-9_]+\}/) > 0) {
		v = substr($0, RSTART + 2, RLENGTH - 3);
		if (! (v in ENVIRON))
			err("Variable " v " is not in the environment");
		else
			sub(/\$\{[A-Za-z0-9_]+\}/, ENVIRON[v]);
	}
}

$1 == "COPY" \
{
	if (NF < 3 || NF > 4)
		err("Usage: COPY src dest [perm]");
	if (mode == "install" || mode == "mtree" || mode == "populate")
		copy($2, $3, $4);
	next;
}

$1 == "COPYDIR" \
{
	if (NF != 3)
		err("Usage: COPYDIR src dest");
	srcdir=$2;
	destdir=$3;
	if (mode == "mtree") {
		printf("./%s type=dir mode=755\n", destdir);
		command="cd " srcdir " && find . -type d -print"
		while (command | getline dir) {
			gsub(/^\.\//, "", dir);
			if (dir == ".")
				continue;
			printf("./%s/%s type=dir mode=755\n", destdir, dir);
		}
		close(command);
	}
	if (mode == "install" || mode == "mtree" || mode == "populate") {
		command="cd " srcdir " && find . -type f -print"
		while (command | getline srcfile) {
			gsub(/^\.\//, "", srcfile);
			copy(srcdir "/" srcfile, destdir "/" srcfile, "");
		}
		close(command);
	}
	next;
}

$1 == "LIBS" || $1 == "SPECIAL" || $1 == "SRCDIRS" \
{
	if (NF < 2)
		err("Usage: " $1 " args...");
	if (mode == "crunch") {
		$1 = tolower($1);
		print;
	}
	next;
}

$1 == "PROG" \
{
	if (NF < 2)
		err("Usage: PROG prog [link ...]");
	if (mode == "crunch") {
		prog = basename($2);
		print "progs " prog;
		for (i = 3; i <= NF; i++)
			print "ln " prog " " basename($i);
	} else {
		for (i = 2; i <= NF; i++) {
			if (crunchprog == "") {
				crunchprog = $i;
				copy(ENVIRON["OBJDIR"] "/" ENVIRON["CRUNCHBIN"],
				    crunchprog, 555);
				continue;
			}
			link(crunchprog, $i, 555);
		}
	}
	next;
}

$1 == "ARGVLN" \
{
	if (NF != 3)
		err("Usage: ARGVLN prog link");
	if (mode == "crunch") {
		$1 = "ln";
		print;
	}
	next;
}

$1 == "LINK" \
{
	if (NF < 3)
		err("Usage: LINK prog link [...]");
	if (mode == "install" || mode == "mtree" || mode == "populate") {
		for (i = 3; i <= NF; i++)
			link($2, $i, 444);
	}
	next;
}

$1 == "SYMLINK" \
{
	if (NF < 3)
		err("Usage: SYMLINK prog link [...]");
	if (mode == "install" || mode == "mtree" || mode == "populate") {
		for (i = 3; i <= NF; i++)
			symlink($2, $i);
	}
	next;
}

$1 == "CMD" \
{
	if (NF < 2)
		err("Usage: CMD ...");
	if (mode == "populate") {
		printf("(cd %s;", ENVIRON["TARGETDIR"]);
		for (i = 2; i <= NF; i++)
			printf(" %s", $i);
		print ") || exit 1";
	}
	next;
}

$1 == "MTREE" \
{
	if (NF < 2)
		err("Usage: MTREE ...");
	if (mode == "mtree") {
		sub(/^[^ \t]+[ \t]+/, "");	# strip first word ("MTREE")
		print;
	}
	next;
}


{
	err("Unknown keyword '" $1 "'");
}


function basename (file) \
{
	gsub(/[^\/]+\//, "", file);
	return file;
}

function copy (src, dest, perm) \
{
	if (perm == "")
		perm = 444;
	if (mode == "install") {
		printf("\t${INSTALL_FILE} -o ${BINOWN} -g ${BINGRP}" \
		    " -m %s %s %s/%s\n",
		    perm, src, ENVIRON["TARGETDIR"], dest)
	} else if (mode == "mtree") {
		printf("./%s mode=%s\n", dest, perm);
	} else {
		printf("rm -rf %s/%s\n", ENVIRON["TARGETDIR"], dest);
		printf("cp %s %s/%s\n", src, ENVIRON["TARGETDIR"], dest);
		printf("chmod %s %s/%s\n", perm, ENVIRON["TARGETDIR"], dest);
	}
}

function link (src, dest, perm) \
{
	if (mode == "install") {
		printf("\t${INSTALL_LINK} -o ${BINOWN} -g ${BINGRP}" \
		    " -m %s %s/%s %s/%s\n",
		    perm, ENVIRON["TARGETDIR"], src, ENVIRON["TARGETDIR"], dest)
	} else if (mode == "mtree") {
		printf("./%s mode=%s\n", dest, perm);
	} else {
		printf("rm -rf %s/%s\n", ENVIRON["TARGETDIR"], dest);
		printf("(cd %s; ln %s %s) || exit 1\n",
		    ENVIRON["TARGETDIR"], src, dest);
	}
}

function symlink (src, dest) \
{
	if (mode == "install") {
		printf("\t${INSTALL_SYMLINK} %s/%s %s/%s\n",
		    ENVIRON["TARGETDIR"], src, ENVIRON["TARGETDIR"], dest)
	} else if (mode == "mtree") {
		printf("./%s type=link link=%s\n", dest, src);
	} else {
		printf("rm -rf %s/%s\n", ENVIRON["TARGETDIR"], dest);
		printf("(cd %s; ln -s %s %s) || exit 1\n",
		    ENVIRON["TARGETDIR"], src, dest);
	}
}

function err(msg) \
{
	printf("parselist: %s at line %d of input.\n", msg, NR) >"/dev/stderr";
	exit 1;
}

function errx(msg) \
{
	printf("parselist: %s.\n", msg) >"/dev/stderr";
	exit 1;
}
