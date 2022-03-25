kill -9 $(ps aux | grep bunker | grep -v "grep" | cut -d " " -f10)
