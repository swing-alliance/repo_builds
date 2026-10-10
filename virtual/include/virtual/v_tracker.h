#pragma once
#include<string>
#include<vector>
#include<core/apptime.h>
#include<model/fund_model.h>
#include "v_account.h"
#include "v_brokerage.h"
namespace v_sim {
	class trade_log {
	public:
			std::string ac_id;
			std::string symbol;
			int type;
			int evt;
			float deal_p;  //成交价格
			float deal_num;  //成交数量
			int status;
			std::string log_t;
	};

	class transfer_log {
	public:
		std::string to_ac_id;
		float amount;
		int status;
		std::string perform_date;
	};

	



	class v_global_tracker {
	public:
		model::datas_fund_manager fund_datasource;
		bool v_mk_buy_trade_log(std::string ac_id,std::string symbol, int type,float buy_m);
		bool v_mk_sell_trade_log(std::string ac_id, std::string symbol, int type,float sell_ratio);
		void v_check_to_do(v_account& addr);     //检查是不是该做一些事情
		void v_export_logs(); 
		bool check_is_trade_day(std::string symbol);
		bool check_is_trading_time();
		int  get_trasfer_days(std::string symbol);   //获取这个返回是t+几？？需要标准加载，低级加载默认返回t+1
		std::vector<transfer_log> trasfer_logs;
		std::vector<trade_log> trade_logs;
	};


}