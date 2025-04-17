# Electronic Voting Machine Verification API

This FastAPI application provides a verification endpoint for an ESP32-based electronic voting machine system. It verifies voter hashes against an Ethereum smart contract to prevent double voting.

## System Overview

The complete system consists of:

1. **ESP32 Hardware Device** with:
   - TFT display
   - Fingerprint scanner
   - Keyboard matrix

2. **Verification API** (this component):
   - Receives voter hashes from the ESP32 device
   - Verifies against the Ethereum blockchain
   - Returns acceptance/rejection status

3. **Ethereum Smart Contract**:
   - Maintains a mapping of voter hashes
   - Prevents double voting

## API Endpoint

### POST /verify

Verifies if a voter has already voted by checking the smart contract.

**Request:**
```json
{
  "voter_hash": "0x1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef"
}
```

**Response:**
- If voter has not voted:
```json
{
  "status": "accepted"
}
```
- If voter has already voted:
```json
{
  "status": "rejected"
}
```

## Setup Instructions

1. Install dependencies:
```bash
pip install fastapi uvicorn web3 pydantic
```

2. Configure your Ethereum node:
   - Update the Web3 provider URL in `voting_verification.py`
   - Replace `YOUR_INFURA_KEY` with your actual Infura API key or use another Ethereum node provider

3. Configure your smart contract:
   - Update `CONTRACT_ADDRESS` in `contract_config.py` with your deployed contract address
   - Verify the ABI matches your deployed contract

4. Run the server:
```bash
uvicorn voting_verification:app --host 0.0.0.0 --port 8000 --reload
```

5. Access the API documentation:
   - Open your browser to `http://localhost:8000/docs`

## Security Considerations

- This API uses HTTPS/mTLS for secure communication with the ESP32 device
- Voter IDs and fingerprints are hashed together to create the voter_hash
- The smart contract ensures each voter can only vote once

## Error Handling

The API includes comprehensive error handling for:
- Invalid input formats
- Ethereum node connectivity issues
- Smart contract errors
