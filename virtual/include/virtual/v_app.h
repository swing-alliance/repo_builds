#pragma once
#include"v_account.h"
#include"v_brokerage.h"
#include"v_tracker.h"
#include<string>
namespace v_sim {
	class v_app {
	private:
		v_account player1;
		v_account player2;
		v_global_tracker vt;
		v_brokerage vbkrg;
	public:
		void init_single_player_fund(std::string start_time);       //fund单机玩家
		void v_usetime_offset(long long seconds);      //在原有的偏移时间后加上事务检查
	};

}


