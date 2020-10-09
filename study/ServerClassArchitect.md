# 서버 클래스 설계하기

서버 클래스에서는 클라이언트의 request가 있는지 확인하고 있다면, 요청을 수신하여 적합한 response를 생성하고 송신하는 역할을 한다.

```C
class Server
{
private:
	struct s_config _config;
	int _server_socket;
	std::vector<int> _client_sockets;
public:
	Server();
	Server(struct ss_config server_config);

	bool init();
	...
}
```

`bool init()`
- read_fd_set, write_fd_set, exception_fd_set 을 만든다.
- socket()
- setsockopt()
- bind
- listen
- fcntl


select()
read()
write()

- request를 감지하는 부분
- request를 수신하는 부분

- response를 생성하는 부분
- response를 송신하는 부분


void getRequest()

bool isValidRequest()
