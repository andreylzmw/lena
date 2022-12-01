# About

## 🔒 Problem

- How to comprehensively test a codec in automatic mode?
- Creating, development and support of a codec is a very complex and fragile task. Programmers need to regularly check the current assembly on different data and in different modes

## 🔑 Solution

- Cloud platform
    - Testing on different metrics and datasets
    - Formation of a single unit-like test, which can be viewed by url
    
## For now
Supported metrics: MSE

Supported databases: CID2013

# Build
## 0. Clone repo
```
git clone https://github.com/andreylzmw/lena.git && cd lena
```

## 1. Download pre-compiled and extracted CID2013 image database from Yandex Disk
```
pip3 install wget requests
python3 ya.py
```
After downloading (there will be error) rename tmp file to output.zip and unzip it.

## 2. Build jpeg extract tool and metrics testing engine
macOS:
```
gcc jpeg.c -o jpeg && gcc metrics.c -o metrics
```
Linux:
```
gcc jpeg.c -o jpeg -lm && gcc metrics.c -o metrics -lm
```

## 3. Run node js server
```
npm install
node server.js
```

## 4. Check locahost on port 3000 and click 'Start testing'
## 5. Wait while page loading and enjoy results!
