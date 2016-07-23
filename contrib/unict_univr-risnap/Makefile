RISNAP=risnap
RISNAPDS=risnapds

all:
	$(MAKE) -C RiToSnap
	$(MAKE) -C RI-DSToSnap
	mv RiToSnap/$(RISNAP) .
	mv RI-DSToSnap/$(RISNAPDS) .
clean:
	
	$(MAKE) clean -C RiToSnap
	$(MAKE) clean -C RI-DSToSnap
	rm $(RISNAP) $(RISNAPDS)
