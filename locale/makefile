
all: NallePUH.ct updated

clean:
	delete NallePUH.#?(c|h)

NallePUH.ct: NallePUH.cd
	CATCOMP NallePUH.cd CTFILE NallePUH.ct

updated: NallePUH.cd
	protect update +s
	update
	date > updated
