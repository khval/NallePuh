
#include <stdio.h>
#include <stdlib.h>

#include <proto/exec.h>
#include <proto/dos.h>

struct file
{
	char *name;
	char *from;
	char *blame;
	char *used;
};

const struct file files[]=
	{
		{"libs:reqtools.l.main","os4depot.net",NULL,NULL},
		{"libs:reqtools.library","os4depot.net","HippoPlayer, OctaMedStudio",NULL},
		{"libs:powerpacker.library","os4depot.net",NULL, "HippoPlayer" },
		{"libs:libblitter.library","os4depot.net",NULL, "NallePuh" },
		{"libs:ptplay.library","os4depot.net",NULL, "demos and players" },
		{"libs:ptreplay.library","os4depot.net",NULL, "demos and players" },
		{NULL,NULL,NULL,NULL}
	};

void msg(const struct file *f)
{
	printf("\nWarning: %s, not installed\n", f-> name);
	printf("  download from: %s\n", f -> from);
	if (f -> blame) printf("  follwing porgrams will crash: %s, if its not installed\n", f -> blame);
	if (f -> used) printf("  follwing porgrams uses this library: %s\n", f -> used);
	printf("\n");
}

void warn()
{
	BPTR l;
	const struct file *f;

	for (f = files; f -> name; f++)
	{
		if ((l = Lock(f -> name, ACCESS_READ)))
		{
			UnLock(l);
		}
		else msg(f);
	}
}

