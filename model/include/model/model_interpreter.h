#pragma once
#include"fund_model.h"
#include<string>
	//数据到内存对象的转换层
	namespace model_int{



		extern bool mem_to_obj_fund(model::fund_model* target, const std::string* buf);

	}