osascript -e 'tell app "firebird" to activate'; sleep 2; cat pw.txt| while read a; do
echo "$a" | pbcopy
osascript -e "tell app \"System Events\" to keystroke \"v\" using command down"
osascript -e "tell app \"System Events\" to keystroke return"
done
osascript -e "tell app \"System Events\" to keystroke \"c\" & return"
