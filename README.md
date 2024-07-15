[![MIT License][license-shield]][license-url]

<br />

<div align="center">
<h3 align="center">HTTPC</h3>
</div>

[![progress-banner](https://backend.codecrafters.io/progress/http-server/d6024b54-aeef-4707-9128-f99b7d5d5df8)](https://app.codecrafters.io/users/codecrafters-bot?r=2qF)

[HTTP](https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol) is the
protocol that powers the web. In this ["Build Your Own HTTP server" Challenge](https://app.codecrafters.io/courses/http-server/overview), I built a multithreaded HTTP/1.1 server in C that is capable of serving multiple clients.

check out CodeCrafters programming challenges and get a free week using my [referral link](https://app.codecrafters.io/r/cute-gorilla-362426). I will get one too.

## Getting Started

The entry point for the HTTP server implementation is in `app/server.c`.

```sh
# compile the server
gcc -lz -lcurl  app/*.c -o server
# start the server
./server --directory /tmp/
```

<!-- ROADMAP -->

## Roadmap

- [x] Bind to a port
- [x] Respond with 200
- [x] Respond with body
- [x] Read header
- [x] Concurrent connections
- [x] Return a file
- [x] Read request body
- [ ] HTTP compression
  - [ ] Compression headers
  - [ ] Multiple Compression schemes
  - [ ] Gzip compression

## Resources

[HTTP request syntax](https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html)

[Threading in C](https://www.geeksforgeeks.org/multithreading-in-c)

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

[license-shield]: https://img.shields.io/github/license/0ussamaBernou/httpc
[license-url]: https://github.com/0ussamaBernou/httpc/blob/master/LICENSE.txt
