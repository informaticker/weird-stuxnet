#!/bin/sh
#
# Author: Fabian Schläpfer
# Date:   July 7th 2010
#
# This script simply updates the local version of our project
# with an up-to-date version straight from the repository
#

# path to an already existing checked out revision of the project
repo="/root/svn/eco"

# update the repository as user 'fabian' (the user who has pubkey authentication on the remote SVN server
# and who checked out the repository in the first place)
su -c "svn update --username fabian --password gggggg --non-interactive /root/svn/eco/" fabian
