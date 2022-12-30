#!venv/bin/python3

import os, sys, json
import urllib.parse as ul

sys.argv.append('.') if len(sys.argv) == 2 else None

base_url = 'https://cloud-api.yandex.net:443/v1/disk/public/resources/download?public_key='
url = ul.quote_plus(sys.argv[1])
folder = sys.argv[2]
res = os.popen('wget -qO - {}{}'.format(base_url, url)).read()
json_res = json.loads(res)
filename = ul.parse_qs(ul.urlparse(json_res['href']).query)['filename'][0]
os.system("caffeinate -s wget -c --retry-connrefused --tries=0 --timeout=5 '{}' -P '{}' -O '{}'".format(json_res['href'], folder, folder))