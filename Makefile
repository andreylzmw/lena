OS := $(shell uname -s)

DATABASES_CREATED := $(shell cd databases && echo 1 || echo 0)
OUTPUT_CREATED := $(shell cd src/output && echo 1 || echo 0)

install:
ifeq ($(OS), Linux)
	ulimit -n 1024
endif

# INSTALL CURL
ifeq ($(OS), Linux)
	sudo apt-get install curl
else
	$(info curl installed, continue...)
endif

# INSTALL GCC
ifeq ($(OS), Linux)
	brew install gcc
endif

# INSTALL WGET
	brew install wget

# INSTALL NODE
	brew install node

# INSTALL PYTHON
	brew install python3

# INSTALL FFMPEG
	brew install ffmpeg

# INSTALL CONVERT
	brew install imagemagick

CID2013:
ifeq ($(DATABASES_CREATED), 0)
	mkdir "databases"
endif

	$(info installing p7zip...)
	brew install p7zip

	$(info downloading CID2013...)
	./download.py "https://disk.yandex.ru/d/URF04zSkDRDlyQ" databases/CID2013.7z
	
	$(info unpacking CID2013...)
	7za -odatabases/CID2013 x databases/CID2013.7z
	
ifeq ($(OUTPUT_CREATED), 0)
	mkdir "src/output"
endif

ifeq ($(OS), Linux)
	gcc src/jpeg.c -o src/output/jpeg -lm && gcc src/metrics.c -o src/output/metrics -lm
else
	gcc src/jpeg.c -o src/output/jpeg && gcc src/metrics.c -o src/output/metrics
endif
	./helpers/process.py databases/CID2013

run:
	npm --prefix server install ./server
	$(info Starting server...)
	node server/server.js

clean:
	rm -f src/output/metrics src/output/jpeg
	rm -rf server/node_modules
