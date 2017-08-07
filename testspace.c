
#include <stdio.h>
#include <string.h>

int main() {



	char buf[1024];
	strcpy(buf, "   This  is a   row   with some  space!   ");

	char *c1, *c2;

	c1=c2=buf;
	while (*c1 != 0) {
		*c1 = *c2;
		c1++;
		while(*c2 == ' ' && *(c2 +1) == ' ') c2++;
		c2++;

	}

  printf("buf=|%s|\n", buf);

}
