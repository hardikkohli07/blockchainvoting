// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

contract SecureVoterVerification {
    mapping(bytes32 => bool) private _voted;
    address private _owner;
    uint256 public voteCount;
    
    event VoteRecorded(bytes32 indexed voterHash, uint256 timestamp);
    event OwnershipTransferred(address indexed previousOwner, address indexed newOwner);

    modifier onlyOwner() {
        require(msg.sender == _owner, "Caller is not owner");
        _;
    }

    constructor() {
        _owner = msg.sender;
    }

    function hasVoted(bytes32 voterHash) public view returns (bool) {
        return _voted[voterHash];
    }

    function recordVote(bytes32 voterHash) external onlyOwner {
        require(!_voted[voterHash], "Voter already voted");
        require(voterHash != bytes32(0), "Invalid voter hash");
        
        _voted[voterHash] = true;
        voteCount++;
        emit VoteRecorded(voterHash, block.timestamp);
    }

    function transferOwnership(address newOwner) public onlyOwner {
        require(newOwner != address(0), "Invalid owner address");
        _owner = newOwner;
        emit OwnershipTransferred(_owner, newOwner);
    }
}