# this script assumes that you have added ssh keys for this machine to your github account

cd ~/

rm -rf ~/hipserver

rm -rf ~/hipflowapp

git clone git@github.com:FieldCommGroup/hipserver.git

# purposefully not recursive
git clone git@github.com:FieldCommGroup/hipflowapp.git

cp -r ~/hipserver ~/hipflowapp/hipflowapp/Hip_Native
rm -rf ~/hipflowapp/hipflowapp/Hip_Native/hipserver
mv ~/hipflowapp/hipflowapp/Hip_Native/hipserver  ~/hipflowapp/hipflowapp/Hip_Native/hipserver

source ~/build-hipserver.sh

source ~/build-hipflowapp.sh
