// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.

#ifndef MAP_INCLUDED
#define MAP_INCLUDED

template<typename KeyType, typename ValueType>
class MyMap
{
public:
    MyMap(){
        root = nullptr;
        m_size = 0;
    }
    ~MyMap(){
        recClear(root);
    }
    void clear(){
        recClear(root);
        m_size=0;
    }
    int size() const{
        return m_size;
    }
    
    // The associate method associates one item (key) with another (value). // If no association currently exists with that key, this method inserts // a new association into the tree with that key/value pair. If there is // already an association with that key in the tree, then the item
    // associated with that key is replaced by the second parameter (value). // Thus, the tree contains no duplicate keys.
    void associate(const KeyType& key, const ValueType& value){
        if (m_size==0){
            root = newNode(key, value);
            m_size++;
            return;
        }
        Node* curr = root;
        while(true){
            if (key==curr->key){
                curr->value = value;
                break;
            }
            //Go Left
            else if (key<curr->key){
                if (curr->left!=nullptr)
                    curr = curr->left;
                else{
                    curr->left = newNode(key, value);
                    m_size++;
                    break;
                }
            }
            //Go Right, key>curr->key
            else{
                if (curr->right!=nullptr)
                    curr = curr->right;
                else{
                    curr->right = newNode(key, value);
                    m_size++;
                    break;
                }
            }
            
        }
    }


	  // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const{
            Node * curr = root;
            
            while (curr!=nullptr){
                if (key==curr->key)
                    return &(curr->value);
                else if (key<curr->key)
                    curr = curr->left;
                else
                    curr = curr->right;
            }
            return nullptr;
    }

	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	  // C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:
    struct Node{
        KeyType key;
        ValueType value;
        Node* left;
        Node* right;
    };
    
    Node* root;
    
    int m_size;
    
    Node* newNode(const KeyType& key, const ValueType& value){
        Node* n = new Node;
        n->key = key;
        n->value = value;
        n->left = nullptr;
        n->right = nullptr;
        
        return n;
    }
        
    void recClear(Node* n){
        if (n==nullptr)
            return;
        recClear(n->left);
        recClear(n->right);
        
        delete n;
    }
        
        
        
        
        
};
    
#endif //MAP_INCLUDED
    
    
    
    
