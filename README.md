![cableguard logo banner](./banner.png)

# Cableguard TOOL for configuring Cableguard TUN
This supplies the main userspace tooling for using and configuring WireGuard tunnels, with two extra options 'genaccount' to generate NEAR protocol implicit account json files with the account ID and key pair that are stored in the ~/.near-credentials/$BLOCKCHAIN_ENV directory

## License
This project is released under the [GPLv2](COPYING).
More information may be found at [WireGuard.com](https://www.wireguard.com/).**

## How to Install from Source
You may need to install libssl-dev with: sudo apt-get install libssl-dev

From the cgtools directory where the source code is downloaded
- make -C ./src -j$(nproc)
- sudo make -C ./src install

With the command
- wg --version
you should get
- wireguard-tools v cableguard 0.90.53:

##How to install form Deb package
wget https://cableguard.fra1.digitaloceanspaces.com/cgtools_00.90.53_amd64.deb
sudo apt install ./cgtools_00.90.53_amd64.deb

## How to Use
You need to set the blockchain network:
export BLOCKCHAIN_ENV=testnet (for testnet, mainnet for mainnet)
You may want to add this line to your .bashrc file

run wg help to display options. The options genaccount and subdomain-peer are not found in defaults wg from Wireguard

# Cableguard Ecosystem
- Cableguard RODIVPN: RODiT and VPN manager
- Cableguard TOOLS: local VPN tunnel configuration
- Cableguard TUN: VPN tunnels
- Cableguard FORGE: RODiT minter

---
<sub><sub><sub><sub>WireGuard is a registered trademark of Jason A. Donenfeld. Cableguard is not sponsored or endorsed by Jason A. Donenfeld.</sub></sub></sub></sub>