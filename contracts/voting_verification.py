from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field, validator
from web3 import Web3
from web3.exceptions import ContractLogicError, BlockNotFound, TransactionNotFound
import re
import os
from typing import Dict, Any

# Import your contract ABI and address from your module
# Assuming these are defined elsewhere in your project
from contract_config import CONTRACT_ABI, CONTRACT_ADDRESS

app = FastAPI(title="E-Voting Verification API")

# Initialize Web3 connection
# Replace with your Ethereum node URL (e.g., Infura, Alchemy, or local node)
# Get API key from environment variable
INFURA_KEY = os.getenv("INFURA_KEY")
if not INFURA_KEY:
    raise ValueError("INFURA_KEY environment variable is not set")
    
w3 = Web3(Web3.HTTPProvider(f"https://sepolia.infura.io/v3/{INFURA_KEY}"))
#improving security layer
w3.eth.default_account = ACCOUNT.address


# Initialize the contract
contract = w3.eth.contract(address=CONTRACT_ADDRESS, abi=CONTRACT_ABI)

class VoterHashRequest(BaseModel):
    voter_hash: str = Field(..., description="Hex string of the voter hash")
    
    @validator('voter_hash')
    def validate_voter_hash(cls, v):
        # Check if it's a valid hex string (with or without 0x prefix)
        if not re.match(r'^(0x)?[0-9a-fA-F]+$', v):
            raise ValueError("voter_hash must be a valid hexadecimal string")
        
        # Remove 0x prefix for length check
    v_without_prefix = v[2:] if v.startswith('0x') else v
    
    # Check length (bytes32 = 32 bytes = 64 hex characters)
    if len(v_without_prefix) != 64:
        raise ValueError("voter_hash must be exactly 32 bytes (64 hex characters)")

        # Ensure it has 0x prefix for web3.py
        if not v.startswith('0x'):
            v = '0x' + v
            
        return v

class VerificationResponse(BaseModel):
    status: str

@app.post("/verify", response_model=VerificationResponse)
async def verify_voter(request: VoterHashRequest) -> Dict[str, str]:
    """Verify if a voter has already voted by checking the smart contract.
    
    Args:
        request: JSON containing the voter_hash
        
    Returns:
        JSON with status "rejected" if already voted, "accepted" otherwise
    """
    try:
        # Check if Web3 is connected
        if not w3.is_connected():
            raise HTTPException(
                status_code=503, 
                detail="Cannot connect to Ethereum node"
            )
        
        # Call the smart contract's hasVoted function
        has_voted = contract.functions.hasVoted(request.voter_hash).call()
        
        # Return appropriate response based on the result
        if has_voted:
            return {"status": "rejected"}
        
        # Record vote transaction
        tx_hash = contract.functions.recordVote(request.voter_hash).transact({
            'from': w3.eth.default_account,
            'gas': 100000
        })
        
        # Wait for transaction receipt
        receipt = w3.eth.wait_for_transaction_receipt(tx_hash)
        
        return {"status": "accepted"}
            
    except (ValueError, TypeError) as e:
        # Handle invalid input
        raise HTTPException(
            status_code=400,
            detail=f"Invalid input: {str(e)}"
        )
    except (ContractLogicError, BlockNotFound, TransactionNotFound) as e:
        # Handle contract-related errors
        raise HTTPException(
            status_code=500,
            detail=f"Smart contract error: {str(e)}"
        )
    except Exception as e:
        # Handle any other unexpected errors
        raise HTTPException(
            status_code=500,
            detail=f"Unexpected error: {str(e)}"
        )

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(
        app, 
        host="0.0.0.0", 
        port=8000,
        ssl_keyfile="key.pem",
        ssl_certfile="cert.pem",
        ssl_ca_certs="ca.pem"
    )