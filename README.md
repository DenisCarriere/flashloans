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

```c++
[[eosio::action]]
void borrow( const name to, const asset quantity, const string memo )
{
    require_auth( to );
    const name contract = "eosio.token"_n;

    // get initial balance of contract
    const asset balance = token::get_balance( contract, get_self(), symcode );
    check( balance.amount >= quantity.amount, "cannot request above current balance");

    // transfer funds to borrower
    token::transfer_action transfer( contract, { get_self(), "active"_n });
    transfer.send( get_self(), to, quantity, memo );

    // check if balance is above
    checkbalance_action checkbalance( get_self(), { get_self(), "active"_n });
    checkbalance.send( get_self(), balance );
}

[[eosio::action]]
void checkbalance( const name account, const asset quantity )
{
    // check balance of account, if below the desired amount, fail the transaction
    const asset balance = token::get_balance( "eosio.token"_n, account, quantity.symbol.code() );
    check( balance.amount >= quantity.amount, account.to_string() + " must have a balance equal or above " + quantity.to_string() );
}
```

## Borrower - Basic implementation

1. `execute` to trigger `borrow` action from flashloans
2. `on_notify` to intercept incoming token transfer from borrowing flashloan
  - Custom logic (trading, arbitrage, etc..)
  - Repay loan at the end.

```c++
[[eosio::action]]
void execute( const asset quantity ) {
    // trigger execution of borrowing a flashloans
    flashloans::borrow_action borrow( "flashloans"_n, { to, "active"_n });
    borrow.send( get_self(), quantity, "" );
}

[[eosio::on_notify("*::transfer")]]
void on_transfer( const name from, const name to, const asset quantity, const string memo )
{
    // only intercept transfers from flashloans
    if ( from != "flashloans"_n ) return;

    // incoming token contract
    const name contract = get_first_receiver();

    // do actions before sending funds back
    // =====>>>> PLACE YOUR CODE HERE

    // repay flashloan
    token::transfer_action transfer( contract, { get_self(), "active"_n });
    transfer.send( get_self(), from, quantity, memo );
}
```

## Build

```bash
eosio-cpp examples/flashloans.cpp -o flashloans.wasm -I external
eosio-cpp examples/borrower.cpp -o borrower.wasm -I external
```