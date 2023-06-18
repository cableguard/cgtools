![cableguard logo banner](./banner.png)

# Cableguard TOOL for configuring Cableguard TUN
This is exactly the same as Wireguard tools, with one extra option 'genaccount' to generate NEAR protocol implicit account json files with the account ID and key pair  that are stored in the ~/.near-credentials/ directory

It is used by the rodtwallet.sh script, with the following options overall after NEAR CLI is installed:

Usage: ./walletsh/rodtwallet.sh [account_id] [Options]

Options:
-  ./walletsh/rodtwallet.sh                       : List of available accounts
-  ./walletsh/rodtwallet.sh <accountID>           : Lists the RODT Ids in the account and its balance
-  ./walletsh/rodtwallet.sh <accountID> keys      : Displays the accountID and the Private Key of the account
-  ./walletsh/rodtwallet.sh <accountID> <RODT Id> : Displays the indicated RODT
-  ./walletsh/rodtwallet.sh <funding accountId> <unitialized accountId> init    : Initializes account with 0.01 NEAR from funding acount
-  ./walletsh/rodtwallet.sh <origin accountId>  <destination accountId> <rotid> : Sends ROTD from origin account to destination account
-  ./walletsh/rodtwallet.sh genaccount            : Creates a new uninitialized accountID

# Wireguard Tools
This supplies the main userspace tooling for using and configuring WireGuard tunnels. More information may be found at [WireGuard.com](https://www.wireguard.com/).**

## License
This project is released under the [GPLv2](COPYING).
