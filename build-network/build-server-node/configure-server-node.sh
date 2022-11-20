# Build DHCP and DNS server node for Minimum HART-IP 2.0 Network

# run this script with an internet connection
# after running, the IP address will have changed, internet will not be available

# Usage: copy files in this folder to the home folder, run 
#   source configure-server-node.sh

# On first login you will change the password 
#   from default of user: ubuntu pw: ubuntu 
#   to user: ubuntu pw: localadmin

# do we need these?
# sudo hostnamectl set-hostname local


sudo apt remove unattended-upgrades
sudo apt update
sudo apt install net-tools
sudo apt install openssl
#sudo systemctl status ssh

# before installing DNSMasq, stop existing DNS service listening on port 53
sudo systemctl disable systemd-resolved
sudo systemctl stop systemd-resolved
echo nameserver 8.8.8.8  | sudo tee /etc/resolv.conf

# DNSMasq: simple DNS and DHCP service
sudo apt install dnsmasq
sudo mkdir /var/lib/dnsmasq
sudo cp dnsmasq.conf /etc
sudo systemctl restart dnsmasq

# ensure port 53 is accessible for DNS service
sudo ufw allow from any to any port 53 proto tcp
sudo ufw allow from any to any port 53 proto udp

# this must execute after DHCP is configured in DNSMasq
sudo cp *.yaml /etc/netplan
sudo netplan apply

# record static IPv4 address on eth0
ip addr
