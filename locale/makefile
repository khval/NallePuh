
all: NallePUH.ct updated

NallePUH.ct: NallePUH.cd
	CATCOMP NallePUH.cd CTFILE NallePUH.ct

updated: NallePUH.ct
	protect update +s
	update
	date > updated
