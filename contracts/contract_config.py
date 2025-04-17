# Ethereum smart contract configuration

from web3 import Account

#improving security
PRIVATE_KEY = os.getenv("ETH_PRIVATE_KEY")
ACCOUNT = Account.from_key(PRIVATE_KEY)

# Contract address on Ethereum
CONTRACT_ADDRESS = "0x404812EfA81108163448AABd8278dD3159cc7e0d"  # Replace with actual contract address

# Contract ABI (Application Binary Interface)
CONTRACT_ABI = [
    {
        "inputs": [
            {
                "internalType": "bytes32",
                "name": "voter_hash",
                "type": "bytes32"
            }
        ],
        "name": "hasVoted",
        "outputs": [
            {
                "internalType": "bool",
                "name": "",
                "type": "bool"
            }
        ],
        "stateMutability": "view",
        "type": "function"
    },
    {
        "inputs": [
            {
                "internalType": "bytes32",
                "name": "",
                "type": "bytes32"
            }
        ],
        "name": "voted",
        "outputs": [
            {
                "internalType": "bool",
                "name": "",
                "type": "bool"
            }
        ],
        "stateMutability": "view",
        "type": "function"
    }
]