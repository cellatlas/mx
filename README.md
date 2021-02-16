# MX

To build `mx` run the following commands:
```
$ git clone https://github.com/sbooeshaghi/mx.git
$ cd mx; mkdir build; cd build
$ g++ -std=c++11 -c ../src/*.cpp; g++ -std=c++11 -o mx *.o; cp mx /usr/local/bin
```

```
Usage: mx <CMD> [arguments] ...

Where <CMD> can be one of: 

view              View matrix
shape             Print shape of matrix
sort              Sort matrix
sum               Sum elements across axis
extract           Extract elements across axis
sample            Sample elements across axis

Running mx <CMD> without arguments prints usage information for <CMD>
```
