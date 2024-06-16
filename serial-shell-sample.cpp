#include "serial-shell.h"
#include <vector>
#include <map>					/* unordered_map で良いはずだが、エラーが出るので… */

/* コマンドクラス */
class CmdBase
{
protected:
	long getParam(int index);
	/* アウトプット */
	void print(const std::string& str){}
	void print(unsigned char val, int base = 16){}
	void println(std::vector<unsigned char>& vals, int column = 16, int base = 16);
	void println(const std::string& str){}
public:
	virtual void exec(const std::string& param) = 0;
	virtual ~CmdBase(){}
};

/* コマンドクラス郡 */
class CmdHelp : public CmdBase
{
public:
	void exec(const std::string& param);
};

class CmdReadEep : public CmdBase
{
public:
	void exec(const std::string& param);
};

/* --- SerialShell 制御 --- */
namespace SerialShell
{
	/* コマンドテーブル作成用インスタンスポインタ取得 */
	template<class Cmd>
	static CmdBase* getCmdInstance()
	{
		static Cmd ins;
		return &ins;
	}

	/* コマンドテーブル */
	static const std::map<const std::string, CmdBase*> cmds_tab =
	{
		{"?", getCmdInstance<CmdHelp>()},
		{"help", getCmdInstance<CmdHelp>()},
		{"read", getCmdInstance<CmdReadEep>()},
		/* 必要に応じて追加 */
	};

	void exec(const std::string& cmd_line)
	{
		std::string::size_type name_end = cmd_line.find_first_of(' '); /* std::string::size_type */
		std::string name = (name_end > 0) ? cmd_line.substr(0, name_end) : cmd_line; /* 区切り文字がない場合は全文 */
		std::string param = (name_end > 0) ? cmd_line.substr(name_end + 1) : "";

#if 0							/* 例外処理を使うなら */
		cmds_tab.at(name)->exec(param);
#else
		auto it = cmds_tab.find(name);
		if (it != cmds_tab.end())
		{
			it->second->exec(param);
		}
#endif
	}
};

/* --- コマンドメソッド郡 --- */
/**
   byte配列のデータをフォーマット出力
   vals : データ vector
   column_num : 列の表示データ数
   base : 基数 (HEX, DEC, BIN, OCT)
*/
void CmdBase::println(std::vector<unsigned char>& vals, int column_num, int base)
{
	int column = 0;
	for (const auto ele : vals)
	{
		print(ele, base);
		if ((++column % column_num) == 0)
		{
			print("\n");
		}
	}
}

void CmdHelp::exec(const std::string& param)
{
	for (const auto& pair : SerialShell::cmds_tab)
	{
		println(pair.first);
#if 0				/* 説明をつける実装例…コマンドクラスにやらせる */
		print(pair.first);
		println(pair.second->getBrief());
#endif
	}
	println("show commands usage.");
}

void CmdReadEep::exec(const std::string& param)
{
	println("save the fun for later");
}
