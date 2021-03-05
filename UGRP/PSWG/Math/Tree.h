#include <Prelude.h>

#define _TREE_MEM_INIT_SIZE 4

template<typename _Ty> class __tree
{
private:
	__tree* parent{ NULL };
	int myNumber{0}; // parent[myNumber] = this;
	__tree** children{ NULL };
	int childrenNumber{ 0 };
	int childrenMemSize{ 0 };

	_Ty Value;

	void print(char depth);

public:
	
	//__tree& operator=(const __tree& copytree);
	//__tree(const __tree& copytree);

	~__tree();

	int getMyNumber();
	int getChildNumber();
	__tree* getParent();
	__tree* getChild(int number);
	
	void setParent(__tree* beParent);
	void setChild(__tree* beChild);
	
	_Ty getValue();
	void setValue(_Ty Data);

	void print();
};



template<typename _Ty> __tree<_Ty>::~__tree()
{
	
	for (int i = 0; i < childrenNumber; i++) { delete (children[i]); }
	this->setParent(NULL);
    delete[] children;
	//모든 연관 관계 해제
}

template<typename _Ty> int __tree<_Ty>::getMyNumber() { return myNumber; }
template<typename _Ty> int __tree<_Ty>::getChildNumber() { return childrenNumber; }
template<typename _Ty> __tree<_Ty>* __tree<_Ty>::getParent() { return parent; }
template<typename _Ty> __tree<_Ty>* __tree<_Ty>::getChild(int number) { return (i < childrenNumber) ? children[number] : NULL; }

template<typename _Ty> void __tree<_Ty>::setParent(__tree<_Ty>* beParent)
{
	if (beParent != parent)
	{
		__tree* everyParent = beParent;
		while (everyParent != NULL)
		{
			if (everyParent == this) { break; }
			everyParent = everyParent->parent;
		}
	
		if (everyParent == NULL)
		{// mean this's children doesn't contain beParent
			
			if (parent != NULL) {
				memcpy(parent->children + myNumber, parent->children + myNumber + 1, sizeof(__tree<_Ty>*)*(parent->childrenNumber - myNumber));
				parent->childrenNumber--;
				for (int i = myNumber; i < parent->childrenNumber; i++) { parent->children[i]->myNumber--; }
				if (_TREE_MEM_INIT_SIZE < parent->childrenMemSize && parent->childrenNumber * 4 <= parent->childrenMemSize)
				{
					__tree** temp = new __tree*[parent->childrenNumber * 2];
					memcpy(temp, parent->children, sizeof(__tree<_Ty>*)*parent->childrenNumber);
					parent->childrenMemSize = parent->childrenNumber * 2;
					delete[] parent->children;
					parent->children = temp;
				}
			}
			parent = beParent;
			if (beParent != NULL) {
				if (parent->children == NULL) {
					parent->children = new __tree*[_TREE_MEM_INIT_SIZE];
					parent->childrenMemSize = _TREE_MEM_INIT_SIZE;
				}
				if (parent->childrenNumber == parent->childrenMemSize)
				{
					__tree** temp = new __tree*[parent->childrenNumber * 2];
					memcpy(temp, parent->children, sizeof(__tree<_Ty>*)*parent->childrenNumber);
					parent->childrenMemSize = parent->childrenNumber * 2;
					delete[] parent->children;
					parent->children = temp;
				}
				myNumber = (parent->childrenNumber)++;
				parent->children[myNumber] = this;
			}
		}
		else
		{
			std::cout << "\n ERROR :: unable to make loop in tree" << std::endl;
		}
	}
}
template<typename _Ty> void __tree<_Ty>::setChild(__tree<_Ty>* beChild){this->setParent(beChild);}

template<typename _Ty> _Ty __tree<_Ty>::getValue() { return Value; }
template<typename _Ty> void __tree<_Ty>::setValue(_Ty Data) { Value = Data; }

template<typename _Ty> void __tree<_Ty>::print()
{
//	std::cout << this->parent << "|" << this->parent->Value;
	this->print(0);
	std::cout << std::endl;
}

template<typename _Ty> void __tree<_Ty>::print(char depth)
{
	for (int i = 0; i < depth; i++)
		std::cout << "\t\t\t ";
	if (depth == 0){std::cout << "-";}
	else{std::cout << "+";}
	std::cout << "----" << this->parent << "|" << this->childrenMemSize<<"|" << this << "|" << Value;

	for (int i = 0; i < childrenNumber; i++)
	{
		if (i == 0){children[0]->print(0);}
		else{ std::cout << "\n"; children[i]->print(depth + 1); }
	}
}	