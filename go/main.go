//go:build cgo
// +build cgo

// go build -buildmode=c-archive -ldflags=-linkmode=external -o out/go.a
package main

/*
//go:cgo_ldflag "-framework"
//go:cgo_ldflag "CoreFoundation"
//go:cgo_ldflag "-framework"
//go:cgo_ldflag "Security"
struct CustomHTTPRequest {
	char *Method;
	char *Path;
	char *Host;
	int Port;
	char *Body;
	char *Header;
	int Error;
};
*/
import "C"
import (
	"bufio"
	"net"
	"net/http"
	"strconv"
	"strings"
)

/*
//export Test
func Test() int {
	log.Println("Test")
	return 1
}
*/

//export CGOParseHTTPRequest
func CGOParseHTTPRequest(message *C.char) C.struct_CustomHTTPRequest {
	m := C.GoString(message)
	reader := strings.NewReader(m)
	req, err := http.ReadRequest(bufio.NewReader(reader))
	if err != nil {
		return C.struct_CustomHTTPRequest{
			Error: 1,
		}
	}
	host, port, _ := net.SplitHostPort(req.Host)
	if err != nil {
		host = ""
		port = "80"
	}
	port_int, err := strconv.Atoi(port)
	if err != nil {
		port_int = 80
	}
	return C.struct_CustomHTTPRequest{
		Method: C.CString(req.Method),
		Path:   C.CString(req.URL.Path),
		Host:   C.CString(host),
		Port:   C.int(port_int),
		Body:   C.CString("TODO"),
		Header: C.CString("TODO"),
		Error:  C.int(0),
	}
}

func main() {}
