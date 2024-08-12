# octopus

A very minimal order book processing UDP server. 

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
 
## Build

``` make ```

## Run

``` ./octopus 1> test/testOutput.csv```

Server is listening on localhost:12345

## Test

``` 
python3 test/test_client.py
```

## Credits

Order book engine based on https://github.com/ajtulloch/quantcup-orderbook with
a double linked list and fast querying.