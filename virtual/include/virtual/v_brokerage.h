#pragma once
#include<string>
#include<vector>
#include"v_account.h"
namespace v_sim{
	class v_brokerage {
	public:
		std::vector<v_account> all_accounts;    //所有参加模拟的账号
		void add_account(v_account& v_ac);
		void is_trade_day();
		
	};


}












