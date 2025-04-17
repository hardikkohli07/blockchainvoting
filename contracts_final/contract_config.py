import os
from web3 import Account

# Get private key from environment
PRIVATE_KEY = os.getenv("ETH_PRIVATE_KEY")
if not PRIVATE_KEY:
    raise ValueError("ETH_PRIVATE_KEY environment variable is not set")

# Create account from private key
ACCOUNT = Account.from_key(PRIVATE_KEY)

# Contract address on Ethereum
CONTRACT_ADDRESS = "0x404812EfA81108163448AABd8278dD3159cc7e0d"

# Contract ABI (Application Binary Interface)
CONTRACT_ABI = [
    {
        "inputs": [{"internalType": "bytes32", "name": "voter_hash", "type": "bytes32"}],
        "name": "hasVoted",
        "outputs": [{"internalType": "bool", "name": "", "type": "bool"}],
        "stateMutability": "view",
        "type": "function"
    },
    {
        "inputs": [{"internalType": "bytes32", "name": "voter_hash", "type": "bytes32"}],
        "name": "recordVote",
        "outputs": [],
        "stateMutability": "nonpayable",
        "type": "function"
    },
    {
        "inputs": [{"internalType": "bytes32", "name": "", "type": "bytes32"}],
        "name": "voted",
        "outputs": [{"internalType": "bool", "name": "", "type": "bool"}],
        "stateMutability": "view",
        "type": "function"
    }
]
