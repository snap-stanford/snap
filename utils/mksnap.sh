#!/bin/bash

#
#	create a SNAP release package
#

VERSION=1.9.1
GITDIR=$HOME/git/snap
WORKDIR=$HOME/snaprelease

DATE=`date +"%Y%m%d-%H%M%S"`

VERNAME=Snap-${VERSION}
FULLNAME=${VERNAME}-${DATE}

echo `date` "Preparing SNAP Release" $FULLNAME "..."

# copy from git to the working release directory
cp -a $GITDIR $WORKDIR/${FULLNAME}

# prepare the directory
cd $WORKDIR
ln -fsn ${FULLNAME} ${VERNAME}

# set the version number
echo ${FULLNAME} > $WORKDIR/${FULLNAME}/Version

# delete redundant directories
cd $WORKDIR/${FULLNAME}
rm -rf .git doc glib-adv projects utils

# prepare a distribution package and a backup package
cd $WORKDIR
rm -f ${VERNAME}.zip ${FULLNAME}.zip
zip -rq ${VERNAME}.zip ${VERNAME}
zip -rq ${FULLNAME}.zip ${FULLNAME}
#tar zcvhf snap-1.9.tgz snap-1.9

echo `date` "Output" $WORKDIR/${FULLNAME}.zip
echo `date` "Completed SNAP Release" ${FULLNAME}

