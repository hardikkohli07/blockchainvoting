from dotenv import load_dotenv
load_dotenv()

from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field, field_validator
from web3 import Web3
from web3.middleware import ExtraDataToPOAMiddleware
import re
import os
from typing import Dict
from contract_config import CONTRACT_ABI, CONTRACT_ADDRESS, ACCOUNT

app = FastAPI(title="E-Voting API")

# Initialize Web3 connection
INFURA_KEY = os.getenv("INFURA_KEY")
if not INFURA_KEY:
    raise ValueError("INFURA_KEY environment variable is not set")

w3 = Web3(Web3.HTTPProvider(f"https://sepolia.infura.io/v3/{INFURA_KEY}"))
w3.middleware_onion.inject(ExtraDataToPOAMiddleware(), layer=0)
w3.eth.default_account = ACCOUNT.address

# Load contract
contract = w3.eth.contract(address=CONTRACT_ADDRESS, abi=CONTRACT_ABI)

class VoterIDRequest(BaseModel):
    voter_id: str = Field(..., min_length=12, max_length=12)
    
    @field_validator('voter_id')
    @classmethod  # Required for class methods in V2
    def validate_voter_id(cls, v: str) -> str:
        if not v.isdigit():
            raise ValueError("Voter ID must contain only numbers")
        return v

class VerificationResponse(BaseModel):
    status: str

@app.post("/submit_voter_id", response_model=VerificationResponse)
async def submit_voter_id(request: VoterIDRequest):
    try:
        # Convert to bytes32 hash
        voter_hash = Web3.keccak(text=request.voter_id).hex()
        
        # Check if already voted
        has_voted = contract.functions.hasVoted(voter_hash).call()
        if has_voted:
            return {"status": "rejected"}
            
        # Build and send transaction
        txn = contract.functions.recordVote(voter_hash).build_transaction({
            'from': ACCOUNT.address,
            'nonce': w3.eth.get_transaction_count(ACCOUNT.address),
            'gas': 100000,
            'gasPrice': w3.to_wei('20', 'gwei')
        })
        
        signed_txn = w3.eth.account.sign_transaction(txn, private_key=ACCOUNT.privateKey)
        tx_hash = w3.eth.send_raw_transaction(signed_txn.rawTransaction)
        w3.eth.wait_for_transaction_receipt(tx_hash)
        
        return {"status": "accepted"}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

if __name__ == "__main__":
    import uvicorn
    uvicorn.run("voting_verification:app", host="0.0.0.0", port=8000)