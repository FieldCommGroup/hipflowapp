Building and running the flow device


To login

	ubuntu/user1user1

To pull all the repository's and configure them correctly and then build them:

	source pull-run.sh

To reset the device to factory conditions:

	source reset.sh

To run the device, type:

	source run.sh


Other:
* if you have been building and running on this RPi, then no need to check these
* assume SSH keys for github are installed (if not, git pull will not work)
* aepositories are private.hipserver, private.hipflowapp
* uses whatever git branches are currently checked out
* default branches are cw-2020 and cw-2020emerson
* git command to check branches is 'git branch'