// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

import "@openzeppelin/contracts/token/ERC20/ERC20.sol";
import "@openzeppelin/contracts/access/Ownable.sol";
// import "@openzeppelin/contracts/token/ERC20/extensions/ERC20Permit.sol";
import "./IPushCommInterface.sol";

contract CARBON is ERC20, Ownable {
    address public pushAddress;
        address public pushChannelAddress;

    uint256 public totalUsers = 0;
    uint256 public totalEnterprises = 0;
    mapping(address => bool) public isRegistered;
    mapping(address => string) public enterprise;
    mapping(string => address) public nameToAddress;
    mapping(address => address) public employeeToEnterprise;

    constructor(address pushCommAddress, address _pushChannelAddress)
        ERC20("CARBON", "RCRBN")
        Ownable(msg.sender)
    // ERC20Permit("CARBON")
    {
        pushAddress = pushCommAddress;
        pushChannelAddress = _pushChannelAddress;
    }

    function register() public {
        isRegistered[msg.sender] = true;
        totalUsers += 1;
    }

    function onboardEnterprise(string memory _name) public {
        enterprise[msg.sender] = _name;
        nameToAddress[_name] = msg.sender;
        totalEnterprises += 1;
        register();
    }

    function burn(address _toBurn, uint256 _value) public onlyOwner {
        _burn(_toBurn, _value);
        IPUSHCommInterface(pushAddress).sendNotification(
            pushChannelAddress, // from channel - recommended to set channel via dApp and put it's value -> then once contract is deployed, go back and add the contract address as delegate for your channel
            _toBurn, // to recipient, put address(this) in case you want Broadcast or Subset. For targeted put the address to which you want to send
            bytes(
                string(
                    // We are passing identity here: https://push.org/docs/notifications/notification-standards/notification-standards-advance/#notification-identity
                    abi.encodePacked(
                        "0", // this represents minimal identity, learn more: https://push.org/docs/notifications/notification-standards/notification-standards-advance/#notification-identity
                        "+", // segregator
                        "3", // define notification type:  https://push.org/docs/notifications/build/types-of-notification (1, 3 or 4) = (Broadcast, targeted or subset)
                        "+", // segregator
                        "Offset Red Carbon Credits", // this is notificaiton title
                        "+", // segregator
                        "You have just offset your Red Carbon Credits. Keep up the good work :)" // notification body
                    )
                )
            )
        );
    }

    function mint(address to, uint256 amount) public onlyOwner {
        // require(msg.sender == 0xD0daae2231E9CB96b94C8512223533293C3693Bf, "no access to mint");
        _mint(to, amount);
        IPUSHCommInterface(pushAddress).sendNotification(
            pushChannelAddress, // from channel - recommended to set channel via dApp and put it's value -> then once contract is deployed, go back and add the contract address as delegate for your channel
            to, // to recipient, put address(this) in case you want Broadcast or Subset. For targeted put the address to which you want to send
            bytes(
                string(
                    // We are passing identity here: https://push.org/docs/notifications/notification-standards/notification-standards-advance/#notification-identity
                    abi.encodePacked(
                        "0", // this represents minimal identity, learn more: https://push.org/docs/notifications/notification-standards/notification-standards-advance/#notification-identity
                        "+", // segregator
                        "3", // define notification type:  https://push.org/docs/notifications/build/types-of-notification (1, 3 or 4) = (Broadcast, targeted or subset)
                        "+", // segregator
                        "Received Red Carbon Credits", // this is notificaiton title
                        "+", // segregator
                        "You have received Red Carbon Credits. Please try to offset as much of these credits as you can." // notification body
                    )
                )
            )
        );
    }
}
