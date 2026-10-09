#pragma once
enum track_evt {
	buy_evt,
	sell_evt
};
enum about_type {
	abt_fund,
	abt_stock,
	transfer,   //专门记录券商向账号回款的
	trade     
};
enum status {
	checked,
	uncheck
};

