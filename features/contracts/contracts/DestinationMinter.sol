// SPDX-License-Identifier: MIT
pragma solidity 0.8.20;

import {CCIPReceiver} from "@chainlink/contracts-ccip/src/v0.8/ccip/applications/CCIPReceiver.sol";
import {Client} from "@chainlink/contracts-ccip/src/v0.8/ccip/libraries/Client.sol";
import {CARBON} from "./greenscreen.sol";

contract DestinationMinter is CCIPReceiver {
    CARBON carbon;

    event MintCallSuccessfull();

    constructor(address router) CCIPReceiver(router) {

    }
    function setCarbonAddress(address _carbonAddress) public {
        carbon = CARBON(_carbonAddress);
    }

    function _ccipReceive(
        Client.Any2EVMMessage memory message
    ) internal override {
        (bool success, ) = address(carbon).call(message.data);
        require(success);
        emit MintCallSuccessfull();
    }
}
