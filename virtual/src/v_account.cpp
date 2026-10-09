#include"virtual/v_account.h"
#include"virtual/v_tracker.h"
#include<string>
#include"virtual/static_enum.h"
namespace v_sim {
	class v_global_tracker;
	bool v_account::v_buy_fund(v_global_tracker& vt,std::string symbol,float val){
		if (val > this->ac_val) {
			return false;
		}
		if (vt.v_mk_buy_trade_log(this->ac_id, symbol, abt_fund, val)) {
			this->ac_val -= val;
			return true;
		}
		return false;
	}

	bool v_account::v_sell_fund(v_global_tracker& vt, std::string symbol, float ratio) {
		if (ratio > 1 || ratio < 0) { return false; }
		if (vt.v_mk_sell_trade_log(this->ac_id,symbol,abt_fund,ratio)){
			return true;
		}
		return false;
	}

}
