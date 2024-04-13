#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <conio.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>

using namespace std;

atomic<bool> downloading(true);
mutex mtx;
const string space = "                                                                         \r";

// 加载动画线程函数
void loadingAnimation() {
	const char symbols[] = {'/', '|', '\\', '-'};
	const int numSymbols = sizeof(symbols) / sizeof(symbols[0]);
	int symbolIndex = 0;

	while (downloading) {
		{
			lock_guard<mutex> lock(mtx);
			cout << "   " << symbols[symbolIndex] << " 下载中..." << flush;
			symbolIndex = (symbolIndex + 1) % numSymbols;
		}
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

// 模拟文件下载函数
void downloadFile() {
	thread loadingThread(loadingAnimation);

	// 模拟文件下载，这里用循环代替实际的下载操作
	for (int i = 0; i <= 100; ++i) {
		{
			lock_guard<mutex> lock(mtx);
			// 清空上一次的进度条
			cout << "\r";
			// 打印进度条
			cout << "[" << string(i / 5, '=') << string(20 - i / 5, ' ') << "] " << i << "%" << flush;
		}
		// 模拟下载延迟
		this_thread::sleep_for(chrono::milliseconds(20));
	}

	downloading = false;
	loadingThread.join();
	cout << "\r" << space << "下载完成！";
    this_thread::sleep_for(chrono::milliseconds(50));
}


// 函数用于将全角符号转换为半角符号
string fullwidth_to_halfwidth(const string& strBuff) {
	string strResult = "";
	for (size_t i = 0; i < strBuff.length(); ++i) {
		if ((strBuff[i] & 0xff) == 0xA1 && (strBuff[i + 1] & 0xff) == 0xA1) {
			// 全角空格
			strResult += 0x20;
			++i;
		} else if ((strBuff[i] & 0xff) == 0xA3 &&
		           (strBuff[i + 1] & 0xff) >= 0xA1 &&
		           (strBuff[i + 1] & 0xff) <= 0xFE) {
			// ASCII码中可见字符
			strResult += strBuff[++i] - 0x80;
		} else {
			if (strBuff[i] < 0) {
				// 如果是中文字符，则拷贝两个字节
				strResult += strBuff[i++];
			}
			strResult += strBuff[i];
		}
	}
	return strResult;
}

// 函数用于处理文件并保存结果
void process_file(const string& file_path) {
	string cleaned_file_path = file_path;
	// 检测文件路径是否被两个双引号包裹，如果是则删除双引号
	if (cleaned_file_path.front() == '"' && cleaned_file_path.back() == '"') {
		cleaned_file_path = cleaned_file_path.substr(1, cleaned_file_path.size() - 2);
	}
	// 打开原始文件
	ifstream input_file(cleaned_file_path);
	if (!input_file.is_open()) {
		cerr << "无法打开文件：" << cleaned_file_path << endl;
		return;
	}
	// 读取文件内容
	string content((istreambuf_iterator<char>(input_file)), istreambuf_iterator<char>());
	// 将全角符号转换为半角符号
	string halfwidth_content = fullwidth_to_halfwidth(content);
	// 构造新文件名
	size_t last_slash_index = cleaned_file_path.find_last_of("\\");
	size_t last_dot_index = cleaned_file_path.find_last_of(".");
	string file_name = cleaned_file_path.substr(last_slash_index + 1, last_dot_index - last_slash_index - 1);
	string file_extension = cleaned_file_path.substr(last_dot_index);
	string new_file_path = cleaned_file_path.substr(0, last_slash_index + 1) + file_name + "_changed" + file_extension;

	// 打开新文件并保存结果
	ofstream output_file(new_file_path);

	if (!output_file.is_open()) {
		cerr << "\r" << "无法创建新文件：" << new_file_path << endl;
		return;
	}

	output_file << halfwidth_content;
	cout << "\r" << "文件已成功转换并保存为：" << new_file_path << endl;
}

int main() {
	string password = "$asdkl92#ad", input, output = "请输入PIN:";
	char ch;

	cout << "\033]0;全角转半角v0.3\007";

	cout << output;
	while (1) {
		ch = _getch();
		if (int(ch) == 13) {
			cout << "\r";
			break;
		} else if (int(ch) == 8) {
			if (!input.empty()) {
				input.pop_back();
				cout << '\b' << ' ' << '\b'; // 光标回退一格，输出空格，再回退一格
			}
		} else {
			input += ch;
			cout << '*';
		}
	}

	if (input != password) {
		cout << "PIN值错误，程序终止.";
		return 0;
	} else {
		cout << "PIN值正确，允许执行下面操作";
	}

	string file_path;
	cout << "\r" << space << "请输入文件路径：";
	getline(cin, file_path);
	this_thread::sleep_for(chrono::milliseconds(500));

	cout << "\033[A" << "                                                                                            ";

	downloadFile();

	this_thread::sleep_for(chrono::milliseconds(1000));
	process_file(file_path);
	return 0;
}

