#include"virtual/v_account.h"
#include"virtual/v_brokerage.h"
#include"virtual/v_tracker.h"
#include"virtual/v_app.h"
#include"virtual/static_enum.h"
#include<iostream>
#include<model/opt_enum.h>
#include<string>
#include<core/apptime.h>
#include<core/paths.h>

namespace v_sim {
	void v_app::init_single_player_fund(std::string start_time) {
		this->vt.fund_datasource.fast_low_level_load_data(core::funds_row_dir);
		this->v_usetime_offset(core::intf_time(start_time) - core::getnow());
		this->player1.ac_id = "player_1";
		this->player1.ac_val = 10000;
		while (1) {
			int opt_code;
			std::cout << "1-timeoffset,2-buy,3-sell,4-exportlog,5-balance,6-exit,"
				"7-apptime,8-nexttradeday,9-topfundcode,10-rule_sort" << std::endl;
			std::cin >> opt_code;
			switch (opt_code) {
			case 1: {
				long long seconds;
				std::cout << "input offset seconds:";
				std::cin >> seconds;
				this->v_usetime_offset(seconds);
				break;
			}
			case 2: {
				std::string symbol;
				float val;
				std::cout << "input symbol:";
				std::cin >> symbol;
				std::cout << "input buy money:";
				std::cin >> val;
				bool rc = this->player1.v_buy_fund(this->vt, symbol, val);
				if (rc == true) { std::cout << "success" << std::endl; }
				else { std::cout << "fail" << std::endl; }
				break;
			}
			case 3: {
				std::string symbol;
				float ratio;
				std::cout << "input symbol:";
				std::cin >> symbol;
				std::cout << "input sell ratio:";
				std::cin >> ratio;
				bool rc = this->player1.v_sell_fund(this->vt, symbol, ratio);
				if (rc == true) { std::cout << "success" << std::endl; }
				else { std::cout << "fail" << std::endl; }
				break;
			}
			case 4: {
				this->vt.v_export_logs();
				break;
			}
			case 5: { std::cout << "balance:" << this->player1.ac_val << std::endl; break; }
			case 6: { this->vt.v_export_logs(); return; }
			case 7: { std::cout << "now:" << core::getnow_string() << std::endl; break; }
			case 8: { int days; std::cout << "多少天:"; std::cin >> days; this->v_jump_next_trading_day(days); break; }
			case 9: {
				int n;
				std::cout << "input n:";
				std::cin >> n;
				this->vt.fund_datasource.print_top_code(n);
				break;
			}
			case 10: {
				model::sort_options opt;
				std::string field_str;
				std::cout << "输入排序字段 (默认 " << model::to_string(opt.field) << "): ";
				std::cin >> field_str;
				model::sort_field f;
				if (model::from_string(field_str, f)) {
					opt.field = f;
				}
				else {
					std::cout << "非法字段，保持默认: "
						<< model::to_string(opt.field) << "\n";
				}
				std::string order_str;
				std::cout << "输入排序方向 DESC/ASC (默认 "
					<< model::to_string(opt.order) << "): ";
				std::cin >> order_str;
				model::sort_order o;
				if (model::from_string(order_str, o)) {
					opt.order = o;
				}
				else {
					std::cout << "非法方向，保持默认: "
						<< model::to_string(opt.order) << "\n";
				}
				std::string topk_str;
				std::cout << "输入 top_k (-1 不限制，默认 " << opt.top_k << "): ";
				std::cin >> topk_str;
				try {
					opt.top_k = std::stoi(topk_str);
				}
				catch (...) {
					std::cout << "非法数字，保持默认: " << opt.top_k << "\n";
				}
				std::string skip_str;
				std::cout << "跳过无效值 -9999? (0/1，默认 "
					<< opt.skip_invalid << "): ";
				std::cin >> skip_str;
				if (skip_str == "0" || skip_str == "1") {
					opt.skip_invalid = (skip_str == "1");
				}
				else {
					std::cout << "非法输入，保持默认: " << opt.skip_invalid << "\n";
				}
				std::cout << "\n当前配置:\n"
					<< "  field        = " << model::to_string(opt.field) << "\n"
					<< "  order        = " << model::to_string(opt.order) << "\n"
					<< "  top_k        = " << opt.top_k << "\n"
					<< "  skip_invalid = " << (opt.skip_invalid ? "true" : "false") << "\n";
				std::cout << "执行: " << core::getnow_string() << std::endl;
				this->vt.fund_datasource.fast_get_all_cal_results(180,4);
				this->vt.fund_datasource.sort_by_options(opt);
				std::cout << "执行结束: " << core::getnow_string() << std::endl;
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
			bool early_exit = false;
			while (this->vt.check_is_trading_time() == false) {
				if (core::get_time_offset() > 0) {
					core::clear_time_offset();
					early_exit = true;
					break;
				}
				core::usetime_offset(3600);
			}
			if (early_exit) break;
			days--;
		}
		this->vt.v_check_to_do(this->player1);
		return;
	}
}
