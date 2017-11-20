# Zest

Zest is a C++ library inspired by Micrsoft C++ library and Facebook/Folly. Most of code, in this library, is not for best performance but for easiest usage in small-medium size project.

## Story

Actually, there is no story. I'm just feeling Boost/XXX is not easy to use and standard library has some caveats in its implementation.
To a degree, Boost has solved most of problems, but I don't like the interface design and code style of boost.

## TODO

### 1. TestFramwork

I plan to use [GoogleTest](https://github.com/google/googletest). Currently, I will write a main function to test the features.

### 2. Libs

* Executor
  * SequentialExecutor
  * ConcurrentExecutor
  * ThreadPool
* Future
* Promise
* Optional
* Maybe
* Json
* Url
* String
* Lock
* Atomic smart pointer
  * AtomicSharedPtr
  * AtomicUniquePtr
  * AtomicWeakPtr
* Reflection
  * Deserialize/Serialize
* Statistics
  * \*WaitTODO\*
* Signal Processing(Fourier Transform?)
  * \*WaitTODO\*

### 3. Network

* IP V4,V6
* TCP/UDP
* Server/Client
* SSL/Security
* RPC

### 4. Server Framework

* Router
* MiddleWare like request handler workflow
* Repository pattern DB access
* Service Oriented Computing

### 5. Distributed Computing Framework

* Hadoop like?