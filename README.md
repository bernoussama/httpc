<a id="readme-top"></a>

<div align="center">

[![MIT License][license-shield]][license-url]
[![CodeFactor](https://www.codefactor.io/repository/github/bernoussama/httpc/badge)](https://www.codefactor.io/repository/github/bernoussama/httpc)
</div>
<br />

<div align="center">
 <img src="./httpc-logo.jpg" width="200" height="200">
<h1 align="center">HTTPC</h1>
</div>

[![progress-banner](https://backend.codecrafters.io/progress/http-server/d6024b54-aeef-4707-9128-f99b7d5d5df8)](https://app.codecrafters.io/users/codecrafters-bot?r=2qF)

## About

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

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->
## License

Distributed under the MIT License. See [LICENSE](https://github.com/bernoussama/httpc/blob/master/licence.txt) for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- CONTACT -->
## Contact

Oussama Bernou - [@bernoussama](https://x.com/bernoussama) - contact@bernoussama.com

<!-- Project Link: [https://github.com/your_username/repo_name](https://github.com/your_username/repo_name) -->


<p align="right">(<a href="#readme-top">back to top</a>)</p>


### Built With

 [![C][C-shield]][C-url]


## Resources

- [HTTP request syntax](https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html)

- [Threading in C](https://www.geeksforgeeks.org/multithreading-in-c)

- The challenge instructions were also a good help.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[license-shield]: https://img.shields.io/github/license/bernoussama/httpc
[license-url]: https://github.com/bernoussama/httpc/blob/master/LICENSE.txt
[C-shield]: https://img.shields.io/badge/-0769AD?style=for-the-badge&logo=C&logoColor=white
[C-url]: https://en.wikipedia.org/wiki/C_(programming_language)
