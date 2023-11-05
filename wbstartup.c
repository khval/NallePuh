/*
	Copyright (C) 2023: Kjetil Hvalstrand
	MIT License.
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <workbench/startup.h>

const char *str_progdir = "PROGDIR:";

bool arg_iconify = false;
bool arg_activate = false;

bool wbStartup(struct WBStartup *wbmsg)
{
	char *name;
	struct Task *me;
	char *tmp;
	struct DiskObject *dobj = NULL;

	if (wbmsg==NULL) return false;

	me = FindTask(NULL);

	name = me -> tc_Node.ln_Name;

	tmp = (char*) malloc(strlen(str_progdir)+strlen(name)+1);

	if (tmp)
	{
		sprintf(tmp,"%s%s",str_progdir,name);

		dobj = GetDiskObject(tmp);
		if (dobj)
		{
			if (FindToolType( dobj -> do_ToolTypes, "iconify")) arg_iconify = true;
			if (FindToolType( dobj -> do_ToolTypes, "activate" )) arg_activate = true;
		}

		FreeDiskObject(dobj);

		free(tmp);
	}

	return true;
}

