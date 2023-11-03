
#include <stdio.h>
#include <stdlib.h>

#include <proto/exec.h>
#include <proto/dos.h>

#define CATCOMP_NUMBERS

#include "locale/NallePUH.h"
#include "gui.h"

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
		{"libs:dogshit.library","os4depot.net",NULL, "demos and players" },
		{NULL,NULL,NULL,NULL}
	};

void msg(const struct file *f)
{
	printf("\n%s: %s, %s\n",_L(str_warning),f-> name,_L(str_not_installed));
	printf("  %s: %s\n", _L(str_download_from), f -> from);
	if (f -> blame) printf("  %s: %s, %s\n",_L(str_following_porgrams_will_crash),f -> blame,_L(str_if_its_not_installed));
	if (f -> used) printf("  %s: %s\n",_L(str_following_porgrams_uses_this_library), f -> used);
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

