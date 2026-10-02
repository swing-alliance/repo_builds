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


	void quick_pull_csv_file(const std::vector<std::string>& csv_names);
	int pull_csv_file(std::string csv_name);


}







