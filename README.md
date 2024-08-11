# octopus

A very minimal order book processing UDP server. 

> I found this old repo sitting in my inbox and I thought I'd publish it. It's a mock project I built in a couple of long hours for an interview at an octopus company. They never got back to me, so I assume it was terrible software, or I pissed them off somehow.

## Prerequisites 

```
❯  uname -r
5.4.72-microsoft-standard-WSL2

❯ g++ --version
g++ (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0

❯ make --version
GNU Make 4.2.1
Built for x86_64-pc-linux-gnu

❯ dpkg -s libboost-dev | grep 'Version'
Version: 1.71.0.0ubuntu2

```
Make sure you have Boost library installed, or the make will fail: `brew install boost`

## Build

``` make ```

## Run

``` ./octopus 2> traces.log ```

Server is listening on localhost:12345

## Test

``` 
mv inputFile.csv test/
python3 test/client.py
```

## Credits

Order book engine based on https://github.com/ajtulloch/quantcup-orderbook with
a double linked list and fast querying.