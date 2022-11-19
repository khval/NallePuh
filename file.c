
#include <stdio.h>
#include <stdlib.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/ahi.h>

#include "gui.h"

void load(const char *name, struct rc *rc)
{
	FILE *fd;
	fd = fopen( name, "r" );
	if (fd)
	{
		fscanf(fd,"audio_mode: %08lx\n", &rc -> audio_mode );
		fscanf(fd,"%*s %d", &rc -> frequency );
		fscanf(fd,"%*s %255[^\n]", rc -> AHI_name );
		fclose(fd);
	}
}

void save(const char *name, struct rc *rc)
{
	FILE *fd;
	fd = fopen( name, "w" );
	if (fd)
	{
		fprintf(fd,"audio_mode: %08lx\nfrequency: %d\nAHI_name: %s\n", 
			rc -> audio_mode, 
			(int) rc -> frequency,
			rc -> AHI_name);

		fclose(fd);
	}
}

