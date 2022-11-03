# this script assumes that you have added ssh keys for this machine to your github account

cd ~/

rm -rf ~/private.hipserver

rm -rf ~/private.hipflowapp

git clone git@github.com:FieldCommGroup/private.hipserver.git

# purposefully not recursive
git clone git@github.com:FieldCommGroup/private.hipflowapp.git

cp -r ~/private.hipserver ~/private.hipflowapp/hipflowapp/Hip_Native
rm -rf ~/private.hipflowapp/hipflowapp/Hip_Native/hipserver
mv ~/private.hipflowapp/hipflowapp/Hip_Native/private.hipserver  ~/private.hipflowapp/hipflowapp/Hip_Native/hipserver

source ~/build-hipserver.sh

source ~/build-hipflowapp.sh
