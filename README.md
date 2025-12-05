Как прогнать тесты

```bash
token-ring$ mkdir build && cd build
build$ cmake ..
build$ make test_token_ring
build$ ./token_ring/test_token_ring
```

Если поменяли код/закомментировали тесты, то нужно пересобрать
```bash
build$ make test_token_ring
build$ ./token_ring/test_token_ring
```

Как запустить сеть (файл `main.cpp`)
```bash
build$ make main
build$ ./main
```
