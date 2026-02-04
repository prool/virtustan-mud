#!/bin/bash
# Scroll syslog (like tail -f) with recode koi8-r -> UTF-8
# This code written by Windows 11 Copilot.
# Prool
# 04 Feb 2026, Duisburg, Germany

tail -n 0 -F syslog | while IFS= read -r line; do
  printf '%s\n' "$line" | iconv -f koi8-r -t UTF-8
done

