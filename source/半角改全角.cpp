#include <iostream>
#include <fstream>
#include <string>
#include <locale>
using namespace std;



// 函数用于将半角符号转换为全角符号
string halfwidth_to_fullwidth(const string& str) {
	string result = "";
	unsigned char tmp;
	unsigned char tmp1;
	for (unsigned int i = 0; i < str.length(); i++) {
		tmp = str[i];
		tmp1 = str[i + 1];
		//cout << "uchar:" << (int) tmp << endl;
		if (tmp > 32 && tmp < 128) {
			//是半角字符
			result += 163;//第一个字节设置为163
			result += (unsigned char)str[i] + 128; //第二个字节+128;
		} else if (tmp >= 163) {
			//是全角字符
			result += str.substr(i, 2);
			i++;
			continue;
		} else if (tmp == 32) {
			//处理半角空格
			result += 161;
			result += 161;
		} else {
			result += str.substr(i, 2);
			i++;
		}
	}
	return result;
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
	// 将半角符号转换为全角符号
	string fullwidth_content = halfwidth_to_fullwidth(content);
	// 构造新文件名
	size_t last_slash_index = cleaned_file_path.find_last_of("\\");
	size_t last_dot_index = cleaned_file_path.find_last_of(".");
	string file_name = cleaned_file_path.substr(last_slash_index + 1, last_dot_index - last_slash_index - 1);
	string file_extension = cleaned_file_path.substr(last_dot_index);
	string new_file_path = cleaned_file_path.substr(0, last_slash_index + 1) + file_name + "_changed" + file_extension;
    
	// 打开新文件并保存结果
	ofstream output_file(new_file_path);
    
	if (!output_file.is_open()) {
		cerr << "无法创建新文件：" << new_file_path << endl;
		return;
	}
    
	output_file << fullwidth_content;
	cout << "文件已成功转换并保存为：" << new_file_path << endl;
}

int main() {
    string file_path;
    cout << "请输入文件路径：";
    getline(cin, file_path);
    
    process_file(file_path);
    return 0;
}
