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
int argchk(char* arg);				//コマンドラインオプションの識別
void errmsg(int p);					//エラーメッセージ表示
int srch(char* org, char* match, int *srchcount, char *boldspot);	//検索
char BoldInsert(char* org, char* boldspot, char* result, int srchcount, int orglength);
int allsmall(char* text);

char opname_d[] = "b";				//デフォルトの保存ファイル名

int i; int j; int k;

//------------------------------------------------------------------------------
//main関数ここから----------------------------------------------------------------
int main(int argc, char *argv[]){	//コマンドライン引数
	
	int chk = 0;					//エラー検出用変数 0でなければエラー
	int s_org = 0;					//検索対象テキスト指定のオプション
	int s_find = 0;					//検索文字列指定のオプション
	int s_save = 0;					//保存先ファイルのオプション
	
//	ループ毎に1つずつコマンドラインオプションを取得する：argchk(argv[i])	
	for(i = 1; i < argc; i++){		//1つ目のパラメータは実行ファイルなので弾く
		switch (argchk(argv[i])){
		  case 'f':
			if (s_find == 0){
				s_find = i;
			}else{chk = 1;}
			break;
		  case 'h':
			if (s_save == 0){
				s_save = i;
			}else{chk = 1;}
			break;
		  case 'o':
			if (s_org == 0){
				s_org = i;
			}else{chk = 1;}
			break;
		  default:
			break;
		}
	}
//-f -oオプションの指定漏れがないかチェック
//s_org,s_findが全て0かつargc==3または4 → 省略と判断
//省略に当てはまらず、0のものがある → 引数エラー
	if (!(s_org && s_find) && argc == 3){		//(-f) string1 (-o) string2 (-h savefile)
		s_org = 0;
		s_find = 1;
		s_save = 0;
	}else if (!(s_org && s_find) && argc == 4){	//(-f) string1 (-o) string2 (-h) savefile
		s_org = 0;
		s_find = 1;
		s_save = 2;
	}else if(!(s_org && s_find)){
		chk = 1;
	}
	if (chk != 0 ){errmsg(chk);return 0;}
//	printf("s_org:%d\ns_find:%d\n", s_org, s_find);
//検索絡みの変数の初期化
	char *org=argv[s_org+1];				//検索先文字列のポインタ変数
	char *match=argv[s_find+1];				//検索文字列のポインタ変数
	int orglength = strlen(org);
	int srchcount = 0;
	char boldspot[orglength+1];				//発見位置保存配列のポインタ
	memset(boldspot, '\0', orglength+1);
	chk = srch(org, match, &srchcount, boldspot);		//検索
	
//文字列を発見した場合の処理----------------
	if(srchcount >= 1){
		char result[orglength + srchcount*7];
		memset(result, '\0', orglength + srchcount*7 - 1);
		BoldInsert(org, boldspot, result, srchcount, orglength);	//resultで<b>挿入後の文を受け取る
		
//ファイル保存にかかる初期化
		char *opname; int fchk = 1;			//opname:指定ファイル名 fchk:ファイルエラーチェック
		int exex = 0;						//拡張子追加フラグ
		if (s_save != 0){opname = argv[s_save+1];
		}else{opname = opname_d;}			//opname_d:デフォルトの出力ファイル名
		char *name_ex=strstr(opname, ".");	//拡張子が始まるポインタを取得
		if (name_ex != NULL){				//拡張子がある
			if (strlen(name_ex) > 2){allsmall(name_ex);}		//小文字に揃える
			if (strcmp(name_ex, ".html") != 0){	//拡張子が違う
			exex = 1;}
		}else{exex = 1;}
		char savefile[strlen(opname)+6];
		memset(savefile, '\0', strlen(opname)+6);
		strncat(savefile, opname, strlen(opname));
		if (exex == 1) { strncat(savefile, ".html", 5); }
		FILE *svfp=fopen(savefile,"w");		//savefile_pointer ファイルをwモードでオープン
		printf("File saved:%s\n", savefile);
				
		fchk = fputs(result, svfp);			//結果をファイルポインタに書き込む
		fchk = fclose(svfp);				//ファイルをクローズ
		if (fchk != 0){ chk = 3; }			//NULLエラーが出ていたら出力する（よくわからない）
//結果表示
		printf("%s\n", result);	
	}else{
		
//発見していない場合の処理------------------
		
		printf("文字列\"%s\"は見つかりませんでした。\n", match);
	}
	errmsg(chk);
	return 0;
}
//main関数ここまで----------------------------------------------------------------
//------------------------------------------------------------------------------

//コマンドライン引数の判別----------------------------------------------------------
int argchk(char* arg){
	if ((arg[0] == '-') && (arg[1] > 32)){	//制御文字,空白文字指定は無視
	return arg[1];}							//オプションの2文字目を返す
	return 0;								
}

//アルファベットを全て小文字に変える(半角文字列のみ)
int allsmall(char* text){
	for (int i=0;i<strlen(text);i++){
		if (text[i]>='A' && text[i]<='Z')
		text[i] += 'a' - 'A';
	}
	return 0;
}

//検索---------------------------------------------------------------------------
int srch(char *org, char *match, int *srchcount, char *boldspot){
	//	方針:1文字目を探す→全一致かどうか確認する
	int orglength = strlen(org);
	int matchlength = strlen(match);
	int scm = orglength - matchlength + 1;	//サーチ回数
	if (scm <= 0) {return 2;}				//検索文字列が対象文字列より長い=>エラー
	for(i = 0; i < scm;){					//ハズレまたは発見が確定した時にi++
		if (org[i] == match[0]) {			//1文字目が一致
			for(j = 0; j < matchlength; j++){
				if (org[i+j] != match[j]) {
					i++;break;				//2文字目以降でハズレ
				}
				if ((org[i+j] == match[j]) & (j == matchlength - 1)) {
					*srchcount += 1;			//発見
					boldspot[i] += 1;		//<b>挿入位置を記録する
					for (k = 0; k < matchlength-1 ; k++){
						i++;				//発見した文字分インデックスを進める
					}
					boldspot[i+1] += 2;		//</b>挿入位置を記録する
					i++;break;
				}
			}
		} else {							//1文字目もハズレ
			i++;
		}
	}
	
	
	return 0;
}

//<b>挿入関数--------------------------------------------------------------------
char BoldInsert(char *org, char *boldspot, char *result, int srchcount, int orglength){
//	printf("boldspot:%s\n", boldspot);
	int cursor = 0; int chk = 0;
	for (int i=0;i<=orglength;i++){
		switch (boldspot[i]){
		  case '\0':					//null文字のままだった
			result[cursor] = org[i];
			cursor += 1;				//1バイト進む
			break;
		  case '\0'+1:
			strncat(result, "<b>", 3);
			result[cursor+3] = org[i];
			cursor += 4;				//4バイト進む:<b>挿入
			break;
		  case '\0'+2:
			strncat(result, "</b>", 4);
			result[cursor+4] = org[i];
			cursor += 5;				//5バイト進む:</b>挿入
			break;
		  case '\0'+3:
			strncat(result, "</b><b>", 7);
			result[cursor+7] = org[i];
			cursor += 8;				//8バイト進む:</b><b>挿入
			break;
		  default:
			printf("spot ERROR\n");
			chk = 3;
			break;
		}
	}
	return (chk);
}


//エラーメッセージ-----------------------------------------------------------------
void errmsg(int chk){
	switch (chk){
	  case 0:
		break;
	  case 1:
		printf("引数が不正です。\n");
		break;
	  case 2:
		printf("検索文字列が探索文字列より長いです。\n");
		break;
	  case 3:
		printf("ファイル出力のエラーです。\n");
	  default:
		printf("不正なエラーです。\n");
		break;
	}
}