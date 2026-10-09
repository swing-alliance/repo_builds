#include<core/apptime.h>
#include"virtual/v_brokerage.h"
#include<virtual/static_enum.h>
#include<string>
#include<core/apptime.h>
#include"virtual/v_account.h"
#include<vector>
namespace v_sim {
	void v_brokerage::add_account(v_account& v_ac) {
		this->all_accounts.push_back(v_ac);
	}
	

}
