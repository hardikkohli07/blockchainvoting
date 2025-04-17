import requests
import json
import sys

def test_verification_api(voter_id):
    """
    Test the verification API endpoint with a given voter_id.
    Args:
        voter_id: 12-digit string of the voter ID to test
    """
    url = "http://localhost:8000/submit_voter_id"
    payload = {"voter_id": voter_id}
    headers = {"Content-Type": "application/json"}
    
    try:
        response = requests.post(url, data=json.dumps(payload), headers=headers)
        if response.status_code == 200:
            result = response.json()
            print(f"Verification result: {result['status']}")
            print(f"Voter ID {voter_id} is {'already used' if result['status'] == 'rejected' else 'valid for voting'}")
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
    voter_id = sys.argv[1] if len(sys.argv) > 1 else "123456789012"
    print(f"Testing verification API with voter ID: {voter_id}")
    test_verification_api(voter_id)
    print("\nUsage example:")
    print("python test_verification.py 123456789012")