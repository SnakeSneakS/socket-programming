//go:build cgo
// +build cgo

// go build -buildmode=c-archive -ldflags=-linkmode=external -o out/go.a
package main

/*
//go:cgo_ldflag "-framework"
//go:cgo_ldflag "CoreFoundation"
struct CustomHTTPRequest {
	char *Method;
	char *Path;
	char *Host;
	char *Body;
	char *Header;
	int Error;
};
*/
import "C"
import (
	"bufio"
	"net/http"
	"strings"
)

/*
//export Test
func Test() int {
	log.Println("Test")
	return 1
}
*/

//export ParseHTTPRequest
func ParseHTTPRequest(message *C.char) C.struct_CustomHTTPRequest {
	m := C.GoString(message)
	reader := strings.NewReader(m)
	req, err := http.ReadRequest(bufio.NewReader(reader))
	if err != nil {
		return C.struct_CustomHTTPRequest{
			Error: 1,
		}
	}
	return C.struct_CustomHTTPRequest{
		Method: C.CString(req.Method),
		Path:   C.CString(req.URL.Path),
		Host:   C.CString(req.Host),
		Body:   C.CString("TODO"),
		Header: C.CString("TODO"),
		Error:  C.int(0),
	}
}

func main() {}
