# Lambda

A simple lambda calculus evaluator. Its just a bunch of functions you define and use as data and pass into other functions.

## How to install

```
git clone https://github.com/lisk77/lambda && cd lambda
mkdir build && cd build
cmake ..
make
```

## How to use

Write your code into a file and pipe it into the binary

```
cat file | lambda
```

or just echo the code directly

```
echo "main = \\ x . x;" | lambda
```

### Syntax

Lambda expression
```
\ param1 param2 param3 ... param3 . body
```

Name definition
```
identifier = lambda expression | identifier;
```

For a file to be able to be run, it needs to define a `main` function.

## Future ideas

- [ ] types
- [ ] imports
- [ ] small standard library
