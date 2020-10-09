# 서버매니저 클래스 설계하기

ServerManager 클래스를 만들어 Config 파일을 읽고, Config 객체와 Server 객체를 만든 이후 실행과 종료를 관리하도록 한다.

```
class ServerManager
{
	private:
		std::string _config_file_path;
		std::vector<Server *> _servers;
	public:
		ServerManager()
		ServerManager(char *config_path);
		void initServers()
		void makeServer(struct s_config);
		bool runServers()
		void exitServers()
}
```

ServerManager가 init과 run, exit을 관리하는 이유는 이번 과제에서는 multiple server를 구현해야 하기 때문이다. 여러 서버가 서로다른 config들을 가지고 있을 가능성을 대비하여, 각각의 conf 파일에 해당하는 서버를 관리하기 위하여 만들어진 클래스이다.

이 때 `initServers()` 에서는 ConfigManager() instance를 생성하고 conf 파일을 parsing 하여 `makeServer(struct s_config)` 함수를 호출한다. 이 때 인자로 들어가는 s_config 구조체에 맞는 conf 파일을 토대로 서버를 생성한다.

`runServers()` 에서는 multiple servers 을 실행시킨다. 서버의 health check 이후 정상적이지 않은 종료가 일어났을 때를 위하여 bool 타입으로 선언한다.

