# shawdowsock-libevent with C++

## 编译方法

```shell
cmake CMakeList.txt
make
```

## 编译要求

* [cmake >= 3.5](https://cmake.org/)
* [libevent >= 2.1.8](http://libevent.org/)
* pthread

## 运行方法

```shell
./ss-server
```

```shell
proxychains curl www.nowcoder.com
```

## 运行软件要求

* proxychains
* curl

## 软件配置

```
## /etc/proxychains.conf
socks5 	127.0.0.1 1080
```

