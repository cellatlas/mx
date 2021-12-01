# MX

To build `mx` run the following commands:

```
$ git clone https://github.com/sbooeshaghi/mx.git
$ cd mx; mkdir build; cd build
$ g++ -std=c++11 ../src/**/*.cpp -o mx && cp mx /usr/local/bin
```

```
Usage: mx <cmd> [arguments] ...

where <cmd> can be one of:

text            Convert .mx to .mtx
fromtext        Convert .mtx to .mx
shape           Print shape of matrix
extract         Extract elements
sum             Sum elements
sort            Sort elements

Running mx <cmd> without arguments prints usage information for <cmd>
```
