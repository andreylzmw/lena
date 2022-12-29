LOGS_CREATED := $(shell mkdir logs)
DATABASES_CREATED := $(shell cd databases && echo 1 || echo 0)

BREW_INSTALLED := $(shell brew -v > logs/brew.txt && echo 1 || echo 0)
GCC_INSTALLED := $(shell gcc -v &> logs/gcc.txt && echo 1 || echo 0)
NODE_INSTALLED := $(shell node -v &> logs/node.txt && echo 1 || echo 0)
PYTHON_INSTALLED := $(shell python3 -V &> logs/python.txt && echo 1 || echo 0)
FFMPEG_INSTALLED := $(shell ffmpeg -version &> logs/ffmpeg.txt && echo 1 || echo 0)
CONVERT_INSTALLED := $(shell convert -v &> logs/convert.txt && echo 1 || echo 0)

install:

# INSTALL HOMEBREW
	$(info checking if brew is installed...)
ifeq ($(BREW_INSTALLED), 0)
	$(info brew not installed, installing...)
	curl -O "https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh"
	/bin/bash install.sh
	rm -f install.sh
else
	$(info brew installed, continue...)
endif

# INSTALL GCC
	$(info checking if gcc is installed...)
ifeq ($(GCC_INSTALLED), 0)
	$(info gcc not installed, installing...)
	xcode-select --install
else
	$(info gcc installed, continue...)
endif

# INSTALL NODE
	$(info checking if node is installed...)
ifeq ($(NODE_INSTALLED), 0)
	$(info node not installed, installing...)
	brew install node
else
	$(info node installed, continue...)
endif

# INSTALL PYTHON
	$(info checking if python is installed...)
ifeq ($(PYTHON_INSTALLED), 0)
	$(info python not installed, installing...)
	brew install python3
else
	$(info python installed, continue...)
endif

# INSTALL FFMPEG
	$(info checking if ffmpeg is installed...)
ifeq ($(FFMPEG_INSTALLED), 0)
	$(info ffmpeg not installed, installing...)
	brew install ffmpeg
else
	$(info ffmpeg installed, continue...)
endif

# INSTALL CONVERT
	$(info checking if convert is installed...)
ifeq ($(CONVERT_INSTALLED), 0)
	$(info convert not installed, installing...)
	brew install imagemagick
else
	$(info convert installed, continue...)
endif


CID2013:
ifeq ($(DATABASES_CREATED), 0)
	mkdir "databases"
endif

	$(info installing p7zip...)
	brew install p7zip

	echo "downloading CID2013..."
	./download.py "https://disk.yandex.ru/d/J18bMLCl_zmaTw" databases/CID2013.7z
	
	echo "unpacking CID2013..."
	7za -odatabases/CID2013 x databases/CID2013.7z
	
	mkdir "src/output"
	gcc src/jpeg.c -o src/output/jpeg && gcc src/metrics.c -o src/output/metrics

	./helpers/process.py databases/CID2013

	./src/output/metrics databases/CID2013.txt

run:
	npm --prefix server install ./server
	$(info Starting server...)
	node server/server.js

clean:
	rm -f src/output/metrics src/output/jpeg
	rm -rf server/node_modules
	rm -rf logs

