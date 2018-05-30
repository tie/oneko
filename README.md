# oneko

Neko runs over the windows

## Getting Started

### Prerequisites

- C standard library (e.g. glibc)
- X11 client-side library (libX11)
- X11 miscellaneous extensions library (libXext)

### Building

Build system is not used because there is only one source file that includes all headers. The command below produces `oneko` executable on the majority of distributions.

```sh
clang -Wno-parentheses -std=c11 -pedantic -lc -lm -lX11 -lXext -D_DEFAULT_SOURCE oneko.c -o oneko
```

The same arguments will work with GCC compiler.

### Usage

Read the [docs](doc).

## Versioning

Patch levels are used for versions compatible with the original oneko. The project will switch to [SemVer](https://semver.org/) for versioning after the first feature-breaking change.

## Authors

* Original program "xneko" written by Masayuki Koba
* Modified to program "oneko" by Tatsuya Kato
* Modified furthermore by
  - John Lerchey
  - Eric Anderson
  - Toshihiro Kanda
  - Kiichiroh Mukose

## License

According to [FSF Directory][1], the (Japanese) README file, as well as the LSM file originally distributed with oneko on sunsite both say that oneko is public domain software.

[1]: https://directory.fsf.org/wiki/Oneko#tab=Details
