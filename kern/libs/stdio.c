#include <defs.h>
#include <stdio.h>
#include <console.h>
#include <unistd.h>
/*HIGH level console I/O */

/* *
 * cputch - writes a single character @c stdout, and it will
 * instance the value of counter pointed by @cnt.
 */
static void
cputch(int c, int *cnt) {
	cons_putc(c);
	(*cnt) ++;
}

/* *
 * vcprintf - format a string and writes it to stdout
 *
 * The return value is the number of characters which would be
 * written to stdout
 *
 * Call this function if you are already dealing with va_list.
 * Or you probably want cprintf() instead.
 * */
int
vcprintf(const char *fmt, va_list ap) {
	int cnt = 0;
    vprintfmt((void*)cputch, NO_FD, &cnt, fmt, ap);
    return cnt;
}

/* *
 * cprintf - formats a string and writes it to stdout
 *
 * The return value is the number of characters which would be
 * written to stdout.
 */
int
cprintf(const char *fmt, ...) {
	va_list ap;
	int cnt;
	va_start(ap, fmt);
	cnt = vcprintf(fmt, ap);
	va_end(ap);
	return cnt;
}

/* cputchar - writes a single character to stdout */
void
cputchar(int c) {
	cons_putc(c);
}

/* *
 * cputs- writes the string pointed by @str to stdout and
 * appends a newline character.
 */
int
cputs(const char *str) {
	int cnt = 0;
	char c;
	while((c = *str ++) != '\0') {
		cputch(c, &cnt);
	}
	cputch('\n', &cnt);
	return cnt;
}

/* getchar - reads a single non-zero character from stdin */
int
getchar(void) {
	int c;
	while((c = cons_getc()) == 0)
		/* do nothing */;
	return c;
}

void put_byte(unsigned char num) {
	cputchar("0123456789ABCDEF"[(num>>4)&0xF]);
	cputchar("0123456789ABCDEF"[num&0xF]);
}

void puthex(unsigned int num){
    put_byte((num>>24)&0xff);
    put_byte((num>>16)&0xff);
    put_byte((num>>8)&0xff);
    put_byte((num)&0xff);
}