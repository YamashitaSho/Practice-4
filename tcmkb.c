#include <stdio.h>
#include <string.h>

/*
C言語学習用課題
コマンドラインを利用し、文字列oから指定文字列fを検索する
該当するものがあれば<b>タグで囲む。
結果は"hで指定されたもの".htmとし、指定されなければb.htmとする。

コマンドライン
-o string: 検索対象テキスト
-f string: 検索文字列
-h string(.htm) : 保存ファイル名(.htmがなければつける)

tcmkb -o string1 -f string2 -h savefileを
tcmkb string1 string2 savefile
と書くことができる。

 エラーメッセージ
・引数が不正な場合
・その他

*/
#define ARG_NOTFOUND 0				//コマンドライン引数未発見
#define ARG_ORIGINAL 0				//コマンドラインオプション-oの配列番号
#define ARG_SEARCH 1				//コマンドラインオプション-fの配列番号
#define ARG_SAVEFILE 2				//コマンドラインオプション-hの配列番号

#define ERROR_NO 0					//エラーなし
#define ERROR_ARG 1					//引数エラー
#define ERROR_LONG 2				//文字数エラー
#define ERROR_FILE 3				//ファイルエラー
#define ERROR_UNKNOWN -1			//不明なエラー

#define BOLDSPOT_FORMAT '0'			//発見位置 初期化
#define BOLDSPOT_START 1			//<b>タグ挿入位置フラグ
#define BOLDSPOT_END 2				//</b>タグ挿入位置フラグ

#define NOTFOUND 0					//文字列を発見できなかった
#define FOUND 1						//文字列を発見した

#define OPNAME_DEFAULT "b.html"		//デフォルトの保存ファイル名
#define OPNAME_DEFAULT_LONG 7		//デフォルトの保存ファイル名の要素数

int commandline_option_input(char *argv[], int commandlineoption[], int argc);
int arg_check(char* arg);				//コマンドラインオプションの識別
void error_msg(int p);					//エラーメッセージ表示
int search(char* org, char* match, int *srchcount, char *boldspot);	//検索
int bold_insert(char* org, char* boldspot, char* result, int srchcount, int orglength);
int file_save(char* result, char *opname, int s_save);
void allsmall(char* text);

int i; int j; int k;

//------------------------------------------------------------------------------
//main関数ここから----------------------------------------------------------------
int main(int argc, char *argv[]){	//コマンドライン引数
	
	int chk = ERROR_NO;				//エラー検出用変数 0でなければエラー
	int commandlineoption[3] = { ARG_NOTFOUND , ARG_NOTFOUND , ARG_NOTFOUND };
	chk = commandline_option_input(argv, commandlineoption, argc);		//コマンドラインオプション指定位置をcommandlineoptionに格納する
	char *org = argv[commandlineoption[ARG_ORIGINAL]+1];				//検索先文字列のポインタ変数
	char *match = argv[commandlineoption[ARG_SEARCH]+1];				//検索文字列のポインタ変数
	int orglength = strlen(org);										//検索対象テキストの文字数
	int matchlength = strlen(match);									//検索文字列の文字数
	int srchcount = 0;													//発見回数
	int resultmax = orglength + ( orglength / matchlength ) * 7;		//検索結果に<b>を挿入した場合の最大文字数
 	char boldspot[orglength+2];											//発見位置保存配列のポインタ(文字の先頭と文字間と文字終端で要素数+2)
	char result[resultmax];	
	
	memset(boldspot, BOLDSPOT_FORMAT , orglength + 1);
	memset(result, '\0', resultmax - 1);
	
	chk = search(org, match, &srchcount, boldspot);		//検索
	
//文字列を発見した場合の処理----------------
	if(srchcount >= FOUND){
		chk = bold_insert(org, boldspot, result, srchcount, orglength);	//resultで<b>挿入後の文を受け取る
		if (chk == ERROR_NO){				//問題ないなら保存
			chk = file_save(result,argv[commandlineoption[ARG_SAVEFILE]+1],commandlineoption[ARG_SAVEFILE]);
		}
		printf("%s\n", result);				//保存した結果を表示
		
//発見していない場合の処理------------------
	} else {
		printf("文字列\"%s\"は見つかりませんでした。\n", match);
	}
	error_msg(chk);
	return 0;
}
//main関数ここまで----------------------------------------------------------------
//------------------------------------------------------------------------------

//コマンドラインオプションを読み込む
int commandline_option_input(char *argv[], int commandlineoption[],int argc){
//	int s_org = commandlineoption[ARG_ORIGINAL];		//検索対象テキスト指定のオプション
//	int s_find = commandlineoption[ARG_SEARCH];		//検索文字列指定のオプション
//	int s_save = commandlineoption[ARG_SAVEFILE];		//保存先ファイルのオプション	
//	ループ毎に1つずつコマンドラインオプションを取得する：arg_check(argv[i])
	int chk = 0;
	for(i = 1; i < argc; i++){		//1つ目のパラメータは実行ファイルなので弾く
		switch (arg_check(argv[i])) {
		  case 'f':
			if (commandlineoption[ARG_SEARCH] == ARG_NOTFOUND) {
				commandlineoption[ARG_SEARCH] = i;
			} else {
				chk = ERROR_ARG;
			}
			break;
			
		  case 'h':
			if (commandlineoption[ARG_SAVEFILE] == ARG_NOTFOUND) {
				commandlineoption[ARG_SAVEFILE] = i;
			} else {
				chk = ERROR_ARG;
			}
			break;
			
		  case 'o':
			if (commandlineoption[ARG_ORIGINAL] == ARG_NOTFOUND) {
				commandlineoption[ARG_ORIGINAL] = i;
			} else {
				chk = ERROR_ARG;
			}
			break;
			
		  default:
			break;
		}
	}
//-f -oオプションの指定漏れがないかチェック
//s_org,s_findが全て0かつargc==3または4 → 省略と判断
//省略に当てはまらず、0のものがある → 引数エラー
//(-f) string1 (-o) string2 (-h savefile)
	if ((commandlineoption[ARG_ORIGINAL] == ARG_NOTFOUND && commandlineoption[ARG_SEARCH] == ARG_NOTFOUND) && argc == 3) {
		commandlineoption[ARG_ORIGINAL] = 0;				//2要素目を検索先文字列とする
		commandlineoption[ARG_SEARCH] = 1;				//3要素目を検索文字列とする
		commandlineoption[ARG_SAVEFILE] = ARG_NOTFOUND;	//保存ファイル名は省略されている
//(-f) string1 (-o) string2 (-h) savefile
	} else if ((commandlineoption[ARG_ORIGINAL] == ARG_NOTFOUND && commandlineoption[ARG_SEARCH] == ARG_NOTFOUND) && argc == 4) {
		commandlineoption[ARG_ORIGINAL] = 0;				//2要素目を検索先文字列とする
		commandlineoption[ARG_SEARCH] = 1;				//3要素目を検索文字列とする
		commandlineoption[ARG_SAVEFILE] = 2;				//4要素目をファイル名とする
	} else if (commandlineoption[ARG_ORIGINAL] == ARG_NOTFOUND && commandlineoption[ARG_SEARCH] == ARG_NOTFOUND) {
		chk = ERROR_ARG;
	}
	if (chk != ERROR_NO){
		error_msg(chk);
		return ERROR_NO;
	}
	return ERROR_NO;
}

//コマンドライン引数の判別----------------------------------------------------------
int arg_check(char* arg){
	if ((arg[0] == '-') && (arg[1] > 32)){	//制御文字,空白文字指定は無視
		return arg[1];
	}							//オプションの2文字目を返す
	return 0;								
}

//アルファベットを全て小文字に変える(半角文字列のみ)
void allsmall(char* text){
	for(int i=0; i < strlen(text) ;i++) {
		if (text[i]>='A' && text[i]<='Z') {
			text[i] += 'a' - 'A';
		}
	}
	return;
}

//検索---------------------------------------------------------------------------
int search(char *org, char *match, int *srchcount, char *boldspot){
	//	方針:1文字目を探す→全一致かどうか確認する
	int orglength = strlen(org);
	int matchlength = strlen(match);
	int scm = orglength - matchlength + 1;	//サーチ回数
	
	if (scm <= 0) {
		return ERROR_LONG;					//検索文字列が対象文字列より長い=>エラー
	}
	
	for(i = 0; i < scm;) {					//ハズレまたは発見が確定した時にi++
		if (org[i] == match[0]) {			//1文字目が一致
			for(j = 0; j < matchlength ; j++){
				if (org[i+j] != match[j]) {
					i++;						//2文字目以降でハズレ
					break;
				}
				if ((org[i+j] == match[j]) & (j == matchlength - 1)) {
					*srchcount += 1;			//発見
					boldspot[i] += BOLDSPOT_START;	//<b>挿入位置を記録する
					for (k = 0; k < matchlength ; k++){
						i++;				//発見した文字分インデックスを進める
					}
					boldspot[i] += BOLDSPOT_END;	//</b>挿入位置を記録する
					break;
				}
			}
		} else {								//1文字目もハズレ
			i++;
		}
	}
	
	return ERROR_NO;
}

//<b>挿入関数--------------------------------------------------------------------
int bold_insert(char *org, char *boldspot, char *result, int srchcount, int orglength) {
	int cursor = 0;
	int chk = ERROR_NO;
	
	for (int i=0; i < orglength+1 ;i++){
		switch (boldspot[i]){
		  case BOLDSPOT_FORMAT:			//フォーマット文字のままだった
			result[cursor] = org[i];
			cursor += 1;				//1バイト進む
			break;
			
		  case BOLDSPOT_FORMAT + BOLDSPOT_START :
			strncat(result, "<b>", 3);
			result[cursor+3] = org[i];
			cursor += 4;				//4バイト進む:<b>挿入
			break;
			
		  case BOLDSPOT_FORMAT + BOLDSPOT_END :
			strncat(result, "</b>", 4);
			result[cursor+4] = org[i];
			cursor += 5;				//5バイト進む:</b>挿入
			break;
			
		  case BOLDSPOT_FORMAT + BOLDSPOT_START + BOLDSPOT_END :
			strncat(result, "</b><b>", 7);
			result[cursor+7] = org[i];
			cursor += 8;				//8バイト進む:</b><b>挿入
			break;
			
		  default:
			printf("spot ERROR\n");
			chk = ERROR_UNKNOWN;
			break;
		}
	}
	return (chk);
}

//ファイルに保存-------------------------------------------------------------------
int file_save(char *result, char *opname, int s_save){
	int fchk = 1;										//fchk:ファイルエラーチェック
	char savefile[strlen(opname)+OPNAME_DEFAULT_LONG];	//保存ファイル名
	char *name_ex=strstr(opname, ".");					//拡張子が始まるポインタを取得
	
	memset(savefile, '\0', strlen(opname)+OPNAME_DEFAULT_LONG);
	
	if (s_save != ARG_NOTFOUND){				//コマンドラインでファイル名を渡されている
		strncat(savefile, opname, strlen(opname));
		if (name_ex != NULL) {					//拡張子がある
			allsmall(name_ex);					//小文字に揃える
			if (strcmp(name_ex, ".html") != 0){	//拡張子が.htmlと一致しない
				strncat(savefile, ".html", 6);	//.htmlをつける
			}
		} else {								//拡張子がない
			strncat(savefile, ".html", 6);		//.htmlをつける
		}
	} else {
		strcpy(savefile, OPNAME_DEFAULT);		//ファイル名の指定がないのでデフォルト名をつける
	}
	
	FILE *svfp = fopen(savefile,"w");	//savefile_pointer ファイルをwモードでオープン

	fchk = fputs(result, svfp);			//結果をファイルポインタに書き込む
	fchk = fclose(svfp);				//ファイルをクローズ
	
	printf("File saved:%s\n", savefile);
	
	if (fchk != 0) {
		return ERROR_UNKNOWN;			//NULLエラーが出ていたら出力する（よくわからない）
	}
	return ERROR_NO;
}


//エラーメッセージ-----------------------------------------------------------------
void error_msg(int chk){
	switch (chk){
	  case ERROR_NO:
		break;
		
	  case ERROR_ARG:
		printf("引数が不正です。\n");
		break;
		
	  case ERROR_LONG:
		printf("検索文字列が探索文字列より長いです。\n");
		break;
		
	  case ERROR_FILE:
		printf("ファイル出力のエラーです。\n");
		break;
		
	  default:
		printf("不正なエラーです。\n");
		break;
	}
}