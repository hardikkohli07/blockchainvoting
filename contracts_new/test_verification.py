import requests
import json
import sys

def test_verification_api(voter_hash):
    """
    Test the verification API endpoint with a given voter hash.
    
    Args:
        voter_hash: Hex string of the voter hash to test
    """
    # API endpoint URL
    url = "http://localhost:8000/verify"
    
    # Ensure voter_hash has 0x prefix
    if not voter_hash.startswith("0x"):
        voter_hash = "0x" + voter_hash
    
    # Prepare request payload
    payload = {"voter_hash": voter_hash}
    headers = {"Content-Type": "application/json"}
    
    try:
        # Send POST request to the API
        response = requests.post(url, data=json.dumps(payload), headers=headers)
        
        # Check if request was successful
        if response.status_code == 200:
            result = response.json()
            print(f"Verification result: {result['status']}")
            print(f"Voter hash {voter_hash} is {'already used' if result['status'] == 'rejected' else 'valid for voting'}")
        else:
            print(f"Error: {response.status_code}")
            print(response.text)
    
    except requests.exceptions.RequestException as e:
        print(f"Request failed: {e}")
    except json.JSONDecodeError:
        print("Failed to parse response as JSON")
    except Exception as e:
        print(f"Unexpected error: {e}")

if __name__ == "__main__":
    # Get voter hash from command line argument or use a default value
    voter_hash = sys.argv[1] if len(sys.argv) > 1 else "0x1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef"
    
    print(f"Testing verification API with voter hash: {voter_hash}")
    test_verification_api(voter_hash)
    
    print("\nUsage example:")
    print("python test_verification.py 0x1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef")