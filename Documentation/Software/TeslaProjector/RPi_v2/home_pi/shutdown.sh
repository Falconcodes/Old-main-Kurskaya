function xbmc_req {
  curl -s -i -X POST --header "Content-Type: application/json" -d "$1" http://127.0.0.1/jsonrpc
}

xbmc_req '{"jsonrpc": "2.0", "method": "System.Shutdown", "id": 1}';

echo " Done. "
