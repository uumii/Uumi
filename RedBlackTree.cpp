#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;

typedef bool RB_COLOR;
#define RB_BLACK true
#define RB_RED false
#define bro(x) (((x)->ftr->lc == (x))?((x)->ftr->rc):((x)->ftr->lc))

template<typename T>
class RB_TREE {
protected:
	struct RB_Node;

	RB_Node* _root;
	RB_Node* _hot;

	int _size;

	RB_Node* LeftRotate(RB_Node*);		//��������
	RB_Node* RightRotate(RB_Node*);		//��������

	RB_Node* find(T);						//���Һ���

	void SolveDoubleRed(RB_Node*);		//���˫������
	void SolveLostBlack(RB_Node*);		//���ʧ������

	void RemoveTree(RB_Node*);			//DFS��������
public:

	RB_TREE() :_root(nullptr), _hot(nullptr), _size(0) {}

	struct iterator;

	iterator begin();
	static iterator end();
	int size();
	bool empty();

	iterator insert(T);

	iterator lower_bound(T);
	iterator upper_bound(T);

	bool remove(T);
	void clear();
	iterator serach(T);
};

template<typename T>
struct RB_TREE<T>::RB_Node {
	T val;
	RB_COLOR RBc;
	RB_Node* lc;
	RB_Node* rc;
	RB_Node* ftr;

	RB_Node(T v = T(), RB_COLOR rbc = RB_RED, RB_Node* father = nullptr, RB_Node* rchild = nullptr, RB_Node* lchild = nullptr)
		:val(v), RBc(rbc), ftr(father), lc(lchild), rc(rchild) {}

	RB_Node* LeftNode() {			//��������е�ǰ��ڵ�
		RB_Node* ptn = nullptr;
		if (lc) {
			ptn = lc;
			if (ptn->rc) {
				ptn = ptn->rc;
			}
		}
		else {
			ptn = this;
			while (ptn->ftr && ptn->ftr->lc != ptn) {
				ptn = ptn->ftr;
			}
			ptn = ptn->ftr;
		}
		return ptn;
	}

	RB_Node* RightNode() {			//��������еĺ�̽ڵ�
		RB_Node* ptn = nullptr;
		if (rc) {
			ptn = rc;
			while (ptn->lc) {
				ptn = ptn->lc;
			}
		}
		else {
			ptn = this;
			while (ptn->ftr && ptn->ftr->lc != ptn) {
				ptn = ptn->ftr;
			}
			ptn = ptn->ftr;
		}
		return ptn;
	}
};

template<typename T>
struct RB_TREE<T>::iterator {
protected:
	RB_Node* _real_node;
public:

	T operator*() {
		return _real_node->val;
	}

	bool operator==(iterator const& itr) {
		return _real_node->val == itr._real_node;
	}

	bool operator!=(iterator const& itr) {
		return _real_node->val != itr._real_node;
	}

	bool operator!() {
		return (_real_node == nullptr) ? true : false;
	}

	iterator& operator=(iterator const& itr) {
		_real_node = itr._real_node;
		return this;
	}

	iterator& operator+(int n) {
		while (n--) {
			if (_real_node == _real_node->RightNode())
				_real_node = nullptr;
			else
				_real_node = _real_node->RightNode();
		}
		return *this;
	}

	iterator& operator-(int n) {
		while (n--) {
			if (_real_node == _real_node->LeftNode())
				_real_node = nullptr;
			else
				_real_node = _real_node->LeftNode();
		}
		return *this;
	}

	iterator& operator++() {
		if (_real_node == _real_node->RightNode())
			_real_node = nullptr;
		else
			_real_node = _real_node->RightNode();
		return *this;
	}

	iterator& operator--() {
		if (_real_node == _real_node->LeftNode())
			_real_node = nullptr;
		else
			_real_node = _real_node->LeftNode();
		return *this;
	}

	//���캯��
	iterator(RB_Node* rb = nullptr) : _real_node(rb) {}//ָ�빹��
	iterator(iterator const& rb) : _real_node(rb._real_node) {}//����������
	iterator(T const& v) : _real_node(find(v)) {}//���ݹ���
};

template <typename T>
typename
RB_TREE<T>::RB_Node* RB_TREE<T>::find(T v) {
	RB_Node* ptn = _root;
	_hot = nullptr;
	while (ptn) {
		_hot = ptn;
		if (ptn->val == v) {
			break;
		}
		else if (ptn->val < v) {
			ptn = ptn->rc;
		}
		else {
			ptn = ptn->lc;
		}
	}
	return ptn;
}

template <typename T>
typename
RB_TREE<T>::RB_Node* RB_TREE<T>::LeftRotate(RB_Node* ptn) {
	RB_Node* pftr = ptn->ftr;
	RB_Node* prc = ptn->rc;
	ptn->rc = prc->lc;
	if (prc->lc) prc->lc->ftr = ptn;
	prc->lc = ptn;
	ptn->ftr = prc;
	prc->ftr = pftr;
	if (pftr) {
		if (pftr->lc == ptn)
			pftr->lc = prc;
		else
			pftr->rc = prc;
	}
	if (ptn == _root) { 	//ά�����ڵ�
		_root = prc;
	}
	return prc;
}

template <typename T>
typename
RB_TREE<T>::RB_Node* RB_TREE<T>::RightRotate(RB_Node* ptn) {
	RB_Node* pftr = ptn->ftr;
	RB_Node* plc = ptn->lc;
	ptn->lc = plc->rc;
	if (plc->rc) plc->rc->ftr = ptn;
	plc->rc = ptn;
	ptn->ftr = plc;
	plc->ftr = pftr;
	if (pftr) {
		if (pftr->lc == ptn)
			pftr->lc = plc;
		else
			pftr->rc = plc;
	}
	if (ptn == _root) {		//ά�����ڵ�	
		_root = plc;
	}
	return plc;
}

template <typename T>
void RB_TREE<T>::SolveDoubleRed(RB_Node* ptn) {
	while (ptn->ftr && ptn->ftr->RBc == RB_RED) {
		RB_Node* pftr = ptn->ftr;
		RB_Node* pgrd = pftr->ftr;
		RB_Node* uncle = bro(pftr);
		if (uncle == nullptr || uncle->RBc == RB_BLACK) {
			if (pgrd->lc == pftr) {
				if (pftr->lc == ptn) {
					RightRotate(pgrd);
					pftr->RBc = RB_BLACK;
				}
				else {
					LeftRotate(pftr);
					RightRotate(pgrd);
					ptn->RBc = RB_BLACK;
				}
			}
			else {
				if (pftr->lc == ptn) {
					RightRotate(pftr);
					LeftRotate(pgrd);
					ptn->RBc = RB_BLACK;
				}
				else {
					LeftRotate(pgrd);
					pftr->RBc = RB_BLACK;
				}
			}
			pgrd->RBc = RB_RED;
		}
		else {
			uncle->RBc = RB_BLACK;
			pftr->RBc = RB_BLACK;
			pgrd->RBc = RB_RED;
			ptn = pgrd;
		}
	}
	if (ptn == _root)
		ptn->RBc = RB_BLACK;

}

template <typename T>
void RB_TREE<T>::SolveLostBlack(RB_Node* ptn) {
	RB_Node* pftr = ptn->ftr;
	RB_Node* pbro = bro(ptn);
	if (pbro == nullptr) return;
	//���ֵ�Ϊ��ɫ��
	if (pbro->RBc == RB_BLACK) {
		//��û�к�ɫֶ�ӡ�
		if (pbro->rc == nullptr && pbro->lc == nullptr) {
			//û�к�ɫֶ��,�к�ɫ����
			if (pftr->RBc == RB_RED) {
				pftr->RBc = RB_BLACK;
				pbro->RBc = RB_RED;
			}
			//û�к�ɫֶ��,�к�ɫ����
			else if (pftr->RBc == RB_BLACK) {
				pbro->RBc = RB_RED;
				if (pftr != _root)
					SolveLostBlack(pftr);
			}
		}
		//���к�ɫֶ�ӡ�
		else {
			//���ֵ����ұߡ�
			if (pftr->lc == ptn) {
				//�ֵ����ұߣ��к�ɫ��ֶ��
				if (pbro->rc && pbro->rc->RBc == RB_RED) {
					LeftRotate(pftr);
					if (pftr->RBc == RB_RED) {
						pbro->RBc = RB_RED;
						pftr->RBc = RB_BLACK;
					}
					pbro->rc->RBc = RB_BLACK;
				}
				//�ֵ����ұߣ��к�ɫ��ֶ��
				else if (pbro->lc && pbro->lc->RBc == RB_RED) {
					RightRotate(pbro);
					LeftRotate(pftr);
					if (pftr->RBc == RB_BLACK) {
						pftr->ftr->RBc = RB_BLACK;
					}
					pftr->RBc = RB_BLACK;
				}
			}
			//���ֵ�����ߡ�
			else {
				//�ֵ�����ߣ��к�ɫ��ֶ��
				if (pbro->lc && pbro->lc->RBc == RB_RED) {
					RightRotate(pftr);
					if (pftr->RBc == RB_RED) {
						pbro->RBc = RB_RED;
						pftr->RBc = RB_BLACK;
					}
					pbro->lc->RBc = RB_BLACK;
				}
				//�ֵ�����ߣ��к�ɫ��ֶ��
				else if (pbro->rc && pbro->rc->RBc == RB_RED) {
					LeftRotate(pbro);
					RightRotate(pftr);
					if (pftr->RBc == RB_BLACK) {
						pftr->ftr->RBc = RB_BLACK;
					}
					pftr->RBc = RB_BLACK;
				}
			}
		}
	}
	//���ֵ�Ϊ��ɫ	��
	else {
		if (pftr->lc == ptn) {
			if (pftr == _root) _root = pftr->rc;
			LeftRotate(pftr);
			pbro->RBc = RB_BLACK;
			pftr->RBc = RB_RED;
		}
		else {
			if (pftr == _root) _root = pftr->lc;
			RightRotate(pftr);
			pbro->RBc = RB_BLACK;
			pftr->RBc = RB_RED;
		}
		SolveLostBlack(ptn);
	}
}

template <typename T>
void RB_TREE<T>::RemoveTree(RB_Node* ptn) {
	if (ptn == nullptr) return;
	RemoveTree(ptn->lc);
	RemoveTree(ptn->rc);
	delete ptn;
}

template <typename T>
typename
RB_TREE<T>::iterator RB_TREE<T>::begin() {
	if (_root == nullptr) return iterator(_root);
	RB_Node* ptn = _root;
	while (ptn->lc) {
		ptn = ptn->lc;
	}
	return iterator(ptn);
}

template <typename T>
typename
RB_TREE<T>::iterator RB_TREE<T>::end() {
	return iterator(nullptr);
}

template <typename T>
int RB_TREE<T>::size() {
	return _size;
}

template <typename T>
bool RB_TREE<T>::empty() {
	return (_size == 0) ? true : false;
}

template <typename T>
typename
RB_TREE<T>::iterator RB_TREE<T>::insert(T v) {
	RB_Node* ptn = find(v);
	if (ptn) return iterator(ptn);
	if (_hot == nullptr) {
		_root = new RB_Node(v, RB_BLACK, nullptr, nullptr, nullptr);
		_size = 1;
		return iterator(_root);
	}
	ptn = new RB_Node(v, RB_RED, _hot, nullptr, nullptr);
	if (_hot->val > v) {
		_hot->lc = ptn;
	}
	else {
		_hot->rc = ptn;
	}
	SolveDoubleRed(ptn);
	_size++;
	return iterator(ptn);
}

template <typename T>
bool RB_TREE<T>::remove(T v) {
	RB_Node* ptn = find(v);
	if (ptn == nullptr) return false;
	while (ptn->lc || ptn->rc) {
		RB_Node* temp = nullptr;
		if (ptn->rc) {
			temp = ptn->RightNode();
		}
		else {
			temp = ptn->LeftNode();
		}
		ptn->val = temp->val;
		ptn = temp;
	}
	if (ptn->RBc == RB_BLACK)
		SolveLostBlack(ptn);
	delete ptn;
	_size--;
	return true;
}

template <typename T>
typename
RB_TREE<T>::iterator RB_TREE<T>::lower_bound(T v) {
	RB_Node* ptn = _root;
	_hot = nullptr;
	while (ptn) {
		_hot = ptn;
		if (ptn->val >= v) {
			ptn = ptn->lc;
		}
		else {
			ptn = ptn->rc;
		}
	}
	if (_hot->val < v) {
		ptn = _hot->RightNode();
	}
	else {
		ptn = _hot;
	}
	return ptn;
}

template <typename T>
typename
RB_TREE<T>::iterator RB_TREE<T>::upper_bound(T v) {
	RB_Node* ptn = _root;
	_hot = nullptr;
	while (ptn) {
		_hot = ptn;
		if (ptn->val > v) {
			ptn = ptn->lc;
		}
		else {
			ptn = ptn->rc;
		}
	}
	if (_hot->val <= v) {
		ptn = _hot->RightNode();
	}
	else {
		ptn = _hot;
	}
	return ptn;
}

template <typename T>
void RB_TREE<T>::clear() {
	if (_root) {
		RB_Node* ptn = _root;
		_root = nullptr;
		_hot = nullptr;
		_size = 0;
		RemoveTree(_root);
	}
}

template <typename T>
typename
RB_TREE<T>::iterator RB_TREE<T>::serach(T v) {
	return iterator(find(v));
}

int main(void)
{
	RB_TREE<double> tree;

	for (int i = 10; i >= 0; i--) {
		tree.insert(i);
	}
	tree.insert(8.5);
	cout << tree.size() << endl;
	tree.remove(10);
	cout << tree.size() << endl;

	int aa = 0;

	return 0;
}