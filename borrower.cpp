#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

#include <flashloans.hpp>
#include <eosio.token.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract]] borrower : public contract {

public:
	using contract::contract;

	[[eosio::action]]
	void execute( const extended_asset ext_quantity ) {
		// trigger execution of flashloan
		flashloans::borrow_action borrowme( "flashloans"_n, { get_self(), "active"_n });
		borrowme.send( get_self(), ext_quantity, "" );
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
};
