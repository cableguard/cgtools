![cableguard logo banner](./banner.png)

# Cableguard TOOL for configuring Cableguard TUN
This supplies the main userspace tooling for using and configuring WireGuard tunnels, with one extra option 'genaccount' to generate NEAR protocol implicit account json files with the account ID and key pair  that are stored in the ~/.near-credentials/ directory

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
- wireguard-tools v cableguard 1.0.20230527:

## How to Use
You need to set the blockchain network:
export BLOCKCHAIN_ENV=testnet (for testnet, mainnet for mainnet)
You may want to add this line to your .bachrc file

It is used by the rodtwallet.sh script, with the following options overall after NEAR CLI is installed:

Usage: ./walletsh/rodtwallet.sh [account_id] [Options]

Options:
-  ./walletsh/rodtwallet.sh                   : List of available accounts
-  ./walletsh/rodtwallet.sh <accountID>       : Lists the RODT Ids in the account and its balance
-  ./walletsh/rodtwallet.sh <accountID> keys  : Displays the accountID and the Private Key of the account
-  ./walletsh/rodtwallet.sh <accountID> rodtId: Displays the indicated RODT
-  ./walletsh/rodtwallet.sh <fundingaccountId> <unitializedaccountId> init   : Initializes account with 0.01 NEAR from funding acount
-  ./walletsh/rodtwallet.sh <originaccountId>  <destinationaccountId> rodtId : Sends ROTD from origin account to destination account
-  ./walletsh/rodtwallet.sh genaccount        : Creates a new uninitialized accountID

# Cableguard Ecosystem
- Cableguard TUN: VPN tunnels
- Cableguard TOOLS: local VPN tunnel configuration
- Cableguard FORGE: RODT minter
- Cableguard WALLET: RODT manager
- Cableguard AUTH: RODT authentication for interoperability with implementation of the Triangle of Trust.
- Cableguard FIND: Server and peer finder
