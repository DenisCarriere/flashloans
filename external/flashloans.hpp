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
