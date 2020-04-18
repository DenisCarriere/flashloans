# `Flashloan` ⚡️

> Flashloan - EOSIO Smart Contract Plugin

## Flashloan Concept

This code sample is meant to be used a code insert in any library which contains liquidity which want offer "flash loan" capabilities.

- Borrower can borrow any amount without any deposit
- Flash loan provider charges 0% fees for flash loan

Introducing free access to liquidity allows developers to come up with clever smart contracts designs.

## Flashloans - Basic implementation

1. `borrow` is the request from the user to borrow a fixed amount of assets which must be paid off by the end of the transaction or else the transaction fails.
2. `checkbalance` affirms that the contract account has equal or greater balance from the initiation.

**flashloans.hpp**

```c++
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract]] flashloans : public contract {

public:
	using contract::contract;

	[[eosio::action]]
	void borrow( const name to, const extended_asset ext_quantity, const string memo );

	[[eosio::action]]
	void checkbalance( const name account, const extended_asset ext_quantity );

	// action wrappers
	using borrow_action = eosio::action_wrapper<"borrow"_n, &flashloans::borrow>;
	using checkbalance_action = eosio::action_wrapper<"checkbalance"_n, &flashloans::checkbalance>;
};
```

## Borrower - Basic implementation

1. `execute` to trigger `borrow` action from flashloans
2. `on_notify` to intercept incoming token transfer from borrowing flashloan
  - Custom logic (trading, arbitrage, etc..)
  - Repay loan at the end.

```c++
[[eosio::action]]
void execute( const asset quantity ) {
    // triggers execution of flashloans
    flashloans::borrow_action borrow( "flashloans"_n, { to, "active"_n });
    borrow.send( get_self(), quantity, "" );
}

[[eosio::on_notify("*::transfer")]]
void on_transfer( const name from, const name to, const asset quantity, const string memo )
{
    // only intercept transfers from flashloans
    if ( from != "flashloans"_n ) return;

    // do actions before sending funds back
    // =====>>>> PLACE YOUR CODE HERE

    // repay flashloan
    token::transfer_action transfer( get_first_receiver(), { get_self(), "active"_n });
    transfer.send( get_self(), from, quantity, memo );
}
```

## Build

```bash
eosio-cpp examples/flashloans.cpp -o flashloans.wasm -I external
eosio-cpp examples/borrower.cpp -o borrower.wasm -I external
```