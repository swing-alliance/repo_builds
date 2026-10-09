#pragma once
#include<string>


namespace v_sim{
	class v_global_tracker;
	class v_account {
	public:
		std::string ac_id;
		float ac_val;   //º€÷µ
		bool v_buy_fund(v_global_tracker& vt,std::string symbol,float val);
		bool v_sell_fund(v_global_tracker& vt, std::string symbol, float ratio);

	};

}






