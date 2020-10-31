#include <bits/stdc++.h>
using namespace std;

const int defaultBucketSize = 4;

struct Bucket {
	int localDepth;
	list<int> elements;

	Bucket() {
		localDepth = 0;
	}
};

class Directory {
private:
	int bucketCapacity;
	int globalDepth;
	vector<shared_ptr<Bucket>> bucketMap;

public:

	Directory() {
		globalDepth = 0;
		bucketCapacity = defaultBucketSize;
		shared_ptr<Bucket> newBucket = make_shared<Bucket>();
		bucketMap.push_back(newBucket);
	}

	Directory(int sz) {
		globalDepth = 0;
		bucketCapacity = sz;
		shared_ptr<Bucket> newBucket = make_shared<Bucket>();
		bucketMap.push_back(newBucket);
	}

	~Directory() {
		// use a smart pointer and ...
		// no need for this now

		// for (Bucket *b : bucketMap) {
		// 	delete b;
		// }
	}

	void display() {
		cout << "Global Depth : " << globalDepth << '\n';
		int idx = 0;
		for (shared_ptr<Bucket> b : bucketMap) {
			cout << "Idx : " << idx++ << '\n';
			cout << "\tLocal Depth : " << b -> localDepth << '\n';
			cout << "\tElements : { ";
			for (auto it = b -> elements.begin(); it != b -> elements.end(); ++it) {
				cout << *it;
				if (it != prev(b -> elements.end())) {
					cout << ", ";
				}
			}
			cout << " }\n";
		}
	}

	inline int calcIndex(int num) {
		return (num & ((1 << globalDepth) - 1));
	}

	void extend() {
		for (int i = 0; i < (1 << globalDepth); ++i) {
			bucketMap.push_back(bucketMap[i]);
		}
		globalDepth++;
	}

	void split(shared_ptr<Bucket> bucket) {
		shared_ptr<Bucket> newBucket = make_shared<Bucket>();

		if (bucket -> localDepth == globalDepth) {
			extend();
		}

		for (int i = 0; i < (1 << globalDepth); ++i) {
			if (bucketMap[i] == bucket &&
			        (i & (1 << (bucket -> localDepth)))) {
				bucketMap[i] = newBucket;
			}
		}

		auto itr = bucket -> elements.begin();
		while (itr != bucket -> elements.end()) {
			if ((*itr) & (1 << (bucket -> localDepth))) {
				int val = *itr;
				itr = bucket -> elements.erase(itr);
				newBucket -> elements.push_back(val);
			} else {
				itr++;
			}
		}

		bucket -> localDepth++;
		newBucket -> localDepth = bucket -> localDepth;
	}

	void insert(int num) {
		int idx = calcIndex(num);
		if ((int)(bucketMap[idx] -> elements.size()) == bucketCapacity) {
			split(bucketMap[idx]);
			insert(num);
		} else {
			bucketMap[idx] -> elements.push_back(num);
		}
	}

	void shrink() {
		globalDepth--;
		for (int i = 0; i < (1 << globalDepth); ++i) {
			bucketMap.pop_back();
		}
	}

	void merge(int idx1) {

		if (bucketMap[idx1] -> localDepth == 0) { return; }

		int idx2 = (idx1 ^ (1 << ((bucketMap[idx1] -> localDepth) - 1)));

		if (idx1 > idx2) {
			swap(idx1, idx2);
		}

		if (bucketMap[idx1] != bucketMap[idx2]) {
			if ((int)(bucketMap[idx1] -> elements.size() + bucketMap[idx2] -> elements.size()) <=
			        bucketCapacity) {

				bucketMap[idx1] -> elements.splice(bucketMap[idx1] -> elements.begin(),
				                                   bucketMap[idx2] -> elements);
				bucketMap[idx2].reset();
				bucketMap[idx2] = bucketMap[idx1];
				bucketMap[idx1] -> localDepth--;

				bool canShrink = true;
				for (shared_ptr<Bucket> b : bucketMap) {
					if (b -> localDepth == globalDepth) {
						canShrink = false;
						break;
					}
				}
				if (canShrink) {
					shrink();
					merge(idx1);
				}
			}
		}
	}


	void del(int num) {
		if (search(num)) {
			int idx = calcIndex(num);
			auto itr = bucketMap[idx] -> elements.begin();
			while (itr != bucketMap[idx] -> elements.end()) {
				if (*itr == num) {
					bucketMap[idx] -> elements.erase(itr);
					break;
				}
				itr++;
			}
			merge(idx);
		} else {
			cout << "Element does not exist\n";
		}
	}

	bool search(int num) {
		int idx = calcIndex(num);
		for (int x : bucketMap[idx] -> elements) {
			if (x == num) {
				return true;
			}
		}
		return false;
	}

};

void menu(Directory &hmap) {

	while (true) {
		int choice = 0, num;
		cout << '\n';
		cout << "1. Display\n";
		cout << "2. Insert\n";
		cout << "3. Delete\n";
		cout << "4. Search\n";
		cout << "5. Exit\n";
		cout << "------------------- \n";
		cout << "Enter your choice : \n";
		cout << "------------------- \n";
		cin >> choice;

		switch (choice) {
		case 1:
			hmap.display();
			break;
		case 2:
			cout << "Enter the number to be inserted : \n";
			cin >> num;
			hmap.insert(num);
			break;
		case 3:
			cout << "Enter the number to be deleted : \n";
			cin >> num;
			hmap.del(num);
			break;
		case 4:
			cout << "Enter the number to be searched : \n";
			cin >> num;
			cout << (hmap.search(num) ? "Found\n" : "Not Found\n");
			break;
		case 5:
			return;
		default:
			cout << "Invalid choice!!\n";
		}
	}
}

int main() {

	int bucketCapacity;
	while (true) {
		cout << "Enter the Bucket capacity : \n";
		cin >> bucketCapacity;

		if (bucketCapacity > 0) {
			break;
		} else {
			cout << "Invalid Bucket capacity!!\n";
		}
	}

	Directory temp(bucketCapacity);

	menu(temp);

	return 0;
}
