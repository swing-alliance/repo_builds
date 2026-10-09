#include "virtual/v_tracker.h"
#include "virtual/v_account.h"
#include "virtual/v_brokerage.h"
#include"virtual/static_enum.h"

#include<model/fund_model.h>
#include<utility>
#include<core/apptime.h>
#include<vector>
#include <cmath>
#include <limits>
#include<time.h>
#include"virtual/json.h"
#include<iostream>
using json = nlohmann::json;

namespace v_sim {
	bool v_global_tracker::v_mk_buy_trade_log(std::string ac_id, std::string symbol, int type, float buy_m) {
		trade_log log;
		log.ac_id = ac_id;
		log.symbol = symbol;
		log.type = type;
		log.evt = buy_evt;
		if (log.type == abt_fund) {
			for (int i = 0; i < this->fund_datasource.data_nums(); i++) {
				if (this->fund_datasource.funds_list[i].fund_code == symbol) {
					std::pair<int, int> rc = this->fund_datasource.funds_list[i].get_split_fund_model(1);
					if (rc.first == -1 || rc.second == -1) { return false; } //失败了，回退
					log.deal_p = this->fund_datasource.funds_list[i].data[rc.second].accum_val;   //直接最后一天的净值买进去
					log.deal_num = buy_m / log.deal_p;
					log.app_t = core::getnow_string();
					log.status = checked;
					this->trade_logs.push_back(log);
					return true;
				}
			}
			return false;
		}
		else if (log.type == abt_stock) { std::cout << "暂未实现" << std::endl; return false; }
		return false;
	}

	bool v_global_tracker::v_mk_sell_trade_log(std::string ac_id, std::string symbol, int type,float sell_ratio) {
		trade_log log;
		log.ac_id = ac_id;
		log.symbol = symbol;
		log.type = type;
		log.evt = sell_evt;
		if (type == abt_fund) {
			float num = 0.0f;
			for (int i = 0; i < this->trade_logs.size(); i++) {
				if (this->trade_logs[i].ac_id == ac_id && this->trade_logs[i].symbol==symbol) {
					if (this->trade_logs[i].evt == buy_evt) {
						num += this->trade_logs[i].deal_num;
					}else if (this->trade_logs[i].evt == sell_evt){
						num -= this->trade_logs[i].deal_num;      //不管卖出到账与否立刻扣掉份额
					}
				}
			}
			if (num <= -0.00000001) { std::cout << "不可能出现" << std::endl; return false; }
			if (std::fabs(num) < 1e-6f) { return false; }   //约等于0的情况
			if (sell_ratio <= 0.0f || sell_ratio > 1.0f) return false;
			log.deal_num = num * sell_ratio;
			for (int i = 0; i < this->fund_datasource.data_nums(); i++) {
				if (this->fund_datasource.funds_list[i].fund_code == symbol) {
					std::pair<int, int> rc = this->fund_datasource.funds_list[i].get_split_fund_model(1);
					if (rc.first == -1 || rc.second == -1) { return false; } //失败了，回退
					log.deal_p = this->fund_datasource.funds_list[i].data[rc.second].accum_val;   //直接最后一天的净值卖进去
					log.app_t = core::getnow_string();
					log.status = checked;
					this->trade_logs.push_back(log);
					transfer_log tsf_log;
					tsf_log.amount = log.deal_p * log.deal_num;
					time_t tnow = core::intf_time(log.app_t);
					tsf_log.perform_date = core::strf_time(tnow + 60 * 60 * 24);                            //t+n日后回到账户,先写一天后返回
					tsf_log.status = uncheck;
					tsf_log.to_ac_id = log.ac_id;
					this->trasfer_logs.push_back(tsf_log);
					return true;
				}
			}
			return false;
		}
		else if (type == abt_stock) { std::cout << "暂未实现" << std::endl; return false;}
		return false;
	}

	void v_global_tracker::v_export_logs() {
		// ---------- 1. 导出交易日志 ----------
		FILE* fp1 = fopen("./v_trade_log", "w");
		if (!fp1) {
			perror("v_trade_log 打开失败");
			return;
		}
		json big_j = json::array();   // 明确是数组
		for (size_t i = 0; i < this->trade_logs.size(); i++) {
			const auto& log = this->trade_logs[i];
			json j;
			j["ac_id"] = log.ac_id;
			j["symbol"] = log.symbol;
			j["type"] = log.type;
			j["evt"] = log.evt;
			j["deal_p"] = log.deal_p;
			j["deal_num"] = log.deal_num;
			j["status"] = log.status;
			j["app_t"] = log.app_t;
			big_j.push_back(j);       //关键：加进数组
		}
		std::string s1 = big_j.dump(4);   // 缩进 4 空格
		fwrite(s1.c_str(), 1, s1.size(), fp1);
		fclose(fp1);                      //关文件
		// ---------- 2. 导出转账日志 ----------
		FILE* fp2 = fopen("./v_transfer_log", "w");
		if (!fp2) {
			perror("v_transfer_log 打开失败");
			return;
		}
		json big_j2 = json::array();
		for (size_t i = 0; i < this->trasfer_logs.size(); i++) {
			const auto& t = this->trasfer_logs[i];
			json j;
			j["to_ac_id"] = t.to_ac_id;
			j["amount"] = t.amount;
			j["status"] = t.status;
			j["perform_date"] = t.perform_date;
			big_j2.push_back(j);
		}
		std::string s2 = big_j2.dump(4);
		fwrite(s2.c_str(), 1, s2.size(), fp2);
		fclose(fp2);
	} //导出日志

	void v_global_tracker::v_check_to_do(v_account& addr) {
		for (int i = 0; i < this->trasfer_logs.size(); i++) {
			if (this->trasfer_logs[i].status == uncheck && core::intf_time(this->trasfer_logs[i].perform_date) <= core::intf_time(core::getnow_string())){
				if (addr.ac_id == this->trasfer_logs[i].to_ac_id) {
					addr.ac_val += this->trasfer_logs[i].amount;
					this->trasfer_logs[i].status = checked;
				}
			}
		}
		return;
	}

	bool v_global_tracker::check_is_trade_day(std::string symbol) { return false; }

	bool v_global_tracker::check_is_trading_time() { return true; }

}



