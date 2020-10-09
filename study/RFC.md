# RFC 7230 ~ 7235

모든 RFC를 읽는 것은 너무 어려운 일인 것 같다. 일단 너무 너무 너무너무 재미가 없고 비효율적이다.

전체적으로 훑으면서 읽다가 중요한 것 같은 내용만 발췌해서 공부해보기 위해 만들게 된 문서다.

우선 7230 부터 읽으면서, 중요 부분은 본문을 가져오고 해석한다. 이 때 `반드시` 어떤 단원의 소제목으로 포함되었는지 꼭 명시하도록 하자.

* 내가 실수로 7230의 URI 부분까지는 날려버렸기 때문에... 7230 3. Message Format 부분부터 시작한다.
* 추후에 빠져있던 내용을 추가하는 경우 맨 아래 별도 부록으로 정리하도록 한다.

## RFC 7230 Message Syntax and Routing

### HTTP Message Format

All HTTP/1.1 messages consist of a start-line followed by a sequence of octets in a format similar to the Internet Message Format [RFC5322]: zero or more header fields (collectively referred to as the "headers" or the "header section"), an empty line indicating the end of the header section, and an optional message body.

- 모든 메세지에는 start-line이 포함되어야 하고, 일련의 octet들로 구성된다. 이 때 헤더필드에는 끝을 나타내는 빈 라인이 포함되어야 한다.

The normal procedure for parsing an HTTP message is to read the start-line into a structure, read each header field into a hash table by field name until the empty line, and then use the parsed data to determine if a message body is expected.  If a message body has been indicated, then it is read as a stream until an amount of octets equal to the message body length is read or the connection is closed.

- 메세지 분석 방법에 대한 내용이다. 우선 스타트 라인부터 읽은 뒤에 헤더필드부터 빈 줄까지 헤더 이름으로 된 해시 테이블을 구성한 뒤, 메세지 본문이 필요한지 확인해라. 본문이 표시된 경우에 본문 길이와 동일한 octet을 읽거나 통신이 끝날 때까지 stream 으로 읽어간다.

A recipient MUST parse an HTTP message as a sequence of octets in an encoding that is a superset of US-ASCII [USASCII].  Parsing an HTTP message as a stream of Unicode characters, without regard for the specific encoding, creates security vulnerabilities due to the varying ways that string processing libraries handle invalid multibyte character sequences that contain the octet LF (%x0A). String-based parsers can only be safely used within protocol elements after the element has been extracted from the message, such as within a header field-value after message parsing has delineated the individual fields.

- 반드시 US-ASCII 상위 집합인 인코딩에서 octet으로 구문을 분석해야 한다! (MUST)
- 인코딩에 관계없이 유니코드 문자로 분석할 경우 octet LF(%x0A) 등의 유효하지 않은 다중 바이트 문자 시퀀스를 처리할 때 보안 문제가 생긴다고 한다. 따라서 파싱할 때 헤더의 개별 필드를 분석한 이후 field-value와 같이 메세지 요소를 추출한 뒤에 사용하도록 하자.

A sender MUST NOT send whitespace between the start-line and the first header field.  A recipient that receives whitespace between the start-line and the first header field MUST either reject the message as invalid or consume each whitespace-preceded line without further processing of it (i.e., ignore the entire line, along with any subsequent lines preceded by whitespace, until a properly formed header field is received or the header section is terminated).

- 발신자는 start-line과 첫 번째 헤더 사이에 공백을 포함시켜서는 안 된다. (MUST NOT)
- 공백이 포함될 경우 메세지를 유효하지 않은 것으로 판단하고 메세지를 처리해서는 안된다. (메세지의 종료를 알리는 빈 라인까지의 내용을 전체 무시하도록 하는 등 예외처리를 해야할 것 같다.)

> 지금까지의 내용요약
>> HTTP로 전달되는 메세지에는 start-line과 header 필드가 포함되어 있어야 하며 끝을 알리는 뉴라인이 포함되어야 한다. 이 때 스타트라인과 첫 번째 헤더필드 사이에는 공백이 있어서는 안되고, 공백이 있을 경우 반드시 예외처리를 해야 한다. 또한 메세지를 분석할 때는 유니코드가 아닌 US-ASCII 인코딩을 사용하고, octet으로 구문분석을 해야 한다.

---

### 3.1 Start Line

뭐 딱히 중요한 내용은 없는 것 같다.

In theory, a client could receive requests and a server could receive responses, distinguishing them by their different start-line formats, but, in practice, servers are implemented to only expect a request (a response is interpreted as an unknown or invalid request method) and clients are implemented to only expect a response.

- 이론적으로 HTTP 메세지는 클라이언트->서버 요청 혹은 서버->클라이언트 응답 두 경우에서 발생한다. 하지만 실제로는 서버가 요청만 예상하도록 구현하고, 클라이언트는 응답만 예상하도록 구현해야 한다.

- start-line = request-line / status-line

### 3.1.1 Request Line

A request-line begins with a method token, followed by a single space (SP), the request-target, another single space (SP), the protocol version, and ends with CRLF.

- request-line = method(token) SP request-target SP HTTP-version CRLF
- SP: space, CRLF: carriage return line feed(\r\n)

Recipients typically parse the request-line into its component parts by splitting on whitespace (see Section 3.5), since no whitespace is allowed in the three components.  Unfortunately, some user agents fail to properly encode or exclude whitespace found in hypertext references, resulting in those disallowed characters being sent in a request-target.

- request-line 에는 공백이 있으면 안된다.

Recipients of an invalid request-line SHOULD respond with either a 400 (Bad Request) error or a 301 (Moved Permanently) redirect with the request-target properly encoded.  A recipient SHOULD NOT attempt to autocorrect and then process the request without a redirect, since the invalid request-line might be deliberately crafted to bypass security filters along the request chain.

- 유효하지 않은 request-line의 수신자는 400(Bad Request) 오류 또는 301(Moved Permanently) 리다이렉트로 응답해야 한다.
- 유효하지 않은 request-line이 들어왔을 때 수신자는 리다이렉트 없이 요청을 수정하고 처리해서는 안된다.

HTTP does not place a predefined limit on the length of a request-line, as described in Section 2.5.  A server that receives a method longer than any that it implements SHOULD respond with a 501 (Not Implemented) status code.  A server that receives a request-target longer than any URI it wishes to parse MUST respond with a 414 (URI Too Long) status code (see Section 6.5.12 of [RFC7231]).

- request-line의 길이에 제한은 없다. 하지만 서버가 실행하려는 것보다 긴 메서드를 받았을 경우 501(Not Implemented) 코드로 응답해야 한다.
- 서버가 URI보다 긴 request-target을 받았을 경우 414(URI Too Long) 코드를 응답해야 한다.

> 내용요약
>> Request Line은 method, target, HTTP-version으로 구성된다.
>>
>> request-line에는 공백이 있으면 안되고, 유효하지 않은 라인이 왔을 때 400 또는 301 코드를 응답해야 한다. 또한 line의 길이에는 제한이 없지만 서버에서 처리할 수 없는 길이가 들어왔을 경우 501, URI보다 긴 target이 요청되었을 경우 414 코드를 응답해야 한다.

---

### 3.1.2 Status Line

The first line of a response message is the status-line, consisting of the protocol version, a space (SP), the status code, another space, a possibly empty textual phrase describing the status code, and ending with CRLF.

- status-line = HTTP-version SP Status-code SP reason-phrase CRLF

The status-code element is a 3-digit integer code describing the result of the server's attempt to understand and satisfy the client’s corresponding request.

- status-code는 세 자리의 정수이다.

The reason-phrase element exists for the sole purpose of providing a textual description associated with the numeric status code, mostly out of deference to earlier Internet application protocols that were more frequently used with interactive text clients.  A client SHOULD ignore the reason-phrase content.

- reason-phrase는 상태코드와 관련된 설명을 제공하며, 클라이언트는 이 내용을 무시해야한다.
- reason-phrase  = *( HTAB / SP / VCHAR / obs-text )

---

### 3.2 Header Fields

Each header field consists of a case-insensitive field name followed by a colon (":"), optional leading

- 각 헤더 필드는 대소문자를 구분하지 않고, 필드 이름 뒤에 콜론이 들어가며 선택적으로 공백이 들어간다.

```
header-field   = field-name ":" OWS field-value OWS

field-name     = token
field-value    = *( field-content / obs-fold )
field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ]
field-vchar    = VCHAR / obs-text

obs-fold       = CRLF 1*( SP / HTAB )
               ; obsolete line folding
               ; see Section 3.2.4
```

The field-name token labels the corresponding field-value as having the semantics defined by that header field.  For example, the Date header field is defined in Section 7.1.1.2 of [RFC7231] as containing the origination timestamp for the message in which it appears.

- field-name 은 field-value를 바탕으로 레이블링한다.
- ex) Data header는 [RFC7231 7.1.1.2]의 내용을 바탕으로 타임스탬프를 포함해야 한다.

---

### 3.2.1 Field Extensibility

A proxy MUST forward unrecognized header fields unless the field-name is listed in the Connection header field (Section 6.1) or the proxy is specifically configured to block, or otherwise transform, such fields.  Other recipients SHOULD ignore unrecognized header fields. These requirements allow HTTP's functionality to be enhanced without requiring prior update of deployed intermediaries.

- field-name이 헤더 필드에 나열되어 있지 않거나 프록시가 이 필드를 차단하지 못하는 경우 인식하지 못한 헤더 필드를 전달해야 한다.
- 다른 수신자는 인식할 수 없는 헤더 필드를 무시해야 한다.

All defined header fields ought to be registered with IANA in the "Message Headers" registry, as described in Section 8.3 of [RFC7231].

- 정의된 모든 헤더 필드는 Message Headers 레지스트리의 IANA에 등록되어야 한다.

---

### 3.2.2 Field Order

The order in which header fields with differing field names are received is not significant.  However, it is good practice to send header fields that contain control data first, such as Host on requests and Date on responses, so that implementations can decide when not to handle a message as early as possible.  A server MUST NOT apply a request to the target resource until the entire request header section is received, since later header fields might include conditionals, authentication credentials, or deliberately misleading duplicate header fields that would impact request processing. A sender MUST NOT generate multiple header fields with the same field name in a message unless either the entire field value for that header field is defined as a comma-separated list [i.e., #(values)] or the header field is a well-known exception (as noted below).

- 헤더 필드와 필드 이름중에 무엇을 먼저 수신해야 되는 지에 대한 순서는 중요하지 않다.
- Host, Data 등 제어 데이터가 포함된 경우, 메세지를 처리 하지 않을 시점을 결정하는 것을 권장한다. (? 이게 무슨말일까)
- 서버는 요청 헤더 부분을 받을 때 헤더가 중복될 수 있으므로 헤더를 온전히 수신하기 전에 리소스에 요청을 해서는 안된다.
- 발신자는 메세지에 동일한 필드 이름을 여러개 생성해서는 안된다.

A recipient MAY combine multiple header fields with the same field name into one "field-name: field-value" pair, without changing the semantics of the message, by appending each subsequent field value to the combined field value in order, separated by a comma.  The order in which header fields with the same field name are received is therefore significant to the interpretation of the combined field value; a proxy MUST NOT change the order of these field values when forwarding a message.

- 동일한 필드 이름을 가지는 대신 value를 다르게 할 수도 있다. 하지만 이럴 경우 순서를 신경쓰자. 순서를 변경해서는 안된다.

### 3.2.3 Whitespace

This specification uses three rules to denote the use of linear whitespace: OWS (optional whitespace), RWS (required whitespace), and BWS ("bad" whitespace).

- 공백을 처리할 때는 OWS, RWD, BWS 규칙을 지켜야 한다.

```
OWS            = *( SP / HTAB )
               ; optional whitespace
RWS            = 1*( SP / HTAB )
               ; required whitespace
BWS            = OWS
               ; "bad" whitespace
```

---

### 3.2.4 Field Parsing

No whitespace is allowed between the header field-name and colon.  In the past, differences in the handling of such whitespace have led to security vulnerabilities in request routing and response handling.  A server MUST reject any received request message that contains whitespace between a header field-name and colon with a response code of 400 (Bad Request).  A proxy MUST remove any such whitespace from a response message before forwarding the message downstream.

- 헤더의 field-name과 콜론 사이에 공백이 있으면 안된다. 공백이 있다면 응답코드가 400(Bad Request) 코드로 요청 메세지를 거부하도록 하자.
- 프록시는 메세지를 다운 스트림으로 전달하기 전에 이러한 공백을 모두 제거하도록 한다.

A field value might be preceded and/or followed by optional whitespace (OWS); a single SP preceding the field-value is preferred for consistent readability by humans.  The field value does not include any leading or trailing whitespace: OWS occurring before the first non-whitespace octet of the field value or after the last non-whitespace octet of the field value ought to be excluded by parsers when extracting the field value from a header field.

- 파싱할 때 필드 값에는 선후 공백이 없는지 꼭 확인하도록 하자. 또 octet 전후에 OWS가 있다면 지워주도록 하자.

Historically, HTTP header field values could be extended over multiple lines by preceding each extra line with at least one space or horizontal tab (obs-fold).  This specification deprecates such line folding except within the message/http media type (Section 8.3.1).  A sender MUST NOT generate a message that includes line folding (i.e., that has any field-value that contains a match to the obs-fold rule) unless the message is intended for packaging within the message/http media type. A server that receives an obs-fold in a request message that is not within a message/http container MUST either reject the message by sending a 400 (Bad Request), preferably with a representation explaining that obsolete line folding is unacceptable, or replace each received obs-fold with one or more SP octets prior to interpreting the field value or forwarding the message downstream.

- HTTP 헤더 필드 값은 여러 줄로 써도 된다. 하지만 발신자는 message/http media type 내에서 패키징 되지 않은 라인 폴딩(obs-fold와 일치하는 field value)을 포함해서는 안된다. (? 이것도 무슨 말일까. 읽다보면 알겠지..?)
- message/http 컨테이너에서 obs-fold가 발견되었다면 400(Bad Request)를 응답해야 한다.
	- 사용되지 않는 라인 폴딩을 쓰지마라. 필드값을 해석하거나 다운스트림으로 전달하기 전에 obs-fold를 하나 이상의 space 등으로 대체하자.

A proxy or gateway that receives an obs-fold in a response message that is not within a message/http container MUST either discard the message and replace it with a 502 (Bad Gateway) response, preferably with a representation explaining that unacceptable line folding was received, or replace each received obs-fold with one or more SP octets prior to interpreting the field value or forwarding the message downstream.

- message/http 컨테이너 내에 응답메세지가 없다면 프록시나 게이트웨이는 메세지를 삭제하고 502(Bad Gateway)를 응답해야 한다.

> 내용정리
>> 헤더의 필드이름과 콜론 사이에 공백을 두지 마라.
>>
>> 프록시는 메세지를 다운스트림으로 보내기 전에 컨테이너 내에 obs-fold가 있는지 확인하고, octet에 OWS가 있는지 확인해라. 있다면 다 지워라.
>>
>> 위에 것들이 있었을 때 400(Bad Request), 502(Bad Gateway)를 적절하게 응답해라.

---

### 3.2.5 Field Limits

별 거 없어 보인다. 패스

---

### 3.2.6 Field Value Components

```
token          = 1*tchar

tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
               / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
               / DIGIT / ALPHA
               ; any VCHAR, except delimiters

A string of text is parsed as a single value if it is quoted using
double-quote marks.

quoted-string  = DQUOTE *( qdtext / quoted-pair ) DQUOTE
qdtext         = HTAB / SP /%x21 / %x23-5B / %x5D-7E / obs-text
obs-text       = %x80-FF

Comments can be included in some HTTP header fields by surrounding
the comment text with parentheses.  Comments are only allowed in
fields containing "comment" as part of their field value definition.

comment        = "(" *( ctext / quoted-pair / comment ) ")"
ctext          = HTAB / SP / %x21-27 / %x2A-5B / %x5D-7E / obs-text

The backslash octet ("\") can be used as a single-octet quoting
mechanism within quoted-string and comment constructs.  Recipients
that process the value of a quoted-string MUST handle a quoted-pair
as if it were replaced by the octet following the backslash.

quoted-pair    = "\" ( HTAB / SP / VCHAR / obs-text )
```

- 필드 값은 위의 규칙을 따르도록 하자.

---

### 3.3 Message Body

```
message-body = *OCTET
```

- Message-Body는 옥텟으로 구성시키자.

The presence of a message body in a response depends on both the request method to which it is responding and the response status code (Section 3.1.2).  Responses to the HEAD request method (Section 4.3.2 of [RFC7231]) never include a message body because the associated response header fields (e.g., Transfer-Encoding, Content-Length, etc.), if present, indicate only what their values would have been if the request method had been GET (Section 4.3.1 of [RFC7231]). 2xx (Successful) responses to a CONNECT request method (Section 4.3.6 of [RFC7231]) switch to tunnel mode instead of having a message body. All 1xx (Informational), 204 (No Content), and 304 (Not Modified) responses do not include a message body.  All other responses do include a message body, although the body might be of zero length.

### 3.3.1 Transfer-Encoding

```
Transfer-Encoding = 1#transfer-coding
```

A recipient MUST be able to parse the chunked transfer coding (Section 4.1) because it plays a crucial role in framing messages when the payload body size is not known in advance.  A sender MUST NOT apply chunked more than once to a message body (i.e., chunking an already chunked message is not allowed).  If any transfer coding other than chunked is applied to a request payload body, the sender MUST apply chunked as the final transfer coding to ensure that the message is properly framed.  If any transfer coding other than chunked is applied to a response payload body, the sender MUST either apply chunked as the final transfer coding or terminate the message by closing the connection.

```
Transfer-Encoding: gzip, chunked
```

Transfer-Encoding MAY be sent in a response to a HEAD request or in a 304 (Not Modified) response (Section 4.1 of [RFC7232]) to a GET request, neither of which includes a message body, to indicate that the origin server would have applied a transfer coding to the message body if the request had been an unconditional GET.  This indication is not required, however, because any recipient on the response chain (including the origin server) can remove transfer codings when they are not needed.

A server MUST NOT send a Transfer-Encoding header field in any response with a status code of 1xx (Informational) or 204 (No Content).  A server MUST NOT send a Transfer-Encoding header field in any 2xx (Successful) response to a CONNECT request (Section 4.3.6 of [RFC7231]).

A server that receives a request message with a transfer coding it does not understand SHOULD respond with 501 (Not Implemented).

---

### 3.3.2 Content-Length

When a message does not have a Transfer-Encoding header field, a Content-Length header field can provide the anticipated size, as a decimal number of octets, for a potential payload body.  For messages that do include a payload body, the Content-Length field-value provides the framing information necessary for determining where the body (and message) ends.  For messages that do not include a payload body, the Content-Length indicates the size of the selected representation (Section 3 of [RFC7231]).

```
Content-Length = 1 * DIGIT
```

A sender MUST NOT send a Content-Length header field in any message that contains a Transfer-Encoding header field.

A user agent SHOULD send a Content-Length in a request message when no Transfer-Encoding is sent and the request method defines a meaning for an enclosed payload body.  For example, a Content-Length header field is normally sent in a POST request even when the value is 0 (indicating an empty payload body).  A user agent SHOULD NOT send a Content-Length header field when the request message does not contain a payload body and the method semantics do not anticipate such a body.

A server MAY send a Content-Length header field in a response to a HEAD request (Section 4.3.2 of [RFC7231]); a server MUST NOT send Content-Length in such a response unless its field-value equals the decimal number of octets that would have been sent in the payload body of a response if the same request had used the GET method.

A server MAY send a Content-Length header field in a 304 (Not Modified) response to a conditional GET request (Section 4.1 of [RFC7232]); a server MUST NOT send Content-Length in such a response unless its field-value equals the decimal number of octets that would have been sent in the payload body of a 200 (OK) response to the same request.

A server MUST NOT send a Content-Length header field in any response with a status code of 1xx (Informational) or 204 (No Content).  A server MUST NOT send a Content-Length header field in any 2xx (Successful) response to a CONNECT request (Section 4.3.6 of    [RFC7231]).

Aside from the cases defined above, in the absence of Transfer-Encoding, an origin server SHOULD send a Content-Length header field when the payload body size is known prior to sending the complete header section.  This will allow downstream recipients to measure transfer progress, know when a received message is complete, and potentially reuse the connection for additional requests.

Any Content-Length field value greater than or equal to zero is valid.  Since there is no predefined limit to the length of a payload, a recipient MUST anticipate potentially large decimal numerals and prevent parsing errors due to integer conversion overflows (Section 9.3).

If a message is received that has multiple Content-Length header fields with field-values consisting of the same decimal value, or a single Content-Length header field with a field value containing a list of identical decimal values (e.g., "Content-Length: 42, 42”), indicating that duplicate Content-Length header fields have been generated or combined by an upstream message processor, then the recipient MUST either reject the message as invalid or replace the duplicated field-values with a single valid Content-Length field containing that decimal value prior to determining the message body length or forwarding the message.

### 3.3.3 Message body length

1.  Any response to a HEAD request and any response with a 1xx
(Informational), 204 (No Content), or 304 (Not Modified) status
code is always terminated by the first empty line after the
header fields, regardless of the header fields present in the
message, and thus cannot contain a message body.

2.  Any 2xx (Successful) response to a CONNECT request implies that the connection will become a tunnel immediately after the empty line that concludes the header fields.  A client MUST ignore any Content-Length or Transfer-Encoding header fields received in such a message.

3.  If a Transfer-Encoding header field is present and the chunked transfer coding (Section 4.1) is the final encoding, the message body length is determined by reading and decoding the chunked data until the transfer coding indicates the data is complete.
If a Transfer-Encoding header field is present in a response and the chunked transfer coding is not the final encoding, the message body length is determined by reading the connection until it is closed by the server.  If a Transfer-Encoding header field is present in a request and the chunked transfer coding is not the final encoding, the message body length cannot be determined reliably; the server MUST respond with the 400 (Bad Request) status code and then close the connection. If a message is received with both a Transfer-Encoding and a Content-Length header field, the Transfer-Encoding overrides the Content-Length.  Such a message might indicate an attempt to perform request smuggling (Section 9.5) or response splitting (Section 9.4) and ought to be handled as an error.  A sender MUST remove the received Content-Length field prior to forwarding such a message downstream.

4.  If a message is received without Transfer-Encoding and with either multiple Content-Length header fields having differing field-values or a single Content-Length header field having an invalid value, then the message framing is invalid and the recipient MUST treat it as an unrecoverable error.  If this is a request message, the server MUST respond with a 400 (Bad Request) status code and then close the connection.  If this is a response message received by a proxy, the proxy MUST close the connection to the server, discard the received response, and send a 502 (Bad Gateway) response to the client.  If this is a response message received by a user agent, the user agent MUST close the connection to the server and discard the received response.

5.  If a valid Content-Length header field is present without Transfer-Encoding, its decimal value defines the expected message body length in octets.  If the sender closes the connection or the recipient times out before the indicated number of octets are received, the recipient MUST consider the message to be incomplete and close the connection.

6.  If this is a request message and none of the above are true, then the message body length is zero (no message body is present).

7.  Otherwise, this is a response message without a declared message body length, so the message body length is determined by the number of octets received prior to the server closing the connection.