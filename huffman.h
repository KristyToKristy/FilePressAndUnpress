#include"Heap.h"
#include<iostream>
#include<time.h> 
#include<fstream>
#include<string>
using namespace std;
template<class T>
struct HuffmanTreeNode
{
	HuffmanTreeNode<T>* _left;
	HuffmanTreeNode<T>* _right;
	T _weight;
	HuffmanTreeNode(const T& w)
		:_left(NULL)
		,_right(NULL)
		,_weight(w)
	{}
};
template<class T>
class HuffmanTree
{
	typedef HuffmanTreeNode<T> Node;
public:
	HuffmanTree()
		:_root(NULL)
	{}
	HuffmanTree(const T* a, size_t n,const T& invalid)
	{
		_root = _GreatTree(a, n,invalid);
	}
	~HuffmanTree()
	{
		Destory(_root);
	}
	Node* GetRootNode()
	{
		return _root;
	}
protected:
    Node* _GreatTree(const T* a,size_t n,const T& invalid)
	{
		struct Compare 
		{
			bool operator()(const Node* l,const Node* r)
			{
				return l->_weight < r->_weight ;
			}
		};
		Heap<Node*, Compare> minHeap;
		for(size_t i=0;i<n;++i)
		{
			if(a[i] != invalid)
			{
				minHeap.Push(new Node(a[i]));
			}
		}
		while(minHeap.Size()>1) //这里不能用Empty(),因为两两配对后可能只剩一个的情况
		{
			Node* left = minHeap.Top();
			minHeap.Pop();
			Node *right = minHeap.Top();
			minHeap.Pop();
			Node* parent = new Node(left->_weight + right->_weight );
			parent->_left = left;
			parent->_right = right;
			minHeap.Push(parent);
		}
		return minHeap.Top();
	}
	
	void Destory(Node* root)
	{
		if(root == NULL)
		{
			return;
		}
		Destory(root->_left);
		Destory(root->_right);
		delete root;
	}
	Node* GetRoot()
	{
		return _root;
	}
private:
	Node* _root;
};

typedef unsigned long LongType;
struct CharInfo
{
	unsigned char _ch;
	LongType _count;
	string _code;
	CharInfo()
		:_ch(0)
		,_count(0)
	{}
	CharInfo(LongType count)
		:_ch(0)
		,_count(count)
	{}
	bool operator !=(const CharInfo& info)const
	{
		return _count != info._count ;
	}
	CharInfo operator + (const CharInfo&info)const
	{
		return CharInfo(_count+info._count );
	}
	bool operator<(const CharInfo &info)const
	{
		return _count <info._count ;
	}
};
//template <class T>
class FileCompress
{
//typedef HuffmanTreeNode<T> Node;
public:
	FileCompress()
	{
		for(size_t i = 0;i<256;++i)
		{
			_infos[i]._ch = i;
			_infos[i]._count = 0;
		}
	}
public:
	void Compress(const char* filename)
	{
		//assert(filename);
		FILE* fout = fopen(filename,"rb");//以二进制形式读
		assert(fout);
		char ch = fgetc(fout);
		while(!feof(fout))
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fout);
		}
		//构建哈弗曼树
		CharInfo invalid(0);
		HuffmanTree<CharInfo >tree(_infos ,256,invalid);
		string code;
		GenerateHuffmanCode(tree.GetRootNode(),code);
		string Compressfilename = filename;
		Compressfilename +=".compress";
		FILE* fin = fopen(Compressfilename.c_str(),"wb");
		fseek(fout,0,SEEK_SET);
		ch = fgetc(fout);
		char value = 0;
		int pos = 0;
		while(!feof(fout))
		{
			string& code = _infos[(unsigned char)ch]._code;
			for(size_t i =0;i<code.size();++i)
			{
				value=value<<1;
				if(code[i] == '1' )
				{
					value |= 1;
				}
				if(++pos == 8)
				{
					fputc(value,fin);
					value = 0;
					pos = 0;
				}
			}
			ch = fgetc(fout);
		}
		if(pos != 0)
		{
			value <<=(8-pos);
			fputc(value,fin);
		}
		//配置文件，主要记录哈弗曼编码的个数
		string configfilename = filename;
		configfilename += ".config";
		FILE* finconfig = fopen(configfilename.c_str(),"wb");
		assert(finconfig);
		char buffer[128];
		string str;
		for(size_t i=0;i<256;++i)
		{
			if(_infos[i]._count >0)
			{
				str+=_infos[i]._ch ;
				str+=",";
				str+=_itoa(_infos[i]._count ,buffer,10);//10:十进制
				str+='\n';
			}
			fputs(str.c_str(),finconfig);
			str.clear();
		}
		fclose(fout);
		fclose(fin);
		fclose(finconfig);
	}
	//解压缩
	void UnCompress(const char* filename)
	{
		string configfilename = filename;
		configfilename +=".config";
		FILE* foutconfig = fopen(configfilename.c_str(),"rb");
		assert(foutconfig);
		string str;
		LongType charCount = 0;
		while(Readline(foutconfig,str))
		{
			if(str.empty())
			{
				str+='\n';
			}
			else
			{
				_infos[(unsigned char)str[0]]._count = atoi(str.substr(2).c_str());
				str.clear();
			}
		}
		//重建哈夫曼树
		CharInfo invalid(0);
		HuffmanTree<CharInfo >tree(_infos ,256,invalid);
	    charCount = tree.GetRootNode()->_weight ._count ;
		string Compressfilename = filename;
		Compressfilename += ".compress";
		FILE* fout = fopen(Compressfilename.c_str(),"rb");
		assert(fout);
		string UnCompressfilename = filename;
		UnCompressfilename +=".UnCompress";
		FILE* fin = fopen(UnCompressfilename.c_str(),"wb");
		assert(fin);
		char ch = fgetc(fout);
		HuffmanTreeNode<CharInfo >*root = tree.GetRootNode();
		HuffmanTreeNode<CharInfo >* cur = root;
		int pos = 7;
		while(1)
		{
			if(ch & (1<<pos))
			{
				cur = cur->_right ;
			}
			else
			{
				cur = cur->_left ;
			}
			if(pos-- == 0)
			{
				pos = 7;
				ch = fgetc(fout);//继续读取下一个字符
			}
			if(cur->_left == NULL && cur->_right == NULL)
			{
				fputc(cur->_weight ._ch ,fin);
				if(--charCount == 0)
				{
					break;
				}
				cur = root;
			}
		}
		fclose(fin);
	}
protected:
	void GenerateHuffmanCode(HuffmanTreeNode<CharInfo>*root,string code)
	{
		if(root == NULL)
		{
			return;
		}
		if(root->_left)
		{
			GenerateHuffmanCode(root->_left ,code + '0');
		}
		if(root->_right)
		{
			GenerateHuffmanCode(root->_right ,code + '1');
		}
		if((root->_left == NULL) && (root->_right == NULL))
		{
			_infos[root->_weight._ch]._code = code;
		}
	}
	bool Readline(FILE* fout,string& str)
	{
		char ch = fgetc(fout);
		if(feof(fout))
		{
			return false;
		}
		while(!feof(fout) && ch != '\n')
		{
			str+=ch;
			ch = fgetc(fout);
		}
		return true;
	}
protected:
	CharInfo _infos[256];
};
void TestCompress()
{
	FileCompress fh;
	//double start_compress = clock();
	//fh.Compress("fc.txt");
	//fh.UnCompress("fc.txt");
	//double finsh_compress = clock();
	fh.Compress("big.txt");
	//cout<<"压缩时间是："<<finsh_compress -start_compress <<"ms"<<endl;
}
void TestUnCompress()
{
	FileCompress fc;
	//double start_compress = clock();
	//fc.UnCompress("fc.txt");
	fc.UnCompress ("big.txt");
//	//double finsh_compress = clock();
//	//cout<<"解压缩时间是："<<finsh_compress -start_compress <<"ms"<<endl;
}
