#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

#include <eosio.token.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract]] flashloans : public contract {

public:
	using contract::contract;

	[[eosio::action]]
	void borrow( const name to, const extended_asset ext_quantity, const string memo )
	{
		require_auth( to );

		// expand extended asset
		const name contract = ext_quantity.contract;
		const asset quantity = ext_quantity.quantity;

		// get initial balance of contract
		const asset balance = token::get_balance( contract, get_self(), quantity.symbol.code() );
		check( balance.amount >= quantity.amount, "cannot request above current balance");

		// transfer funds to borrower
		token::transfer_action transfer( contract, { get_self(), "active"_n });
		transfer.send( get_self(), to, quantity, memo );

		// check if balance is above
		checkbalance_action checkbalance( get_self(), { get_self(), "active"_n });
		checkbalance.send( get_self(), extended_asset{ balance, contract } );
	}

	[[eosio::action]]
	void checkbalance( const name account, const extended_asset ext_quantity )
	{
		// expand extended asset
		const name contract = ext_quantity.contract;
		const asset quantity = ext_quantity.quantity;

		// check balance of account, if below the desired amount, fail the transaction
		const asset balance = token::get_balance( contract, account, quantity.symbol.code() );
		check( balance.amount >= quantity.amount, account.to_string() + " must have a balance equal or above " + quantity.to_string() );
	}

	// action wrappers
	using borrow_action = eosio::action_wrapper<"borrow"_n, &flashloans::borrow>;
	using checkbalance_action = eosio::action_wrapper<"checkbalance"_n, &flashloans::checkbalance>;
};
