#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <unordered_map>
#include <tuple>
#include <iomanip>
#include <functional>
#include <set>
#include <array>

using namespace std;
#define MAX_CHILDREN 5



// Structures
template <typename T>
struct TreeNodeM {
	T val;
	TreeNodeM* children[MAX_CHILDREN];
	TreeNodeM(T value) {
		val = value;
		for (int i = 0; i < MAX_CHILDREN; i++) {
			children[i] = nullptr;
		}
	}
};

template <typename T>
void toTreeM(TreeNodeM<T>** root, const vector<vector<T>>& myTree)
{

	// Do this
#if 0
	int size = myTree.Size();
	int window = 0;
	root[0]->children[0]->val = mytree[0][0];
	for (int i = 1; i < size; i++)
	{
		if (i % 5 != 1)
		{
			for (int j = 0; j < 5; j++)
				// window size to added on ever level
			{
				root[i]->children[j] = myTree[i][j];
			}
		}
	}
#endif



};


template<typename T>
vector<T> trackerM(TreeNodeM<T>* root, T valA, T valB, T& result_node)
{
	// Do this
	return {};
}
// Declarations for utility functions
// Enums
enum PrintType {
	Invalid,
	PreOrder,
	InOrder,
	PostOrder
};
vector<vector<int>> get_treedata(string& spec0_PreOrder, string& spec0_InOrder);
template <typename T> void showLevelOrderM(TreeNodeM<T>* tree);
template <typename T> void treeToStringM(string& out, TreeNodeM<T>* tree, PrintType pr);
template <typename T> void validateResultM(TreeNodeM<T>* tree, const string& inOrderSpec, const string& preOrderSpec);
#if 0
int main()
{
	// Test 1
	cout << "\nTEST 1\n";

	string spec0_PreOrder;
	string spec0_InOrder;
	vector<vector<int>> spec0 = get_treedata(spec0_PreOrder, spec0_InOrder);

	TreeNodeM<int>* treeM = nullptr;
	toTreeM<int>(&treeM, spec0);
	showLevelOrderM<int>(treeM);
	validateResultM<int>(treeM, spec0_InOrder, spec0_PreOrder);

	cout << "\nTEST 2\n";
	cout << "Nodes to search: " << 100 << " " << 715 << endl;
	cout << "Correct answer: 1 - track 100 10 2 1 5 13 715" << endl;
	int res10 = -1;
	vector<int> v10 = trackerM<int>(treeM, 100, 715, res10);
	if (v10.size() == 0)
		cout << "No result found\n";
	else {
		cout << "Result: " << res10 << endl;
		for (auto val : v10) {
			cout << val << " ";
		}
		cout << endl;
	}

	cout << "\nTEST 3\n";
	cout << "Nodes to search: " << 77 << " " << 715 << endl;
	cout << "Correct answer: No result found" << endl;
	int res11 = -1;
	vector<int> v11 = trackerM<int>(treeM, 77, 715, res11);
	if (v11.size() == 0)
		cout << "No result found\n";
	else {
		cout << "Result: " << res11 << endl;
		for (auto val : v11) {
			cout << val << " ";
		}
		cout << endl;
	}

	cout << "\nTEST 4\n";
	cout << "Nodes to search: " << 100 << " " << 300 << endl;
	cout << "Correct answer: 10 - track 100 10 300" << endl;
	int res12 = -1;
	vector<int> v12 = trackerM<int>(treeM, 100, 300, res12);
	if (v12.size() == 0)
		cout << "No result found\n";
	else {
		cout << "Result: " << res12 << endl;
		for (auto val : v12) {
			cout << val << " ";
		}
		cout << endl;
	}

	return 0;
}
template <typename T, std::size_t N>
struct TmVal {
	std::array<T, MAX_CHILDREN> data;
	template<typename... Args>
	TmVal(Args... args) : data{ args... } {
		for (int i = 0; i < MAX_CHILDREN; i++) {
			if (i >= sizeof...(args)) {
				data[i] = -1;
			}
		}
	};
};
template<typename T, size_t N>
void add(vector<T>& current, TmVal<T, N> val) {
	for (int i = 0; i < N; i++) {
		current.push_back(val.data[i]);
	}
	return;
}
vector<vector<int>> get_treedata(string& preOrder, string& inOrder)
{
	vector<int> spec0_l0 = { 1 };
	vector<int> spec0_l1 = {};
	add(spec0_l1, TmVal<int, MAX_CHILDREN>(2, 3, 4, 5));
	vector<int> spec0_l2 = {};
	for (int i = 0; i < MAX_CHILDREN; ++i) {
		add(spec0_l2, TmVal<int, MAX_CHILDREN>(10 + i, 30 + i, 50 + i, 70 + i));
	}
	vector<int> spec0_l3 = {};
	for (int i = 0; i < (MAX_CHILDREN * MAX_CHILDREN); ++i) {
		add(spec0_l3, TmVal<int, MAX_CHILDREN>(100 + i, 300 + i, 500 + i, 700 + i));
	}
	vector<vector<int>> spec0 = { spec0_l0, spec0_l1, spec0_l2, spec0_l3 };

	preOrder = "1 2 10 100 300 500 700 30 101 301 501 701 50 102 302 502 702 70 103 303 503 703 3 11 105 305 505 705 31 106 306 506 706 51 107 307 507 707 71 108 308 508 708 4 12 110 310 510 710 32 111 311 511 711 52 112 312 512 712 72 113 313 513 713 5 13 115 315 515 715 33 116 316 516 716 53 117 317 517 717 73 118 318 518 718 ";
	inOrder = "100 300 10 500 700 101 301 30 501 701 2 102 302 50 502 702 103 303 70 503 703 105 305 11 505 705 106 306 31 506 706 3 107 307 51 507 707 108 308 71 508 708 1 110 310 12 510 710 111 311 32 511 711 4 112 312 52 512 712 113 313 72 513 713 115 315 13 515 715 116 316 33 516 716 5 117 317 53 517 717 118 318 73 518 718 ";

	return move(spec0);
}

template <typename T>
void showLevelOrderM(TreeNodeM<T>* tree) {
	if (!tree)
		return;
	vector<vector<T>> result;
	TreeNodeM<T>* current = tree;
	queue<TreeNodeM<T>*> queue;
	queue.push(current);
	int level = 1;
	while (!queue.empty()) {
		size_t queueSize = queue.size();
		vector<int> thisLevel = {};
		for (int levelIterator = 0; levelIterator < queueSize; levelIterator++) {
			current = queue.front();
			queue.pop();
			if (!current) {
				thisLevel.push_back(-1);
				for (int i = 0; i < MAX_CHILDREN; i++) {
					queue.push(nullptr);
				}
				continue;
			}
			thisLevel.push_back(current->val);
			for (int i = 0; i < MAX_CHILDREN; i++) {
				if (current->children[i])
					queue.push(current->children[i]);
				else
					queue.push(nullptr);
			}
		}
		bool allEmpty = true;
		for (int resultIterator = 0; resultIterator < thisLevel.size(); resultIterator++) {
			if (thisLevel[resultIterator] != -1) {
				allEmpty = false;
				break;
			}
		}
		if (!allEmpty) {
			result.push_back(thisLevel);
		}
		else {
			break;
		}
		level++;
	}
	cout << "Tree - Level order:\n";
	int maxChildren = MAX_CHILDREN;
	for (level = 0; level < result.size(); level++) {
		vector<T> levelData = result[level];
		int levelSize = (int)levelData.size();
		bool isFirst = false;
		for (int item = 0; item < levelSize; item++) {
			T node = levelData[item];
			isFirst = item % maxChildren == 0;
			if (isFirst && item > 0) {
				cout << "- ";
			}
			if (node != -1)
				cout << node << " ";
			else
				cout << "x ";
		}
		cout << endl;
	}
}
template <typename T>
void treeToStringM(string& out, TreeNodeM<T>* tree, PrintType pr) {
	if (!tree)
		return;
	switch (pr)
	{
	case PreOrder:
		out += to_string(tree->val);
		out += " ";
		for (int i = 0; i < MAX_CHILDREN; i++) {
			treeToStringM<T>(out, tree->children[i], pr);
		}
		break;
	case InOrder: // For odd amount of child nodes, the even number of child nodes are printed first.
		for (int i = 0; i < (MAX_CHILDREN / 2); i++) {
			treeToStringM<T>(out, tree->children[i], pr);
		}
		out += to_string(tree->val);
		out += " ";
		for (int i = (MAX_CHILDREN / 2); i < MAX_CHILDREN; i++) {
			treeToStringM<T>(out, tree->children[i], pr);
		}
		break;
	default:
		break;
	}
}
template <typename T>
void validateResultM(TreeNodeM<T>* tree, const string& inOrderSpec, const string& preOrderSpec) {
	string inorderString = "";
	string preorderString = "";
	bool inorderOK = false;
	bool preorderOK = false;

	treeToStringM<T>(inorderString, tree, InOrder);
	treeToStringM<T>(preorderString, tree, PreOrder);

	if (inorderString == inOrderSpec) inorderOK = true;
	if (preorderString == preOrderSpec) preorderOK = true;

	if (inorderOK && preorderOK) {
		cout << "Result ok" << endl;
	}
	else {
		if (!inorderOK) cout << "InOrder mismatch. Expected " << inOrderSpec << "- input was - " << inorderString << endl;
		if (!preorderOK) cout << "PreOrder mismatch. Expected " << preOrderSpec << "- input was - " << preorderString << endl;
	}

	return;
}

#endif