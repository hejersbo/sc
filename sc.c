#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#define NONE 	-1

#define NUM	256
#define LSHIFT	257
#define RSHIFT	258
#define DONE	259

int v;
int t;
char *p;

/* Read a string from stdin, and return pointer to it */
char *rl_gets(void)
{
	static char *line_read = (char *)NULL;

	if (line_read) {
		free(line_read);
		line_read = (char *)NULL;
	}
	line_read = readline("> ");
	if (line_read && *line_read)
		add_history(line_read);
	return line_read;
}

int expression(void);

void printbin(size_t const size, void const * const ptr)
{
	unsigned char *b = (unsigned char*) ptr;
	unsigned char byte, bits_in_prefix;
	int i, j;

	bits_in_prefix = 0;
	for (i=size-1;i>=0;i--) {
		if (b[i] != 0 || bits_in_prefix || i == 0) {
			bits_in_prefix = 1;
			for (j=7;j>=0;j--) {
				byte = b[i] & (1<<j);
				byte >>= j;
				printf("%u", byte);
			}
			printf(" ");
		}
	}
	puts("");
}

void error(char *m)
{
	fprintf(stderr, "%s\n", m);
	exit(1);
}

int btoi(char *b)
{
	int n;

	n = 0;
	while (*b != '\0') {
		if (*b != '0' && *b != '1') {
			error("wrong binary format");
		}
		n = n*2 + (*b - '0');
		b++;
	}

	return n;
}

int power(int base, int power)
{
	int result, i;

	result = 1;
	for (i=0; i < power; i++)
	{
		result *= base;
	}
	return result;
}

void next(void)
{
	while (*p == ' ' || *p == '\t')
		p++;

	if (isdigit(*p)) {
		int n;
		if (*p == '0' && *(p + 1) == 'x') {
			sscanf((p + 2), "%x%n", &v, &n);
			n+=2;
		}
		else if (*p == '0' && *(p + 1) == 'b') {
			char bin[32];
			sscanf((p + 2), "%s%n", &bin[0], &n);
			v = btoi(bin);
			n+=2;
		}
		else {
			sscanf(p, "%d%n", &v, &n);
		}
		t = NUM;
		p += n;
	}
	else if (*p == '<' && *(p + 1) == '<') {
		t = LSHIFT;
		p += 2;
	}
	else if (*p == '>' && *(p + 1) == '>') {
		t = RSHIFT;
		p += 2;
	}
	else {
		t = *p;
		p++;
	}
	printf("next(): t = %d, v = %d, p = %s\n", t, v, p);
}

int number(void) {

	if (t == NUM)
		return v;
	else {
		error("number expected");
		return 0;
	}
}

int factor(void) {
	int n;

	if (t == '(') {
		next();
		n = expression();
		if (t != ')')
			error(") expected");
		next();
	} else {
		n = number();
		next();
	}
	return n;
}

int term(void)
{
	int op, n, m;

	n = factor();
	while ((op = t) == '*' || op == '/' ||
			op == LSHIFT || op == RSHIFT ||
			op == '&' || op == '|' ||
			op == '^') {
		next();
		m = factor();
		switch (op) {
			case '*':
				n = n * m;
				continue;
			case '/':
				n = n / m;
				continue;
			case LSHIFT:
				n = n << m;
				continue;
			case RSHIFT:
				n = n >> m;
				continue;
			case '&':
				n = n & m;
				continue;
			case '|':
				n = n | m;
				continue;
			case '^':
				n = power(n, m);
				continue;
		}
	}
	return n;
}

int expression(void)
{
	int sign, op, n, m;

	if ((sign = (t == '-')) || t == '+')
		next();
	n = term();
	if (sign)
		n = -n;
	while ((op = t) == '+' || op == '-') {
		next();
		m = term();
		n = (op == '+') ? n + m : n - m;
	}
	return n;
}

int main(void) {
	int e;

	while (1) {
		p = rl_gets();
		next();
		e = expression();
		printf("%d    0x%04x    ", e, e);
		printbin(sizeof(int), &e);
		printf("\n");
	}
	return 0;
}

