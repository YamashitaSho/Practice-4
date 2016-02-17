# Practice-4
##概要
オリジナル文字列から検索文字列を探し出し、該当する文字列があれば &lt;b&gt; タグで囲って結果を表示してファイルを保存する。複数個存在すれば、すべて囲む。検索する文字列が見つからなければ該当なしメッセージを出す。それ以外はエラーメッセージを出す。
##コンパイル方法
gcc -Wall -o tcmkb tcmkb.c
##実行方法
tcmkb -o "abcdefg" -f "bcd" -h "output(.htm)"

tcmkb -o "string1" -f "string2"	-h "output"に限り
tcmkb "string1" "string2" "output"
と書くことが出来る。(outputは省略可能)
-hオプションを省略した場合は、b.htmに保存される。