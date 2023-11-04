
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
	int32 used_id;
};

const struct file files[]=
	{
		{"libs:reqtools.l.main","os4depot.net",NULL,NULL,0},
		{"libs:reqtools.library","os4depot.net","HippoPlayer, OctaMedStudio",NULL,0},
		{"libs:powerpacker.library","os4depot.net",NULL, "HippoPlayer",0 },
		{"libs:libblitter.library","os4depot.net",NULL, "NallePuh",0 },
		{"libs:ptplay.library","os4depot.net",NULL, NULL, str_demos_and_players },
		{"libs:ptreplay.library","os4depot.net",NULL, NULL, str_demos_and_players },
		{"libs:dogshit.library","os4depot.net",NULL, NULL, str_demos_and_players },
		{NULL,NULL,NULL,NULL,0}
	};

void msg(const struct file *f)
{
	char *used = f -> used;
	if (!used) 	if (f -> used_id) used = _L(f -> used_id);

	printf("\n%s: %s, %s\n",_L(str_warning),f-> name,_L(str_not_installed));
	printf("  %s: %s\n", _L(str_download_from), f -> from);
	if (f -> blame) printf("  %s: %s, %s\n",_L(str_following_programs_will_crash),f -> blame,_L(str_if_its_not_installed));
	if (used) printf("  %s: %s\n",_L(str_following_programs_uses_this_library), used);
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

