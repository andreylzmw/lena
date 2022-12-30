# About

## 🔒 Problem

- How to comprehensively test a codec in automatic mode?
- Creating, development and support of a codec is a very complex and fragile task. Programmers need to regularly check the current assembly on different data and in different modes

## 🔑 Solution

- Cloud platform
    - Testing on different metrics and datasets
    - Formation of a single unit-like test, which can be viewed by url
    
## For now
Supported metrics: MSE

Supported databases: CID2013

# Depencies
- brew

# Build (only for macOS)
## 0. Clone repo
```
git clone https://github.com/andreylzmw/lena.git && cd lena
```

## 1. Build
```
make install
```

## 2. Download and process CID2013
```
make CID2013
```

## 3. Run server
```
make server
```

## 4. Check locahost on port 3000 and click 'Start testing'
## 5. Wait while page loading and enjoy results!

# TODO
- [x] Fix downloading process
- [x] Process datasets (fix paths)
- [x] Replace python with make for all auto-steps
- [x] Install depencies automatically
- [x] Open image from with (also as runs executable)
- [] Live output in web app
- [] Perfomance tests
- [] BMP decoder
- [] PNG decoder