#include"virtual/v_app.h"
#include<string>
#include <windows.h>
int main(){

	v_sim::v_app app;
	app.init_single_player_fund("2025-4-1 00:01:00");
	return 0;

}

