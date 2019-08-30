#
# Makefile for non-Microsoft compilers
#

all: MakeAll

MakeAll:
	mkdir Release
	$(MAKE) -C agmfit
	$(MAKE) -C agmgen
	$(MAKE) -C bigclam
	$(MAKE) -C cascadegen
	$(MAKE) -C cascades
	$(MAKE) -C centrality
	$(MAKE) -C cesna
	$(MAKE) -C circles
	$(MAKE) -C cliques
	$(MAKE) -C coda
	$(MAKE) -C community
	$(MAKE) -C concomp
	$(MAKE) -C flows
	$(MAKE) -C forestfire
	$(MAKE) -C graphgen
	$(MAKE) -C graphhash
	$(MAKE) -C infopath
	$(MAKE) -C kcores
	$(MAKE) -C knnjaccardsim
	$(MAKE) -C kronem
	$(MAKE) -C kronfit
	$(MAKE) -C krongen
	$(MAKE) -C localmotifcluster
	$(MAKE) -C lshtest
	$(MAKE) -C magfit
	$(MAKE) -C maggen
	$(MAKE) -C mkdatasets
	# 'motifcluster' compilation skipped by default, it requires ARPACK
	#$(MAKE) -C motifcluster
	$(MAKE) -C motifs
	$(MAKE) -C ncpplot
	$(MAKE) -C netevol
	$(MAKE) -C netinf
	$(MAKE) -C netstat
	$(MAKE) -C node2vec
	$(MAKE) -C randwalk
	$(MAKE) -C rolx
	$(MAKE) -C temporalmotifs
	$(MAKE) -C testgraph

clean:
	$(MAKE) clean -C agmgen
	$(MAKE) clean -C agmfit
	$(MAKE) clean -C bigclam
	$(MAKE) clean -C cascadegen
	$(MAKE) clean -C cascades
	$(MAKE) clean -C centrality
	$(MAKE) clean -C cesna
	$(MAKE) clean -C circles
	$(MAKE) clean -C cliques
	$(MAKE) clean -C coda
	$(MAKE) clean -C community
	$(MAKE) clean -C concomp
	$(MAKE) clean -C flows
	$(MAKE) clean -C forestfire
	$(MAKE) clean -C graphgen
	$(MAKE) clean -C graphhash
	$(MAKE) clean -C infopath
	$(MAKE) clean -C kcores
	$(MAKE) clean -C knnjaccardsim
	$(MAKE) clean -C kronem
	$(MAKE) clean -C kronfit
	$(MAKE) clean -C krongen
	$(MAKE) clean -C localmotifcluster
	$(MAKE) clean -C lshtest
	$(MAKE) clean -C magfit
	$(MAKE) clean -C maggen
	$(MAKE) clean -C mkdatasets
	$(MAKE) clean -C motifcluster
	$(MAKE) clean -C motifs
	$(MAKE) clean -C ncpplot
	$(MAKE) clean -C netevol
	$(MAKE) clean -C netinf
	$(MAKE) clean -C netstat
	$(MAKE) clean -C node2vec
	$(MAKE) clean -C randwalk
	$(MAKE) clean -C rolx
	$(MAKE) clean -C temporalmotifs
	$(MAKE) clean -C testgraph
	rm -rf Debug Release ipch *.tgz *.zip

bin-linux:
	mv Release Snap-$(VERSION)-Ubuntu18.04
	tar zcvf Snap-$(VERSION)-Ubuntu18.04.tgz Snap-$(VERSION)-Ubuntu18.04
	mv Snap-$(VERSION)-Ubuntu18.04 Release

bin-mac:
	mv Release Snap-$(VERSION)-macOS14.10
	tar zcvf Snap-$(VERSION)-macOS14.10.tgz Snap-$(VERSION)-macOS14.10
	mv Snap-$(VERSION)-macOS14.10 Release

bin-win:
	mv Release Snap-$(VERSION)-Win32
	zip -r Snap-$(VERSION)-Win32.zip Snap-$(VERSION)-Win32/*.exe
	mv Snap-$(VERSION)-Win32 Release

