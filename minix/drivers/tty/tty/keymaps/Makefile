# Generate binary keymaps.

.include <bsd.own.mk>

FILES=	scandinavian.map

FILESDIR= /usr/lib/keymaps

.SUFFIXES:      .src .map

HOST_CPPFLAGS+= -I${NETBSDSRCDIR}/minix

.src.map:
	${_MKTARGET_CREATE}
	${HOST_CC} ${HOST_CFLAGS} ${HOST_CPPFLAGS} ${HOST_LDFLAGS} -DKEYSRC=\"$<\" -o ${.OBJDIR}/${.TARGET}_genmap ${.CURDIR}/genmap.c
	${.OBJDIR}/${.TARGET}_genmap > ${.OBJDIR}/${.TARGET}
	rm -f ${.OBJDIR}/${.TARGET}_genmap

realall: ${FILES}

CLEANFILES+= ${FILES}

.include <bsd.prog.mk>
