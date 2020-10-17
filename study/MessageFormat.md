# RFC Message Format 정리

# Request Response 공통

- HTTP 메세지는 octet sequence로 구성되어 있으며, US-ASCII 로 인코딩해야 한다.



# Request 부분

- start-line과 first header filed 사이에 공백(white space)이 있으면 안된다.

### request line
- method SP request-target SP HTTP-version CRLF

request-target
- origin-form(절대경로+?쿼리) - CONNECT와 OPTIONS에서는 사용 x (아래 두 가지 형식으로 나타날 수 있음.)
	- 1) GET http://www.example.org/where?q=now HTTP/1.1
	- 2) GET /where?q=now HTTP/1.1
	     Host: www.example.org
- absolute-form(절대경로-uri(url)) 프록시를 만들 때 사용 + CONNECT와 OPTIONS에서는 사용 x
- authority-form Connect에만 사용됨
	- CONNECT www.example.com:80 HTTP/1.1
- asterisk-form OPTIONS에서만 사용됨 (아래 두 가지 형식으로 나타날 수 있음.)
	- 1) OPTIONS * HTTP/1.1
	- 2) OPTIONS * HTTP/1.1
	     Host: www.example.org:8001

<br>
<br>

* **리퀘스트 라인은 whitespace를 기준으로 split 한다.**
* **스플릿 후 세 가지 요소가 없으면 400 에러를 나타내거나 301 리다이렉트를 한다.**
* 메서드가 미리 지정된 request-line length보다 길면 501(Not Implemented) 에러 전송.
* 리퀘스트 타겟보다 URI가 길면 414(URI Too Long) 에러 전송
* Request-line 길이는 최소 8000 옥텟 권장.


### 헤더필드

```C
/*
** header-field   = field-name ":" OWS field-value OWS
**
** field-name     = token
** field-value    = *( field-content / obs-fold )
** field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ]
** field-vchar    = VCHAR / obs-text
**
** obs-fold       = CRLF 1*( SP / HTAB )
**                ; obsolete line folding
**                ; see Section 3.2.4
*/
```

헤더 필드 네임과 콜론 사이에는 공백이 허용되지 않는다. (콜론 기준으로 스플릿 후 공백 find 후 있으면 400 Bad Request)

프록시/게이트웨이에서 헤더 필드 맨 앞에 공백이 오면 502 Bad Gateway를 보낸 후 탭을 space octets로 대체한다

헤더 필드의 순서는 중요하지 않으나 중복 헤더 검사를 해야 함.








# Response 부분

- start-line과 first header filed 사이에 공백(white space)이 있을 시 전체 라인을 무시한다.

### status-line

### Header Fields

처리하려는 것보다 큰 헤더필드를 수신하면 4xx(client error)를 응답해야 함.