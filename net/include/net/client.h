#include<string>
#include<vector>


namespace net {
	class serverconfig {
	public:
		std::string server_host;
		int server_port;
		bool is_configured=false;
		void init_config();
	};

	extern serverconfig g_server_config;

	void quick_push_csv_file(const std::vector<std::string>& csv_names,int threads);
	void quick_pull_csv_file(const std::vector<std::string>& csv_names,int threads);
	int pull_csv_file(std::string csv_name);
	int push_csv_file(std::string csv_name);


	//兼容写法,调用时显式指定路径
	int pull_csv_file_general(std::string csv_name, std::string target_path);
	void quick_pull_csv_file_general(const std::vector<std::string>& csv_names, int threads, std::string target_path);

}







