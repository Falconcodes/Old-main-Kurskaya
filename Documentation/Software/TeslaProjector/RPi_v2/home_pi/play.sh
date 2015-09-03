## XBMC: Remotely Start Playing a Media File From the Command Line Using the JSON API
## Configure your XBMC RPC details here

# You should not have to touch anything below
ID=$1
if [ "$ID" == "" ];
then
  echo "Syntax $0 </full/path/file.[mp4|mp3|etc]>"
  exit
fi
function xbmc_req {
  curl -s -i -X POST --header "Content-Type: application/json" -d "$1" http://127.0.0.1/jsonrpc
}
echo -n "Opening video id $ID on $XBMC_HOST ..."
xbmc_req '{"jsonrpc": "2.0", "method": "Playlist.Clear", "params":{"playlistid":1}, "id": 1}';
xbmc_req '{"jsonrpc": "2.0", "method": "Playlist.Add", "params":{"playlistid":1, "item" :{ "file" : "'$ID'"}}, "id" : 2}';
xbmc_req '{"jsonrpc": "2.0", "method": "Player.Open", "params":{"item":{"playlistid":1, "position" : 0}, "options": {"repeat": "all"}}, "id": 3}';
echo " done."
