#!/bin/bash
curl -F credential_0="$1" -F credential_1="$2" -F credential_2=345600 -F destination=/auth/ -c cookies_"$1".txt -k https://is.muni.cz/system/login_form.pl
