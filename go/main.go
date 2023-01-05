// go build -buildmode=c-archive -o out/go.a
package main

/*
struct Vertex {
    int X;
    int Y;
};
*/
import "C"
import (
	"bufio"
	"net/http"
	"strings"
)

//export Test
/*
func Test() int {
	log.Println("Test")
	return 1
}
struct CustomHTTPRequest {
	char *Method
	char *URL
	char *Host
	char *Body
	char *Header
	int Error
};
*/

//export CustomHTTPRequest
/*
type CustomHTTPRequest struct {
	Method string
	URL    string
	Host   string
	Body   string
	Header string
	Error  bool
}
*/

//export Test
func Test(X, Y C.int) C.struct_Test {
	return C.struct_Test{0, 0}
}

//export ParseHTTPRequest
func ParseHTTPRequest(message string) CustomHTTPRequest {
	reader := strings.NewReader(message)
	req, err := http.ReadRequest(bufio.NewReader(reader))
	if err != nil {
		return CustomHTTPRequest{
			Error: true,
		}
	}
	return CustomHTTPRequest{
		Method: req.Method,
		URL:    req.URL.RawPath,
		Host:   req.Host,
		Body:   "TODO",
		Header: "TODO",
		Error:  false,
	}
}

func main() {}
