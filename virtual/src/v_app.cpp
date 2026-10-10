#include"virtual/v_account.h"
#include"virtual/v_brokerage.h"
#include"virtual/v_tracker.h"
#include"virtual/v_app.h"
#include"virtual/static_enum.h"
#include<iostream>
#include<string>
#include<core/apptime.h>
#include<core/paths.h>
namespace v_sim {
	void v_app::init_single_player_fund(std::string start_time){
		this->vt.fund_datasource.fast_low_level_load_data(core::funds_row_dir);
		this->v_usetime_offset(core::intf_time(start_time)-core::getnow());
		this->player1.ac_id = "player_1";
		this->player1.ac_val = 10000;
		while (1) {
			int opt_code;
			std::cout << "1-时间偏移,2-买入，3-卖出，4-导出日志快照，5-查看余额，6-退出,7-查看app时间,8-跳到下个交易日,9-查看前n个基金代码" << std::endl;
			std::cin >> opt_code;
			switch (opt_code) {
			case 1: { long long seconds;  std::cout << "输入偏移时间:"; std::cin >> seconds; this->v_usetime_offset(seconds); break; }
			case 2: { 
				std::string symbol;
				float val;
				std::cout << "输入代码:";
				std::cin >> symbol;
				std::cout << "输入买入总钱:";
				std::cin >> val;
				bool rc=this->player1.v_buy_fund(this->vt,symbol,val);
				if (rc == true) { std::cout << "成功" << std::endl; }
				else { std::cout << "失败" << std::endl; }
				break;
			}
			case 3: { 
				std::string symbol;
				float ratio;
				std::cout << "输入代码:";
				std::cin >> symbol;
				std::cout << "输入卖出比例:";
				std::cin >> ratio;
				bool rc = this->player1.v_sell_fund(this->vt, symbol, ratio);
				if (rc == true) { std::cout << "成功" << std::endl; }
				else { std::cout << "失败" << std::endl; }
				break; 
			}
			case 4: { 
				this->vt.v_export_logs();
				break;
			}
			case 5: { std::cout << "余额为:" << this->player1.ac_val << std::endl; break; }
			case 6: { this->vt.v_export_logs(); return; }
			case 7: { std::cout << "现在是:" << core::getnow_string() << std::endl; break; }
			case 8: { this->v_jump_next_trading_day(1); break; }
			case 9: {
				int n;
				std::cout << "输入n:";
				std::cin >> n;
				this->vt.fund_datasource.print_top_code(n);
				break;
			}
			default: break;
			}
		}
	}
	
	void v_app::v_usetime_offset(long long seconds) {
		core::usetime_offset(seconds);
		this->vt.v_check_to_do(this->player1);
		return;
	}

	void v_app::v_jump_next_trading_day(int days) {
		while (days > 0) {
			while (this->vt.check_is_trading_time()) {
				core::usetime_offset(3600);
			}
			while (this->vt.check_is_trading_time() == false) {
				if (core::get_time_offset() > 0) { core::clear_time_offset();return; }
				core::usetime_offset(3600);	
			}
			days--;
		}
		this->vt.v_check_to_do(this->player1);
		return;
	}
}
