# Build HART-IP 2.0 Flow Device

# run this script with an internet connection
# after running, the IP address will have changed, internet will not be available

# Usage: copy files in this folder to the home folder, run 
#   source configure-flow-device.sh

sudo apt remove unattended-upgrades
sudo apt update
sudo apt install net-tools
sudo apt install openssl
sudo apt install libssl-dev
sudo apt-get install -y ninja-build
sudo apt-get install -y gdb
sudo apt install git-all
sudo apt install make
sudo apt install g++
sudo apt install libjsoncpp-dev
sudo apt install ssh

# if DHCP is not available on the server node, use static IP address for the Flow Device 
# sudo cp -r *.yaml /etc/netplan/
# sudo netplan apply

echo
echo
echo record the IP address:
ip addr

# check that SSH is enabled
# sudo systemctl status ssh

# add your access credentials to github
echo
echo
echo generate a SSH key for your RPi: https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent
echo and here: https://docs.github.com/en/authentication/connecting-to-github-with-ssh
echo add SSH keys to your github account here: https://github.com/settings/keys

echo
echo
echo after you have installed your keys to github,
echo pull the repositoies and build them using commandline action:
echo source pull-build.sh

