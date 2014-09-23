flashpolicyd
============
flash socket policy server

Dependency
==========
* brickred server library(https://github.com/kaienkira/brickred-server)
```
config.sh --prefix=<prefix> --build-unix
make
make install
```

Compile and Install
===================
```
$ make
$ sudo mkdir -p /opt/flashpolicyd
$ sudo cp bin/* /opt/flashpolicyd
$ cd /etc/init.d
$ sudo ln -s /opt/flashpolicyd/flashpolicyd.init flashpolicyd
```
on ubuntu
```
$ sudo update-rc.d flashpolicyd defaults
$ sudo service flashpolicyd start
```
on centos
```
$ sudo chkconfig --add flashpolicyd
$ sudo service flashpolicyd start
```

test
```
$ python -c 'print "<policy-file-request/>%c" % 0' | nc 127.0.0.1 843
```

Licence
-------
Copyright Kaien Chen(kaienkira@gmail.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
